import serial
import argparse
import os
import time

filename = "uartboot.img"

parser = argparse.ArgumentParser()
parser.add_argument("--file", help="image file")
parser.add_argument("--tty", help="target tty")
parser.add_argument("--baudrate", help="baudrate")

args = parser.parse_args()
img_name = args.file
port = args.tty
baudrate = args.baudrate


def read_img(image, size):
    print(image)
    with open(image, "rb") as f:
        img = f.read(size)
        return img


def send_mes(mes):
    s.write(mes.encode())


with serial.Serial(port, baudrate) as s:
    size = os.path.getsize(img_name)

    print("Start Loading...")
    send_mes("U\n")
    send_mes("U\n")
    mes = s.readline()
    print("Message sending...")
    print(mes)

    print("Image size: ", size)
    send_mes(f"{size}\n")
    mes = s.readline()
    print(mes)

    img = read_img(img_name, size)
    s.write(img)
    mes = s.readline()
    print(mes)
