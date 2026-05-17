#!/usr/bin/env python3
"""
Generates a large test file for the CGI script to serve.
Run this once before testing.
"""

import os
import sys
import random
import string

# ─────────────────────────────────────────────
#  CONFIG
# ─────────────────────────────────────────────
OUTPUT_PATH = "./largeFile"
SIZE_MB     = 100          # size of the file in MB
MODE        = "random"     # "random" = binary random data
                           # "text"   = readable ASCII text
                           # "zeros"  = all zeros (fastest, most compressible)
# ─────────────────────────────────────────────

CHUNK = 1024 * 1024  # 1MB at a time


def generate_chunk(mode, size):
    if mode == "random":
        return os.urandom(size)
    elif mode == "text":
        chars = (string.ascii_letters + string.digits + " \n").encode()
        return bytes(random.choice(chars) for _ in range(size))
    else:
        return b"\x00" * size


def main():
    target_bytes = SIZE_MB * 1024 * 1024

    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)

    print(f"Generating {SIZE_MB}MB {MODE} file → {OUTPUT_PATH}")

    written = 0
    with open(OUTPUT_PATH, "wb") as f:
        while written < target_bytes:
            remaining = target_bytes - written
            chunk_size = min(CHUNK, remaining)
            f.write(generate_chunk(MODE, chunk_size))
            written += chunk_size
            pct = written / target_bytes * 100
            print(f"\r  {written // (1024*1024)}MB / {SIZE_MB}MB  ({pct:.0f}%)", end="", flush=True)

    print(f"\nDone — {os.path.getsize(OUTPUT_PATH):,} bytes written to {OUTPUT_PATH}")


if __name__ == "__main__":
    main()