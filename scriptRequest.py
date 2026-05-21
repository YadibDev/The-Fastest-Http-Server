#!/usr/bin/env python3
"""Test server handling of a very large HTTP header key."""

import socket
import sys

HOST = "127.0.0.1"
PORT = 8082
REQUEST = (
    "GET  / SMTP\r\n"
    f"Host: {HOST}\r\n"
    f"HEADER: test-value\r\n"
    "Connection: close\r\n"
    "\r\n"
)

def send_request_and_print_response():
    try:
        with socket.create_connection((HOST, PORT), timeout=5) as s:
            s.sendall(REQUEST.encode())
            response = b""
            while True:
                data = s.recv(4096)
                if not data:
                    break
                response += data
            if response:
                print(response.decode(errors="replace"))
            else:
                print("No response received (server may have dropped connection).")
    except socket.timeout:
        print("ERROR: Connection timed out (server did not respond in time).")
    except ConnectionRefusedError:
        print(f"ERROR: Connection refused – is a server listening on {HOST}:{PORT}?")
    except Exception as e:
        print(f"ERROR: {e}")

if __name__ == "__main__":
    send_request_and_print_response()