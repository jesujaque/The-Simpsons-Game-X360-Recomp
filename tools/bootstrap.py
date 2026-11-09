#!/usr/bin/env python3
"""Comprueba el entorno de desarrollo y los datos del juego.

Uso:
    python tools/bootstrap.py --check
    python tools/bootstrap.py --check --game-root "D:/dumps/extracted"
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

OK = "[ OK ]"
WARN = "[WARN]"
FAIL = "[FAIL]"


def command_version(command: list[str]) -> str | None:
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            timeout=10,
            check=False,
        )
    except (OSError, subprocess.SubprocessError):
        return None
    text = (result.stdout or result.stderr).strip().splitlines()
    return text[0].strip() if text else None


def check_python() -> tuple[bool, str]:
    version = sys.version_info
    ok = version >= (3, 10)
    return ok, f"Python {version.major}.{version.minor}.{version.micro}"


def check_command(name: str, args: list[str]) -> tuple[bool, str]:
    path = shutil.which(name)
    if not path:
        return False, f"{name}: no encontrado en PATH"
    version = command_version([path, *args])
    return True, f"{name}: {version or path}"


def find_game_root(hint: Path | None) -> Path | None:
    candidates: list[Path] = []
    if hint:
        candidates.append(hint)
    env = os.environ.get("SIMPSONS_GAME_ROOT")
    if env:
        candidates.append(Path(env))
    candidates.extend([ROOT / "game" / "extracted", Path.cwd() / "game" / "extracted"])

    for candidate in candidates:
        if (candidate / "default.xex").is_file():
            return candidate.resolve()
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description="Diagnóstico del entorno de desarrollo")
    parser.add_argument("--check", action="store_true", help="solo comprobar, no modificar nada")
    parser.add_argument("--game-root", type=Path, help="directorio con default.xex")
    args = parser.parse_args()

    print("The Simpsons Game - Xbox 360 Recompiled")
    print("=" * 45)

    checks: list[tuple[str, bool, str]] = []
    checks.append(("Python", *check_python()))
    checks.append(("Git", *check_command("git", ["--version"])))
    checks.append(("CMake", *check_command("cmake", ["--version"])))
    checks.append(("Ninja", *check_command("ninja", ["--version"])))
    checks.append(("Compilador C++", *check_command("clang++", ["--version"])))

    sdk = ROOT / "tools" / "rexglue-sdk"
    rexglue = sdk / ("rexglue.exe" if os.name == "nt" else "rexglue")
    checks.append(("SDK ReXGlue", rexglue.is_file(), str(rexglue if rexglue.is_file() else sdk / "<falta>")))

    manifest = ROOT / "game" / "simpsons_manifest.toml"
    checks.append(("Manifiesto", manifest.is_file(), str(manifest)))

    game_root = find_game_root(args.game_root)
    checks.append(
        (
            "Datos del juego",
            game_root is not None,
            str(game_root) if game_root else "default.xex no encontrado",
        )
    )

    for label, ok, detail in checks:
        print(f"{OK if ok else WARN} {label}: {detail}")

    print()
    if game_root is None:
        print("Siguiente paso: extrae tu ISO con")
        print('  python tools/extract_iso.py "<ruta>.iso" --out game/extracted')
        return 0

    if not rexglue.is_file():
        print("Siguiente paso: descarga el SDK de ReXGlue en tools/rexglue-sdk/")
        print("  https://github.com/rexglue/rexglue-sdk/releases")
        return 0

    print("Entorno listo. Genera el código con:")
    print("  python tools/codegen.py --sdk tools/rexglue-sdk")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
