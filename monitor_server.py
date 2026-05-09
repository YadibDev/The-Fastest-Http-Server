#!/usr/bin/env python3
"""
██████╗ ███████╗██████╗ ██╗   ██╗███████╗██████╗
██╔══██╗██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗
██████╔╝███████╗██████╔╝██║   ██║█████╗  ██████╔╝
██╔═══╝ ╚════██║██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗
██║     ███████║██║  ██║ ╚████╔╝ ███████╗██║  ██║
╚═╝     ╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝
Webserver Monitor — by PID | Ctrl+C to exit
"""

import os
import sys
import time
import argparse
import threading
import collections
from datetime import datetime, timedelta

try:
    import psutil
except ImportError:
    sys.exit("❌  psutil not installed — run:  pip install psutil rich")

try:
    from rich.console import Console
    from rich.table import Table
    from rich.panel import Panel
    from rich.layout import Layout
    from rich.live import Live
    from rich.text import Text
    from rich.columns import Columns
    from rich.progress import BarColumn, Progress, TextColumn
    from rich import box
    from rich.align import Align
except ImportError:
    sys.exit("❌  rich not installed — run:  pip install rich")


# ─── Settings ──────────────────────────────────────────────────────────────────
HISTORY_LEN   = 60          # number of sparkline points
REFRESH_HZ    = 1.0         # refresh rate per second
ALERT_CPU     = 85.0        # CPU% warning threshold
ALERT_MEM_MB  = 500.0       # RAM MB warning threshold
ALERT_FDS     = 1000        # file descriptor warning threshold
SPARKLINE_CHARS = "▁▂▃▄▅▆▇█"


# ─── Helpers ──────────────────────────────────────────────────────────────────

def sparkline(values: collections.deque, width: int = 20) -> str:
    """Convert a list of values into a textual horizontal sparkline"""
    if not values:
        return " " * width
    vmin, vmax = min(values), max(values)
    span = vmax - vmin or 1
    bars = [
        SPARKLINE_CHARS[int((v - vmin) / span * (len(SPARKLINE_CHARS) - 1))]
        for v in list(values)[-width:]
    ]
    return "".join(bars).rjust(width)


def fmt_bytes(n: float) -> str:
    for unit in ("B", "KB", "MB", "GB", "TB"):
        if n < 1024:
            return f"{n:,.1f} {unit}"
        n /= 1024
    return f"{n:,.1f} PB"


def fmt_duration(seconds: float) -> str:
    td = timedelta(seconds=int(seconds))
    h, rem = divmod(td.seconds, 3600)
    m, s   = divmod(rem, 60)
    days   = td.days
    if days:
        return f"{days}d {h:02d}:{m:02d}:{s:02d}"
    return f"{h:02d}:{m:02d}:{s:02d}"


def color_pct(val: float, warn: float = 70, danger: float = 90) -> str:
    if val >= danger: return "bold red"
    if val >= warn:   return "bold yellow"
    return "bold green"


def bar_str(val: float, width: int = 20, warn: float = 70, danger: float = 90) -> Text:
    filled  = int(val / 100 * width)
    empty   = width - filled
    style   = color_pct(val, warn, danger)
    return Text("█" * filled + "░" * empty, style=style)


# ─── Data Collection ─────────────────────────────────────────────────────────────

class ServerStats:
    """Collects process statistics and maintains history for plotting"""

    def __init__(self, pid: int):
        try:
            self.proc = psutil.Process(pid)
        except psutil.NoSuchProcess:
            sys.exit(f"❌  No process with PID: {pid}")

        # Initialize cpu_percent once (first reading is always 0.0)
        self.proc.cpu_percent(interval=None)

        self.pid         = pid
        self.start_time  = datetime.now()
        self.cpu_hist    = collections.deque([0.0] * HISTORY_LEN, maxlen=HISTORY_LEN)
        self.mem_hist    = collections.deque([0.0] * HISTORY_LEN, maxlen=HISTORY_LEN)
        self.net_recv_prev = 0
        self.net_sent_prev = 0
        self.net_recv_rate = 0.0
        self.net_sent_rate = 0.0
        self.alerts      : list[str] = []
        self._lock       = threading.Lock()
        self._snapshot   : dict      = {}

    def _get_net_conns(self) -> tuple[int, int, int]:
        """Returns (ESTABLISHED, TIME_WAIT, CLOSE_WAIT) counts for the process"""
        try:
            conns = self.proc.connections(kind="inet")
            est = sum(1 for c in conns if c.status == "ESTABLISHED")
            tw  = sum(1 for c in conns if c.status == "TIME_WAIT")
            cw  = sum(1 for c in conns if c.status == "CLOSE_WAIT")
            return est, tw, cw
        except (psutil.AccessDenied, psutil.NoSuchProcess):
            return 0, 0, 0

    def collect(self):
        """Collects all data into a snapshot"""
        try:
            proc = self.proc
            with proc.oneshot():
                cpu   = proc.cpu_percent(interval=None)
                mem   = proc.memory_info()
                mempct= proc.memory_percent()
                try:
                    fds   = proc.num_fds()
                except (psutil.AccessDenied, AttributeError):
                    fds   = -1
                try:
                    thrs  = proc.num_threads()
                except (psutil.AccessDenied, AttributeError):
                    thrs  = -1
                try:
                    status = proc.status()
                except psutil.NoSuchProcess:
                    status = "dead"
                try:
                    nice  = proc.nice()
                except (psutil.AccessDenied, AttributeError):
                    nice  = "?"
                create_time = proc.create_time()

            # Network — system-wide (single process doesn't have network io_counters)
            net = psutil.net_io_counters()
            recv_rate = max(0, net.bytes_recv - self.net_recv_prev)
            sent_rate = max(0, net.bytes_sent - self.net_sent_prev)
            self.net_recv_prev = net.bytes_recv
            self.net_sent_prev = net.bytes_sent
            self.net_recv_rate = recv_rate
            self.net_sent_rate = sent_rate

            # Process I/O
            try:
                io = proc.io_counters()
                io_read  = io.read_bytes
                io_write = io.write_bytes
            except (psutil.AccessDenied, AttributeError):
                io_read = io_write = -1

            est, tw, cw = self._get_net_conns()

            uptime_sec = time.time() - create_time

            # Update history
            self.cpu_hist.append(cpu)
            self.mem_hist.append(mem.rss / 1024 / 1024)

            # Check alerts
            self.alerts = []
            if cpu >= ALERT_CPU:
                self.alerts.append(f"🔴 High CPU: {cpu:.1f}%")
            if mem.rss / 1024 / 1024 >= ALERT_MEM_MB:
                self.alerts.append(f"🔴 High RAM: {mem.rss/1024/1024:.1f} MB")
            if fds != -1 and fds >= ALERT_FDS:
                self.alerts.append(f"🔴 High FDs: {fds}")

            with self._lock:
                self._snapshot = {
                    "cpu":        cpu,
                    "mem_rss":    mem.rss,
                    "mem_vms":    mem.vms,
                    "mem_pct":    mempct,
                    "fds":        fds,
                    "threads":    thrs,
                    "status":     status,
                    "nice":       nice,
                    "uptime":     uptime_sec,
                    "net_recv":   net.bytes_recv,
                    "net_sent":   net.bytes_sent,
                    "recv_rate":  recv_rate,
                    "sent_rate":  sent_rate,
                    "io_read":    io_read,
                    "io_write":   io_write,
                    "conns_est":  est,
                    "conns_tw":   tw,
                    "conns_cw":   cw,
                    "cpu_hist":   list(self.cpu_hist),
                    "mem_hist":   list(self.mem_hist),
                    "alerts":     list(self.alerts),
                    "ts":         datetime.now(),
                }

        except psutil.NoSuchProcess:
            sys.exit(f"\n⚠️  Process {self.pid} terminated or no longer exists.")

    def snapshot(self) -> dict:
        with self._lock:
            return dict(self._snapshot)


# ─── User Interface ────────────────────────────────────────────────────────────

def build_header(s: dict, pid: int, proc_name: str) -> Panel:
    status_color = {
        "running":  "green",
        "sleeping": "cyan",
        "disk-sleep":"yellow",
        "dead":     "red",
        "zombie":   "red",
    }.get(s.get("status",""), "white")

    ts_str = s["ts"].strftime("%H:%M:%S") if "ts" in s else "--"
    uptime = fmt_duration(s.get("uptime", 0))

    title = Text()
    title.append("● PSERVER ", style="bold green")
    title.append(f"PID:{pid}  ", style="bold white")
    title.append(f"{proc_name}  ", style="cyan")
    title.append(f"[{s.get('status','?')}]", style=f"bold {status_color}")

    sub = Text()
    sub.append(f"  ⏱ uptime: {uptime}   ", style="dim")
    sub.append(f"🕐 {ts_str}   ", style="dim")
    sub.append(f"nice: {s.get('nice','?')}   ", style="dim")
    sub.append(f"threads: {s.get('threads','?')}", style="dim")

    combined = Text.assemble(title, "\n", sub)
    return Panel(combined, border_style="green dim", padding=(0, 1))


def build_cpu_panel(s: dict) -> Panel:
    cpu = s.get("cpu", 0.0)
    hist = s.get("cpu_hist", [0] * HISTORY_LEN)
    avg  = sum(hist[-10:]) / max(len(hist[-10:]), 1)
    peak = max(hist) if hist else 0

    t = Text()
    t.append(f"  {cpu:>5.1f}%  ", style=color_pct(cpu))
    t.append(bar_str(cpu))
    t.append(f"\n  avg(10s): {avg:>5.1f}%   peak: {peak:>5.1f}%\n", style="dim")
    spark = sparkline(collections.deque(hist), width=40)
    t.append(f"  {spark}", style="green dim")

    return Panel(t, title="[bold]CPU[/bold]", border_style="green dim", padding=(0,0))


def build_mem_panel(s: dict) -> Panel:
    rss  = s.get("mem_rss", 0) / 1024 / 1024
    vms  = s.get("mem_vms", 0) / 1024 / 1024
    pct  = s.get("mem_pct", 0.0)
    hist = s.get("mem_hist", [0] * HISTORY_LEN)
    peak = max(hist) if hist else 0

    t = Text()
    t.append(f"  RSS: {rss:>7.1f} MB  ", style=color_pct(pct, 60, 80))
    t.append(bar_str(pct, warn=60, danger=80))
    t.append(f"  {pct:.1f}%\n", style=color_pct(pct, 60, 80))
    t.append(f"  VMS: {vms:>7.1f} MB   peak: {peak:.1f} MB\n", style="dim")
    spark = sparkline(collections.deque(hist), width=40)
    t.append(f"  {spark}", style="yellow dim")

    return Panel(t, title="[bold]RAM[/bold]", border_style="yellow dim", padding=(0,0))


def build_net_panel(s: dict) -> Panel:
    recv_r = fmt_bytes(s.get("recv_rate", 0)) + "/s"
    sent_r = fmt_bytes(s.get("sent_rate", 0)) + "/s"
    total_r = fmt_bytes(s.get("net_recv", 0))
    total_s = fmt_bytes(s.get("net_sent", 0))
    est = s.get("conns_est", 0)
    tw  = s.get("conns_tw", 0)
    cw  = s.get("conns_cw", 0)

    t = Text()
    t.append(f"  ↓ recv: {recv_r:>12}   total: {total_r}\n", style="cyan")
    t.append(f"  ↑ sent: {sent_r:>12}   total: {total_s}\n", style="magenta")
    t.append(f"\n  ESTABLISHED: ", style="dim")
    t.append(f"{est}", style="bold green")
    t.append(f"   TIME_WAIT: ", style="dim")
    t.append(f"{tw}", style="yellow")
    t.append(f"   CLOSE_WAIT: ", style="dim")
    t.append(f"{cw}\n", style="red")

    return Panel(t, title="[bold]Network[/bold]", border_style="blue dim", padding=(0,0))


def build_io_panel(s: dict) -> Panel:
    fds     = s.get("fds", -1)
    io_r    = s.get("io_read", -1)
    io_w    = s.get("io_write", -1)

    t = Text()
    fds_str = str(fds) if fds != -1 else "N/A"
    fds_color = "red" if fds != -1 and fds >= ALERT_FDS else "green"
    t.append(f"  File Descriptors: ", style="dim")
    t.append(f"{fds_str}\n", style=f"bold {fds_color}")

    if io_r != -1:
        t.append(f"  I/O read:  {fmt_bytes(io_r)}\n", style="dim")
        t.append(f"  I/O write: {fmt_bytes(io_w)}\n", style="dim")
    else:
        t.append("  I/O: requires sudo\n", style="dim red")

    return Panel(t, title="[bold]I/O & FDs[/bold]", border_style="magenta dim", padding=(0,0))


def build_alerts_panel(alerts: list[str]) -> Panel | None:
    if not alerts:
        return None
    t = Text()
    for a in alerts:
        t.append(f"  {a}\n", style="bold red")
    return Panel(t, title="[bold red]⚠ Alerts[/bold red]", border_style="red", padding=(0,0))


def render(stats: ServerStats, proc_name: str, console: Console):
    s = stats.snapshot()
    if not s:
        return

    layout = Layout()
    layout.split_column(
        Layout(name="header",  size=4),
        Layout(name="top",     size=7),
        Layout(name="bottom",  size=7),
        Layout(name="alerts",  size=3),
    )

    layout["header"].update(build_header(s, stats.pid, proc_name))

    layout["top"].split_row(
        Layout(build_cpu_panel(s), name="cpu"),
        Layout(build_mem_panel(s), name="mem"),
    )
    layout["bottom"].split_row(
        Layout(build_net_panel(s), name="net"),
        Layout(build_io_panel(s),  name="io"),
    )

    alert_panel = build_alerts_panel(s.get("alerts", []))
    if alert_panel:
        layout["alerts"].update(alert_panel)
    else:
        ok = Text("  ✓ All indicators are within normal limits", style="green dim")
        layout["alerts"].update(Panel(ok, border_style="green dim", padding=(0,0)))

    return layout


# ─── Entry Point ──────────────────────────────────────────────────────────────

def parse_args():
    parser = argparse.ArgumentParser(
        description="Advanced Webserver Monitor",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument(
        "pid", type=int, nargs="?", default=None,
        help="PID of the server (optional — shows process list if omitted)"
    )
    parser.add_argument(
        "--interval", "-i", type=float, default=REFRESH_HZ,
        help=f"Seconds between updates (default: {REFRESH_HZ})"
    )
    parser.add_argument(
        "--alert-cpu", type=float, default=ALERT_CPU,
        help=f"CPU warning threshold %% (default: {ALERT_CPU})"
    )
    parser.add_argument(
        "--alert-mem", type=float, default=ALERT_MEM_MB,
        help=f"RAM warning threshold in MB (default: {ALERT_MEM_MB})"
    )
    return parser.parse_args()


def pick_pid_interactively() -> int:
    """Shows a list of processes and asks the user to select a PID"""
    console = Console()
    table = Table(title="Running Processes", box=box.SIMPLE_HEAVY, style="green dim")
    table.add_column("PID",  style="cyan",  justify="right")
    table.add_column("Name", style="white")
    table.add_column("CPU%", style="yellow", justify="right")
    table.add_column("RAM",  style="magenta", justify="right")
    table.add_column("Status", style="dim")

    procs = []
    for p in psutil.process_iter(["pid", "name", "status", "memory_info"]):
        try:
            info = p.info
            cpu  = p.cpu_percent(interval=None)
            mem  = info["memory_info"].rss / 1024 / 1024 if info["memory_info"] else 0
            procs.append((info["pid"], info["name"], cpu, mem, info["status"]))
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass

    procs.sort(key=lambda x: x[3], reverse=True)
    for pid, name, cpu, mem, status in procs[:30]:
        table.add_row(str(pid), name, f"{cpu:.1f}", f"{mem:.1f} MB", status)

    console.print(table)
    return int(console.input("\n[bold green]Enter server PID: [/bold green]"))


def main():
    global ALERT_CPU, ALERT_MEM_MB
    args = parse_args()

    ALERT_CPU    = args.alert_cpu
    ALERT_MEM_MB = args.alert_mem

    pid = args.pid if args.pid else pick_pid_interactively()

    try:
        proc_name = psutil.Process(pid).name()
    except psutil.NoSuchProcess:
        sys.exit(f"❌  No process with PID: {pid}")

    stats   = ServerStats(pid)
    console = Console()

    # Initial warm-up to avoid zero readings
    stats.collect()
    time.sleep(0.5)
    stats.collect()

    console.clear()

    try:
        with Live(
            render(stats, proc_name, console),
            console=console,
            refresh_per_second=int(1 / args.interval),
            screen=True,
        ) as live:
            while True:
                time.sleep(args.interval)
                stats.collect()
                layout = render(stats, proc_name, console)
                if layout:
                    live.update(layout)

    except KeyboardInterrupt:
        console.clear()
        console.print("\n[green]✓ Monitoring stopped.[/green]\n")
    except psutil.NoSuchProcess:
        console.print(f"\n[red]⚠ Process {pid} terminated.[/red]\n")


if __name__ == "__main__":
    main()