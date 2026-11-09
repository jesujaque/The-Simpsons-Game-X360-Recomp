#!/usr/bin/env python3
"""Empaqueta una release del port sin incluir contenido del juego.

Ejemplos:
    python tools/package_release.py --version 0.1.0 --doctor build/bin/SimpsonsDoctor
    python tools/package_release.py --version 0.2.0 --exe build/bin/SimpsonsRecomp.exe
"""

from __future__ import annotations

import argparse
import platform
import sys
import tarfile
import tempfile
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

COMMON_FILES = [
    "README.md",
    "LICENSE",
    "CHANGELOG.md",
    "config.example.toml",
    "docs/BUILDING.md",
    "docs/LEGAL.md",
    "docs/RELEASING.md",
]


def detect_platform() -> str:
    system = platform.system().lower()
    machine = platform.machine().lower()
    if machine in ("amd64", "x86_64"):
        arch = "x64"
    elif machine in ("aarch64", "arm64"):
        arch = "arm64"
    else:
        arch = machine or "unknown"
    if system.startswith("win"):
        return f"windows-{arch}"
    if system == "darwin":
        return f"macos-{arch}"
    return f"linux-{arch}"


def collect(staging: Path, exe: Path | None, doctor: Path | None) -> list[Path]:
    staging.mkdir(parents=True, exist_ok=True)
    files: list[Path] = []

    if exe:
        target = staging / exe.name
        target.write_bytes(exe.read_bytes())
        files.append(target)
    if doctor:
        target = staging / doctor.name
        target.write_bytes(doctor.read_bytes())
        files.append(target)

    for relative in COMMON_FILES:
        source = ROOT / relative
        if source.is_file():
            target = staging / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(source.read_bytes())
            files.append(target)

    return files


def write_zip(archive: Path, files: list[Path], base: Path) -> None:
    with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED) as handle:
        for path in files:
            handle.write(path, path.relative_to(base).as_posix())


def write_tar(archive: Path, files: list[Path], base: Path) -> None:
    with tarfile.open(archive, "w:gz") as handle:
        for path in files:
            handle.add(path, arcname=path.relative_to(base).as_posix())


def main() -> int:
    parser = argparse.ArgumentParser(description="Crea el archivo de una release")
    parser.add_argument("--version", required=True, help="versión sin la 'v' (ej. 0.1.0)")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--exe", type=Path, help="SimpsonsRecomp (build completa)")
    group.add_argument("--doctor", type=Path, help="SimpsonsDoctor (build de herramientas)")
    parser.add_argument("--out", type=Path, default=ROOT / "dist")
    parser.add_argument("--platform", default=detect_platform())
    parser.add_argument("--format", choices=("zip", "tar.gz"), default=None)
    args = parser.parse_args()

    binary = args.exe or args.doctor
    assert binary is not None
    if not binary.is_file():
        print(f"ERROR: no existe {binary}", file=sys.stderr)
        return 1

    kind = "full" if args.exe else "doctor"
    extension = args.format or ("zip" if platform.system() == "Windows" else "tar.gz")
    args.out.mkdir(parents=True, exist_ok=True)

    archive = args.out / f"SimpsonsRecomp-{args.version}-{kind}-{args.platform}.{extension}"

    with tempfile.TemporaryDirectory(prefix="simpsons-release-") as tmp:
        staging = Path(tmp) / f"{kind}-{args.platform}"
        files = collect(staging, args.exe, args.doctor)
        if extension == "zip":
            write_zip(archive, files, staging)
        else:
            write_tar(archive, files, staging)

    print(f"OK: {archive} ({archive.stat().st_size / (1024 * 1024):.1f} MB)")
    print("Nota: el paquete no incluye datos del juego; el usuario debe instalar su copia.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
