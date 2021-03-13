import argparse
import filecmp
import shutil
import subprocess as sp
from dataclasses import InitVar, dataclass, field
from pathlib import Path
from sys import exit
from typing import Iterable

from rich.console import Console

RUN_CROSS = "./dockcross-linux-aarch64"

# lazy init
recipies: Iterable = None
targets: dict
console: None


def init_global():
    global recipies, targets, console
    recipies = [
        BuildProject(
            keyword="impl-c",
            project_folder="impl-c",
            target_folder="res/kernel",
            target_files=["kernel8.img", "kernel8.elf"],
            run_cross=True,
        ),
        BuildProject(
            keyword="bootloader",
            project_folder="bootloader",
            target_folder="res/bootloader",
            target_files=["kernel8.img", "kernel8.elf"],
            run_cross=True,
        ),
        BuildProject(
            keyword="impl-rs",
            project_folder="impl-rs",
            target_folder="res/kernel",
            target_files=["kernel8.img", "kernel8.elf"],
            run_cross=False,
        ),
    ]
    targets = {r.keyword: r for r in recipies}
    console = Console()


def main():
    global console
    args = TypedArgs.parse()
    target = targets.get(args.target, None)
    if not target:
        console.print(f"not valid target name: {args.target}")
        exit(1)

    op = args.operation
    if op not in ["build", "clean"]:
        console.print(f"not valid operation: {args.operation}")
        exit(1)

    success: bool = getattr(target, op)()
    if not success:
        exit(1)


@dataclass
class TypedArgs:
    target: str
    operation: str

    @classmethod
    def parse(cls):
        p = argparse.ArgumentParser(description="build items")
        p.add_argument("target", help="[" + "|".join([i for i in targets.keys()]) + "]")
        p.add_argument("op", help="[build|clean]")
        arg = p.parse_args()
        return cls(target=arg.target, operation=arg.op)


@dataclass
class BuildProject:
    keyword: str
    project_folder: str
    target_folder: str
    target_files: Iterable[str]
    run_env: str = field(init=False)

    run_cross: InitVar[bool] = False

    def __post_init__(self, run_cross):
        self.run_env = RUN_CROSS if run_cross else ""

    def build(self) -> bool:
        Path(self.target_folder).mkdir(exist_ok=True)
        if run_cmd(f"{self.run_env} make -C {self.project_folder}") == False:
            return False
        if (
            copy2_dir(
                file_names=self.target_files,
                src_dir=self.project_folder,
                dst_dir=self.target_folder,
            )
            == False
        ):
            return False
        return True

    def clean(self) -> bool:
        shutil.rmtree(self.target_folder, ignore_errors=True)
        if run_cmd(f"{self.run_env} make -C {self.project_folder} clean") == False:
            return False
        return True


def is_content_different(a: Path, b: Path):
    return not filecmp.cmp(a, b)


def run_cmd(args: str) -> bool:
    console.print(f"🔥[bold green]{args}[reset]")
    args = args.split()
    try:
        if args[0] == RUN_CROSS:
            sp.run(" ".join(args), check=True, shell=True)
        else:
            sp.run(args, check=True)
    except sp.CalledProcessError:
        return False
    return True


def copy2_dir(file_names: Iterable[str], src_dir: str, dst_dir: str) -> bool:
    try:
        a, b = Path(src_dir), Path(dst_dir)
        if not b.exists():
            b.mkdir(exist_ok=True)
        for f in file_names:
            src, dst = a / f, b / f
            if not dst.exists() or is_content_different(src, dst):
                shutil.copy2(src, dst)
                console.print(f"✅ [bold green]copy file: [red]{src} => {dst}[reset]")
    except Exception:
        return False
    return True


if __name__ == "__main__":
    init_global()
    main()