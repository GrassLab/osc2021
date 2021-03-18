#!/usr/bin/env python3


tty_dev = input("Input tty device: ")
boot_file_path = input("Input Boot image file[../kernel8.img]: ")

if boot_file_path == "":
    boot_file_path = "../kernel8.img"

boot_file = open(boot_file_path, "rb")

with open(tty_dev, "w") as tty:
  tty.write("load\n")
  tty.close()

with open(tty_dev, "wb", buffering = 0) as tty:
  tty.write(boot_file.read())
  tty.close()

with open(tty_dev, "w") as tty:
  tty.write("c")
  tty.close()
