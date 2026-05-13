#!/usr/bin/env python3

import os
import html

# Required CGI Header
print("Content-Type: text/html\r\n\r\n")
print()  # Blank line to separate headers from body

# Format environment variables into table rows
rows = ""
for key, value in sorted(os.environ.items()):
    rows += f"""
    <tr>
        <td class="key">{html.escape(key)}</td>
        <td class="value">{html.escape(value)}</td>
    </tr>"""

# HTML Template with Stealth Aesthetics
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI_ENV_INSPECTOR</title>
    <style>
        body {{
            background-color: #222831;
            color: #EEEEEE;
            font-family: 'Courier New', Courier, monospace;
            margin: 0;
            padding: 40px;
            line-height: 1.5;
        }}

        .container {{
            max-width: 1200px;
            margin: 0 auto;
        }}

        header {{
            border-bottom: 2px solid #8BC34A;
            margin-bottom: 30px;
            padding-bottom: 10px;
        }}

        h1 {{
            color: #8BC34A;
            margin: 0;
            font-size: 1.4rem;
            letter-spacing: 1px;
        }}

        .terminal-box {{
            background-color: #393E46;
            border-radius: 4px;
            box-shadow: 0 8px 16px rgba(0,0,0,0.5);
            overflow: hidden;
        }}

        table {{
            width: 100%;
            border-collapse: collapse;
        }}

        th {{
            background-color: #2D333B;
            color: #FF9800;
            text-align: left;
            padding: 12px 20px;
            font-size: 0.9rem;
            border-bottom: 2px solid #222831;
        }}

        td {{
            padding: 10px 20px;
            border-bottom: 1px solid #4E545C;
            font-size: 0.85rem;
            word-break: break-all;
        }}

        .key {{
            color: #8BC34A;
            font-weight: bold;
            white-space: nowrap;
            width: 30%;
        }}

        .value {{
            color: #EEEEEE;
        }}

        tr:hover {{
            background-color: #434a54;
        }}

        .footer {{
            margin-top: 20px;
            color: #FF9800;
            font-size: 0.75rem;
            text-align: right;
        }}
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>[ SYSTEM_ENVIRONMENT_VARIABLES ]</h1>
        </header>

        <div class="terminal-box">
            <table>
                <thead>
                    <tr>
                        <th>KEY</th>
                        <th>VALUE</th>
                    </tr>
                </thead>
                <tbody>
                    {rows}
                </tbody>
            </table>
        </div>

        <div class="footer">
            RUN_LEVEL: CGI_ACTIVE // STDOUT_SUCCESS
        </div>
    </div>
</body>
</html>
""")