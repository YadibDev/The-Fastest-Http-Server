#!/usr/bin/python3
import os, sys, time

# 1. Headers - خادمك يجب أن يمرر هذه السطور للعميل
print("Content-Type: text/html\r\n\r\n")

print("<html><head><style>table {border-collapse: collapse; width: 100%;} th, td {border: 1px solid #ddd; padding: 8px;} tr:nth-child(even){background-color: #f2f2f2;}</style></head><body>")
print("<h1>🚀 Webserv CGI Ultimate Auditor</h1>")

# 2. اختبار الـ Timeout (إذا أرسلت ?sleep=5 في الـ URL)
query = os.environ.get("QUERY_STRING", "")
if "sleep=" in query:
    seconds = int(query.split("sleep=")[1].split("&")[0])
    print(f"<p>⚠️ Sleeping for {seconds} seconds to test your Timeout logic...</p>")
    sys.stdout.flush() # دفع البيانات فوراً
    time.sleep(seconds)

# 3. عرض المتغيرات البيئية (Environment Variables)
print("<h2>1. Environment Variables Audit</h2>")
print("<table><tr><th>Variable</th><th>Value</th></tr>")
for key in sorted(os.environ.keys()):
    print(f"<tr><td><b>{key}</b></td><td>{os.environ[key]}</td></tr>")
print("</table>")

# 4. اختبار الـ Standard Input (POST Body)
print("<h2>2. POST Data (stdin)</h2>")
if os.environ.get("REQUEST_METHOD") == "POST":
    try:
        content_length = int(os.environ.get("CONTENT_LENGTH", 0))
        if content_length > 0:
            body = sys.stdin.read(content_length)
            print(f"<p>Received {content_length} bytes:</p>")
            print(f"<pre style='background: #eee; padding: 10px;'>{body}</pre>")
        else:
            print("<p>POST request received, but CONTENT_LENGTH is 0.</p>")
    except Exception as e:
        print(f"<p style='color:red;'>Error reading stdin: {e}</p>")
else:
    print("<p>No POST data (Method is GET).</p>")

print("</body></html>")
