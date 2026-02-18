import socket
import threading
import random
import time

HOST = "127.0.0.1"
PORT = 8081
REQUESTS = 200
THREADS = 50


def worker(thread_id):
    for _ in range(REQUESTS // THREADS):
        try:
            client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client.connect((HOST, PORT))

            request = (
                "GET /index.html HTTP/1.1\r\n"
                f"Host: {HOST}\r\n"
                "User-Agent: TestClient/1.0\r\n"
                "Accept: */*\r\n"
                "Connection: close\r\n"
                "\r\n"
            )

            client.sendall(request.encode())

            while True:
                data = client.recv(4096)
                if not data:
                    break

            client.close()

            # random small delay like real users
            time.sleep(random.uniform(0.01, 0.1))

        except Exception as e:
            print("Error:", e)


threads = []

for i in range(THREADS):
    t = threading.Thread(target=worker, args=(i,))
    t.start()
    threads.append(t)

for t in threads:
    t.join()

print("Simulation complete.")
