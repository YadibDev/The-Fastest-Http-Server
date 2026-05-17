#!/usr/bin/env python3

import os
import sys
import html

# 1. CGI Header
print("Content-Type: text/html\r\n\r\n")

# 2. Read Body from stdin
# CGI servers pass the length of the body via the CONTENT_LENGTH environment variable
try:
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
except ValueError:
    content_length = 0

if content_length > 0:
    # Read exactly the number of bytes specified
    raw_body = sys.stdin.read(content_length)
    display_body = html.escape(raw_body)
else:
    display_body = "NO_BODY_DETECTED"

# 3. Format Environment Variables
env_rows = ""
for key, value in sorted(os.environ.items()):
    env_rows += f"""
    <tr>
        <td class="key">{html.escape(key)}</td>
        <td class="value">{html.escape(value)}</td>
    </tr>"""

# 4. Output Rendered HTML
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI_SYSTEM_INSPECTOR</title>
    <style>
        body {{
            background-color: #222831;
            color: #EEEEEE;
            font-family: 'Courier New', Courier, monospace;
            margin: 0;
            padding: 40px;
        }}
        .container {{ max-width: 1200px; margin: 0 auto; }}
        header {{ border-bottom: 2px solid #8BC34A; margin-bottom: 30px; }}
        h1 {{ color: #8BC34A; font-size: 1.4rem; }}
        h2 {{ color: #FF9800; font-size: 1.1rem; margin-top: 30px; border-left: 3px solid #FF9800; padding-left: 10px; }}
        
        .box {{
            background-color: #393E46;
            border-radius: 4px;
            padding: 15px;
            overflow-x: auto;
            box-shadow: 0 4px 10px rgba(0,0,0,0.4);
        }}
        
        pre {{
            margin: 0;
            color: #8BC34A;
            white-space: pre-wrap;
            word-break: break-all;
        }}
        
        table {{ width: 100%; border-collapse: collapse; margin-top: 10px; }}
        td {{ padding: 8px 15px; border-bottom: 1px solid #4E545C; font-size: 0.85rem; }}
        .key {{ color: #8BC34A; font-weight: bold; width: 30%; }}
        .value {{ color: #EEEEEE; word-break: break-all; }}
        tr:hover {{ background-color: #434a54; }}
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>SYSTEM_INSPECTOR_v1.0</h1>
        </header>

        

        <h2>OS_ENVIRONMENT_VARIABLES</h2>
        <div class="box">
            <table>
                {env_rows}
            </table>
        </div>
        <h2>STDIN_REQUEST_BODY</h2>
        <div class="box">
            <pre>{display_body}</pre>
        </div>
    </div>
</body>
</html>
""")