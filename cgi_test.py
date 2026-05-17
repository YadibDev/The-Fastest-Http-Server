#!/usr/bin/python3
import sys
import hashlib

# إخبار الخادم بنوع المحتوى فوراً
print("Content-Type: text/plain\r\n\r\n", end="")

# حساب الـ Hash للبيانات المتدفقة من stdin
sha256 = hashlib.sha256()
total_size = 0

try:
    while True:
        chunk = sys.stdin.buffer.read(100000) # قراءة 64KB في كل مرة
        if not chunk:
            break
        sha256.update(chunk)
        total_size += len(chunk)

    print(f"Status: Success\n")
    print(f"Received Size: {total_size} bytes\n")
    print(f"Received Hash: {sha256.hexdigest()}\n")
except Exception as e:
    print(f"Status: Error\n")
    print(f"Error: {str(e)}\n")