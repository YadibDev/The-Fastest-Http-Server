import random
import string
import json

def random_string(length=10):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def generate_realistic_payload():
    payload = {
        "user": {
            "id": random.randint(1000, 9999),
            "name": random_string(8),
            "email": f"{random_string(5)}@example.com"
        },
        "action": random.choice(["login", "purchase", "update_profile"]),
        "timestamp": random.randint(1600000000, 1800000000),
        "metadata": {
            "ip": ".".join(str(random.randint(0, 255)) for _ in range(4)),
            "device": random.choice(["mobile", "desktop", "tablet"])
        }
    }
    return json.dumps(payload, indent=2)

def chunk_encode(data, chunk_size=16):
    chunks = []
    for i in range(0, len(data), chunk_size):
        chunk = data[i:i+chunk_size]
        size = format(len(chunk), 'x')
        chunks.append(f"{size}\r\n{chunk}\r\n")
    chunks.append("0\r\n\r\n")
    return ''.join(chunks)

def chunk_decode(chunked_data):
    i = 0
    decoded = ""

    while True:
        # Read chunk size
        j = chunked_data.find("\r\n", i)
        size_str = chunked_data[i:j]
        size = int(size_str, 16)

        if size == 0:
            break

        # Move to chunk data
        i = j + 2
        chunk = chunked_data[i:i+size]
        decoded += chunk

        # Move past chunk and CRLF
        i += size + 2

    return decoded

def create_files():
    original_payload = generate_realistic_payload()

    # Encode
    chunked_body = chunk_encode(original_payload)

    # Decode
    unchunked_body = chunk_decode(chunked_body)

    # Save files
    with open("request.txt", "w", encoding="utf-8") as f:
        f.write(chunked_body)

    with open("unchunked_result.json", "w", encoding="utf-8") as f:
        f.write(unchunked_body)

    print("[+] Files created:")
    print(" - chunked_body.txt (chunked transfer encoding)")
    print(" - unchunked_result.json (decoded original payload)")

if __name__ == "__main__":
    create_files()