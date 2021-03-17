import sys

def to_bytes(n):
    out = b""
    for _ in range(4):
        out = out + bytes([n % 256])
        n //= 256
    return out

if __name__ == "__main__":
    with open(sys.argv[1], "wb", buffering=0) as dest, open(sys.argv[2], "rb") as src:
        kernel = src.read()
        dest.write(to_bytes(len(kernel)))
        dest.write(kernel)
