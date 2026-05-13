from flask import Flask, request, render_template_string
import os

print("content-type: text/html\r\n\r\n")

app = Flask(__name__)

# HTML Template with Embedded CSS
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Server Environment Inspector</title>
    <style>
        :root {
            --bg-color: #222831;
            --accent-green: #8BC34A;
            --accent-orange: #FF9800;
            --text-main: #EEEEEE;
            --card-bg: #393E46;
        }

        body {
            background-color: var(--bg-color);
            color: var(--text-main);
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
        }

        .container {
            max-width: 1000px;
            margin: 0 auto;
        }

        header {
            border-bottom: 2px solid var(--accent-green);
            margin-bottom: 30px;
            padding-bottom: 10px;
        }

        h1 { color: var(--accent-green); margin: 0; font-size: 1.5rem; }
        h2 { color: var(--accent-orange); font-size: 1.2rem; border-left: 4px solid var(--accent-orange); padding-left: 10px; }

        .data-section {
            background-color: var(--card-bg);
            border-radius: 8px;
            padding: 20px;
            margin-bottom: 30px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.3);
            overflow-x: auto;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            font-family: 'Courier New', Courier, monospace;
            font-size: 0.9rem;
        }

        th {
            text-align: left;
            color: var(--accent-green);
            padding: 10px;
            border-bottom: 1px solid #4E545C;
        }

        td {
            padding: 10px;
            border-bottom: 1px solid #4E545C;
            word-break: break-all;
        }

        tr:hover { background-color: #454d55; }

        .body-content {
            background: #1a1a1a;
            color: #dcdcdc;
            padding: 15px;
            border-radius: 4px;
            white-space: pre-wrap;
            border-left: 4px solid var(--accent-green);
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>SYSTEM_INSPECTOR_V1</h1>
        </header>

        <section>
            <h2>Incoming Request Body</h2>
            <div class="data-section">
                <div class="body-content">{{ request_body if request_body else "No body received." }}</div>
            </div>
        </section>

        <section>
            <h2>Environment Variables</h2>
            <div class="data-section">
                <table>
                    <thead>
                        <tr>
                            <th>Variable Key</th>
                            <th>Value</th>
                        </tr>
                    </thead>
                    <tbody>
                        {% for key, value in env_vars.items() %}
                        <tr>
                            <td>{{ key }}</td>
                            <td>{{ value }}</td>
                        </tr>
                        {% endfor %}
                    </tbody>
                </table>
            </div>
        </section>
    </div>
</body>
</html>
"""

@app.route('/', methods=['GET', 'POST', 'PUT'])
def index():
    # Extracting Environment Variables
    env_vars = dict(os.environ)
    
    # Extracting Request Body (decoded to string)
    request_body = request.get_data(as_text=True)

    return render_template_string(
        HTML_TEMPLATE, 
        env_vars=env_vars, 
        request_body=request_body
    )

if __name__ == '__main__':
    # Running on localhost:5000
    app.run(debug=True)