import socket
import threading
import random
import time

HOST = "127.0.0.1"
PORT = 8083
THREADS = 200        # increase carefully
REQUESTS_PER_THREAD = 200

success = 0
fail = 0
lock = threading.Lock()


def random_request():
    paths = [
        "/",
    ]

    headers = [
        "User-Agent: stress-test",
        "Accept: */*",
        "Connection: keep-alive",
    ]

    request = f"GET {random.choice(paths)} HTTP/1.1\r\n"
    request += f"Host: localhost\r\n"
    request += "\r\n".join(headers)
    request += "\r\n\r\n"

    return request


def worker():
    global success, fail

    for _ in range(REQUESTS_PER_THREAD):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(60)
            s.connect((HOST, PORT))

            req = random_request()
            s.sendall(req.encode())

            data = s.recv(1024 * 4)

            if data:
                with lock:
                    success += 1
            else:
                with lock:
                    fail += 1


        except Exception:
            with lock:
                fail += 1
    s.close()


threads = []

start = time.time()

for _ in range(THREADS):
    t = threading.Thread(target=worker)
    t.start()
    threads.append(t)

for t in threads:
    t.join()

end = time.time()

print("Finished")
print("Success:", success)
print("Fail:", fail)
print("Time:", round(end - start, 2), "seconds")