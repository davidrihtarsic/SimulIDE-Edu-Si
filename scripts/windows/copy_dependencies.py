"""Copy transitive MinGW DLL imports for an EXE and its Qt plugins."""

import re
import shutil
import subprocess
import sys
from pathlib import Path


def copy_dependencies(package: Path, toolchain: Path) -> None:
    available = {p.name.lower(): p for p in toolchain.glob("*.dll")}
    pending = [p for p in package.rglob("*") if p.suffix.lower() in (".exe", ".dll")]
    scanned = set()
    while pending:
        binary = pending.pop()
        if binary in scanned:
            continue
        scanned.add(binary)
        imports = subprocess.check_output(
            ["objdump", "-p", str(binary)], text=True, errors="replace"
        )
        for name in re.findall(r"DLL Name:\s*(\S+)", imports):
            source = available.get(name.lower())
            if source is None:
                # Windows system DLLs are supplied by the OS, not MSYS2.
                continue
            destination = package / source.name
            if not destination.exists():
                shutil.copy2(source, destination)
                print(f"Copied {source.name}")
            pending.append(destination)


if __name__ == "__main__":
    copy_dependencies(Path(sys.argv[1]), Path(sys.argv[2]))
