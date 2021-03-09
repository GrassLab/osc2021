# Copyright (C) 2021 IanChen (ianchen-tw@github)

import argparse
import dataclasses
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

    @classmethod
    def parse(cls):
        fields = [f.name for f in dataclasses.fields(cls)]
        p = argparse.ArgumentParser(description="Send kernel image via tty device")
        p.add_argument(
            "--dry",
            dest="dry",
            action="store_true",
            help="dry run, not opening any device at all",
        )
        for f in fields:
            p.add_argument(f)
        _args = p.parse_args()
        arg = cls(**{f: getattr(_args, f) for f in fields})
        arg.dry = _args.dry
        return arg


def main():
    console = Console()
    arg = TypedArgs.parse()

    console.print(f"Load image from [dim white]{arg.image}", style="bold green")
    img = KernelImg(file_path=arg.image)
    console.print(
        f"    :package:image size: [white]{img.size()} bytes", style="bold green"
    )

    tty = SerialSender(portname=arg.tty, dry=arg.dry)

    # send four bytes as filesize header
    size_bin = img.size().to_bytes(4, byteorder="big")
    tty.send(size_bin)

    # send the whole kernel in binary format
    total_bytes = img.size()
    with TransferProgress(console, total_bytes) as p:
        for b in img.bytes():
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
