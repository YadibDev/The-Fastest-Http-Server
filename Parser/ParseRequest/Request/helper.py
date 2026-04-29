def chunk_encode(data: bytes, chunk_size=32):
    chunks = []
    
    for i in range(0, len(data), chunk_size):
        chunk = data[i:i+chunk_size]
        size = format(len(chunk), 'x').encode()  # hex size
        chunks.append(size + b"\r\n" + chunk + b"\r\n")
    
    chunks.append(b"0\r\n\r\n")  # end of chunks
    return b"".join(chunks)


def file_to_chunked(input_file, output_file, chunk_size=32):
    with open(input_file, "rb") as f:
        data = f.read()

    chunked_data = chunk_encode(data, chunk_size)

    with open(output_file, "wb") as f:
        f.write(chunked_data)

    print(f"[+] Chunked file written to: {output_file}")


if __name__ == "__main__":
    # Example usage
    file_to_chunked("nature.mp4", "video.mp4", chunk_size=32)