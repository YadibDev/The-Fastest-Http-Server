#!/usr/bin/env python3

import os
import sys
import urllib.parse

def print_headers():
    print("Content-Type: text/html")
    print()  # important: blank line separates headers from body

def get_query_params():
    query_string = os.environ.get("QUERY_STRING", "")
    return urllib.parse.parse_qs(query_string)

def main():
    print_headers()

    params = get_query_params()

    name = params.get("name", ["Guest"])[0]

    print("<html>")
    print("<head><title>CGI Python</title></head>")
    print("<body>")
    print(f"<h1>Hello, {name}</h1>")
    print("<p>This is a CGI Python script running on the server.</p>")
    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()