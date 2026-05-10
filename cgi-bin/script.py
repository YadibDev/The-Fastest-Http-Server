#!/usr/local/bin/python3.2
print("content-type: text/plan\r\n\r\n", end = "")       


import os

for key, value in os.environ.items():
    print(f'{key}={value}\n')
