import socket

def parse_request(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        raw = f.read()

    headers_part, body = raw.split("\n\n", 1)

    lines = headers_part.splitlines()
    request_line = lines[0]
    headers = lines[1:]

    method, path, version = request_line.split()

    header_dict = {}
    for h in headers:
        if ":" in h:
            key, value = h.split(":", 1)
            header_dict[key.strip()] = value.strip()

    return method, path, version, header_dict, body


def send_request(ip, port, file_path):
    method, path, version, headers, body = parse_request(file_path)

    # Ensure Host header exists
    if "Host" not in headers:
        headers["Host"] = f"{ip}:{port}"

    # Add Content-Length automatically
    if body:
        headers["Content-Length"] = str(len(body.encode('utf-8')))

    # Rebuild request
    request = f"{method} {path} {version}\r\n"
    for k, v in headers.items():
        request += f"{k}: {v}\r\n"
    request += "\r\n"
    request += body

    # Send via socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((ip, port))
        s.sendall(request.encode())

        response = s.recv(4096)
        print("Response:")
        print(response.decode(errors="ignore"))


if __name__ == "__main__":
    # Change these as needed
    TARGET_IP = "127.0.0.1"
    TARGET_PORT = 8081
    REQUEST_FILE = "request.txt"

    send_request(TARGET_IP, TARGET_PORT, REQUEST_FILE)