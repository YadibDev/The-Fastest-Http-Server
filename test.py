import socket

host = "127.0.0.1"
port = 8082
path = "/"

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((host, port))

request = (
    f"G ET {path} HTTP/1.1\r\n"
    f"Host: {host}:8081\r\n"
    f"Connection: close\r\n"
    f"\r\n"
)

sock.send(request.encode())

response = b""
while True:
    chunk = sock.recv(4096)
    if not chunk:
        break
    response += chunk

sock.close()

# طباعة الاستجابة كاملة
print(response.decode())