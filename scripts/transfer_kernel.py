# Copyright (C) 2021 IanChen (ianchen-tw@github)

import argparse
import time
from dataclasses import dataclass
from typing import Any, Iterable

import rich.progress as rp
import serial
from rich.console import Console


@dataclass
class TypedArgs:
    image: str
    tty: str
    dry: str

    @classmethod
    def parse(cls):
        p = argparse.ArgumentParser(description="Send kernel image via tty device")
        p.add_argument(
            "--dry",
            dest="dry",
            action="store_true",
            help="dry run, not opening any device at all",
        )
        p.add_argument("image")
        p.add_argument("tty")
        arg = p.parse_args()
        return cls(image=arg.image, tty=arg.tty, dry=arg.dry)


def encode_literal_num(n: int, size: int) -> bytes:
    """Send numbers as literal string in decimal
    `size`: target length of the byte str
    example: 123 -> ["0","0",... , "1","2","3"]
    """
    s = str(n)
    s = "0" * (size - len(s)) + s
    # prevent overflow
    assert s[0] == "0"
    return s.encode("utf-8")


def main():
    console = Console()
    arg = TypedArgs.parse()

    console.print(f"Load image from [dim white]{arg.image}", style="bold green")
    img = KernelImg(file_path=arg.image)
    console.print(
        f"    :package:image size: [white]{img.size()} bytes", style="bold green"
    )

    tty = SerialSender(portname=arg.tty, dry=arg.dry)

    # Protocal
    # 1. send command to use
    tty.send("load\n".encode("utf-8"))

    # 2. send header1: file size(32 bytes literal dec value)
    size_bin = encode_literal_num(img.size(), 32)
    tty.send(size_bin)

    # 3. send header2: check_sum(32 bytes literal dec value)
    check_sum = encode_literal_num(img.check_sum(), 32)
    tty.send(check_sum)

    # 4. send the whole kernel in binary format
    total_bytes = img.size()
    with TransferProgress(console, total_bytes) as p:
        local_checksum = 0
        for i, b in enumerate(img.bytes()):
            local_checksum += int(b[0])
            local_checksum %= 856039
            if i < 5:
                # list of 1 bytes
                d = b[0]
                console.print(
                    f"byte [green]{i}[reset]: {d:02x}(hex), {d:3d}(dec), checksum:{local_checksum}"
                )
            if i == 5:
                console.print("[green] omitted...[reset]")
            if i > img.size() - 5:
                d = b[0]
                console.print(
                    f"byte [green]{i}[reset]: {d:02x}(hex), {d:3d}(dec), checksum:{local_checksum}"
                )
            tty.send(b)
            p.advance_bytes(1)


class SerialSender:
    def __init__(self, portname: str, baud: int = 115200, dry=True):
        self.portname: str = portname
        self.baud: int = baud
        self.dry: bool = dry
        if not dry:
            self.serial: Any = serial.Serial(self.portname, self.baud)

    def _write(self, data):
        if self.dry:
            time.sleep(0.0003)
        else:
            self.serial.write(data)

    def sendln(self, s: str):
        s += "\n"
        for c in s:
            self._write(c.encode())

    def send(self, data: bytes):
        assert type(data) == bytes
        self._write(data)


class KernelImg:
    def __init__(self, file_path: str):
        self.file_path: str = file_path
        with open(self.file_path, "rb") as f:
            self.data = f.read()

    def size(self) -> int:
        return len(self.data)

    def bytes(self) -> Iterable[bytes]:
        """List of data splitted by 1 byte"""
        return [b"%c" % d for d in self.data]

    def check_sum(self) -> int:
        # mod: 0856039
        # 😎 my student id
        return sum([int(b) for b in self.data]) % 856039


class TransferProgress:
    def __init__(self, console: Console, total_bytes: int):
        self.progress = rp.Progress(
            rp.TextColumn("[bold blue]{task.description}", justify="right"),
            rp.BarColumn(bar_width=None),
            "[progress.percentage]{task.percentage:>3.1f}%",
            rp.DownloadColumn(),
            rp.TimeRemainingColumn(),
            console=console,
        )
        self.task: int = self.progress.add_task(
            description="transfer image...", total=total_bytes
        )

    def advance_bytes(self, bytes: int):
        self.progress.update(self.task, advance=bytes)

    def __enter__(self):
        self.progress.start()
        return self

    def __exit__(self, *argv):
        self.progress.stop()


if __name__ == "__main__":
    main()
