#!/usr/bin/env python3
import sys

# The "Beautiful" part: CSS for a glassmorphism look
CSS = "body{background:linear-gradient(45deg,#0f0c29,#302b63,#24243e);color:#fff;font-family:sans-serif;display:grid;place-items:center;height:100vh;margin:0}#c{background:rgba(255,255,255,0.1);backdrop-filter:blur(10px);padding:2rem;border-radius:20px;border:1px solid rgba(255,255,255,0.2);box-shadow:0 8px 32px 0 rgba(0,0,0,0.37);text-align:center}"

# The "Hard to Parse" part: Functional obfuscation
(lambda _, __, ___: [
    sys.stdout.write(f"Content-Type: text/html\r\n\r\n"),
    [sys.stdout.write(t) for t in (
        _('<html lang="en"><head><style>'), CSS, _('</style></head><body><div id="c">'),
        __("Python CGI: The Aesthetic Paradox"),
        ___("This page was generated via a single-expression lambda chain. It's readable by machines, but designed for the soul."),
        _('</div></body></html>')
    )]
])(
    lambda s: s,                                                # Identity function
    lambda t: f"<h1 style='letter-spacing:2px'>{t}</h1>",      # Title wrapper
    lambda p: f"<p style='opacity:0.8;line-height:1.6'>{p}</p>" # Paragraph wrapper
)