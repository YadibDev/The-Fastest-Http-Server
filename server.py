import socket
import time
HOST = "127.0.0.1"   # Server IP or hostname
PORT = 8082          # Server port

# HTTP request
request = (
    "GET /cgi-bin/script.py HTTP/1.1\r\n"
    f"Host: {HOST}\r\n"
    "Connection: close\r\n"
    "\r\n"
)

# Create TCP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server
    sock.connect((HOST, PORT))
    print(f"[+] Connected to {HOST}:{PORT}")

    # Send HTTP request
    sock.sendall(request.encode())
    print("[+] Request sent")

    # Shutdown writing side of socket
    # This tells the server:
    # "I finished sending data"
    # sock.shutdown(socket.SHUT_WR)
    # time.sleep(3)
    sock.shutdown(socket.SHUT_RD)
    print("[+] Write side shutdown")

    # Receive response
    response = b""

    while True:
        data = sock.recv(4096)
        print(data)
        if not data:
            break

        response += data

    print("[+] Response received:")
    print(response.decode(errors="ignore"))

finally:
    sock.close()
    print("[+] Socket closed")