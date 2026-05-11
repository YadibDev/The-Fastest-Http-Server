#!/usr/bin/python3
import os
import sys
import time
import html
import urllib.parse  # مكتبة قياسية لفك تشفير الروابط

# ─── الثوابت ───────────────────────────────────────────────
MAX_SLEEP_SECONDS   = 10     
MAX_BODY_BYTES      = 1024 * 1024  # 1 MB

# ─── دوال مساعدة ───────────────────────────────────────────

def escape(value: str) -> str:
    """تحويل الأحرف لمنع XSS"""
    return html.escape(str(value), quote=True)

def parse_query(query_string: str) -> dict:
    """تحليل QUERY_STRING مع فك التشفير (Handling %20, %2B, etc.)"""
    params = {}
    if not query_string:
        return params
    for part in query_string.split("&"):
        if "=" in part:
            key, _, val = part.partition("=")
            # فك تشفير الـ URL (مثل تحويل %20 إلى مسافة)
            key = urllib.parse.unquote_plus(key.strip())
            val = urllib.parse.unquote_plus(val.strip())
            params[key] = val
    return params

def send_headers():
    """إرسال الهيدرز بالصيغة القياسية لـ CGI"""
    sys.stdout.write("Content-Type: text/html\r\n")
    sys.stdout.write("X-Powered-By: Webserv-CGI-Auditor\r\n")
    sys.stdout.write("\r\n") # الفاصل الإلزامي بين الهيدر والبودي
    sys.stdout.flush()

def render_style() -> str:
    return """
    <style>
        body  { font-family: 'Courier New', monospace; margin: 2rem; background: #f4f7f6; }
        h1    { color: #2c3e50; border-left: 5px solid #2980b9; padding-left: 10px; }
        h2    { background: #2980b9; color: white; padding: 5px 10px; border-radius: 3px; }
        table { border-collapse: collapse; width: 100%; background: white; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        th, td{ border: 1px solid #ddd; padding: 10px; text-align: left; }
        th    { background: #34495e; color: white; }
        tr:hover { background: #f1f1f1; }
        pre   { background: #2d3436; color: #dfe6e9; padding: 15px; border-radius: 5px; 
                overflow-x: auto; white-space: pre-wrap; word-break: break-all; }
        .status-box { padding: 10px; margin: 10px 0; border-radius: 4px; border: 1px solid; }
        .warn { background: #fff3cd; color: #856404; border-color: #ffeeba; }
        .err  { background: #f8d7da; color: #721c24; border-color: #f5c6cb; }
        .ok   { background: #d4edda; color: #155724; border-color: #c3e6cb; }
    </style>
    """

# ─── اختبار Timeout ────────────────────────────────────────

def handle_sleep(params: dict):
    if "sleep" not in params:
        return

    try:
        seconds = int(params["sleep"])
    except (ValueError, TypeError):
        print('<div class="status-box err">⛔ قيمة sleep غير صالحة.</div>')
        return

    if seconds > MAX_SLEEP_SECONDS:
        print(f'<div class="status-box warn">⚠️ تم تقليص النوم من {seconds}s إلى {MAX_SLEEP_SECONDS}s.</div>')
        seconds = MAX_SLEEP_SECONDS

    print(f'<div class="status-box warn">⏳ جاري النوم لمدة {seconds} ثانية (CGI Timeout Test)...</div>')
    sys.stdout.flush() # ضروري جداً ليرسل الخادم الرسالة قبل النوم
    time.sleep(seconds)

# ─── عرض المتغيرات البيئية ─────────────────────────────────

def render_env_vars():
    print("<h2>1. Environment Variables Audit</h2>")
    print("<table>")
    print("<tr><th>Variable</th><th>Value</th></tr>")
    # عرض المتغيرات الهامة لمشروع Webserv أولاً
    important_vars = ['REQUEST_METHOD', 'QUERY_STRING', 'CONTENT_LENGTH', 'CONTENT_TYPE', 'PATH_INFO', 'SCRIPT_NAME', 'SERVER_PROTOCOL']
    
    for key in important_vars + sorted(list(set(os.environ.keys()) - set(important_vars))):
        if key in os.environ:
            print(f"<tr><td><b>{escape(key)}</b></td><td>{escape(os.environ[key])}</td></tr>")
    print("</table>")

# ─── عرض POST Body ─────────────────────────────────────────

def render_post_data():
    print("<h2>2. POST Data (stdin)</h2>")
    method = os.environ.get("REQUEST_METHOD", "GET").upper()

    if method != "POST":
        print("<p>الطلب ليس من نوع POST.</p>")
        return

    content_length = int(os.environ.get("CONTENT_LENGTH", 0))

    if content_length <= 0:
        print("<div class='status-box warn'>Payload Empty (CONTENT_LENGTH=0)</div>")
        return

    if content_length > MAX_BODY_BYTES:
        print(f'<div class="status-box err">⛔ الجسم ضخم جداً!</div>')
        return

    try:
        # قراءة البيانات الثنائية ثم فك تشفيرها
        body = sys.stdin.buffer.read(content_length)
        decoded = body.decode("utf-8", errors="replace")
        print(f"<div class='status-box ok'>✅ استُقبل {len(body)} بايت:</div>")
        print(f"<pre>{escape(decoded)}</pre>")
    except Exception as exc:
        print(f'<div class="status-box err">⛔ خطأ قراءة الحامل: {escape(str(exc))}</div>')

# ─── نقطة الدخول ───────────────────────────────────────────

def main():
    # 1. إرسال الهيدرز أولاً وقبل كل شيء
    send_headers()

    # 2. تحليل البارامترات
    query_string = os.environ.get("QUERY_STRING", "")
    params = parse_query(query_string)

    # 3. بناء الصفحة
    print(f"<!DOCTYPE html><html><head><meta charset='UTF-8'>{render_style()}</head><body>")
    print("<h1>🚀 Webserv CGI Auditor v2.0</h1>")

    handle_sleep(params)
    render_env_vars()
    render_post_data()

    print("<br><hr><center><small>CGI Process ID: " + str(os.getpid()) + "</small></center>")
    print("</body></html>")
    sys.stdout.flush()

if __name__ == "__main__":
    main()