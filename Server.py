import socket

def send_raw_http(host, port, raw_payload):
    print(f"Connecting to {host}:{port}...")
    try:
        # إنشاء سوكيت TCP
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.settimeout(2)
        client.connect((host, port))
        
        # إرسال الطلب الخام (يجب أن ينتهي بـ \r\n\r\n)
        client.sendall(raw_payload.encode())
        
        # استقبال الرد بالكامل
        response = b""
        while True:
            chunk = client.recv(4096)
            if not chunk:
                break
            response += chunk
            
        print("--- Raw Response Received ---")
        print(response.decode(errors='ignore'))
        client.close()
        
    except Exception as e:
        print(f"Socket Error: {e}")

# مثال لاختبار الـ Chunked Encoding أو طلبات خاطئة:
raw_http_request = (
    "GET /index.html HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Custom-Header: Testing\r\n"
    "\r\n"
)

send_raw_http("127.0.0.1", 8082, raw_http_request)