#!/usr/bin/env python3
"""Ejecuta el codegen de ReXGlue sobre el manifiesto del proyecto.

La CLI del SDK puede cambiar entre versiones. Este script construye el comando,
lo muestra y permite añadir argumentos extra. Si tu versión usa otras flags,
ajusta `build_command()` o pasa `--extra-arg`.

Uso:
    python tools/codegen.py --sdk tools/rexglue-sdk
    python tools/codegen.py --sdk tools/rexglue-sdk --dry-run
    python tools/codegen.py --sdk tools/rexglue-sdk -- --jobs 8
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def find_rexglue(sdk: Path) -> Path | None:
    for name in ("rexglue.exe", "rexglue"):
        candidate = sdk / name
        if candidate.is_file():
            return candidate
    for name in ("rexglue.exe", "rexglue"):
        candidate = sdk / "bin" / name
        if candidate.is_file():
            return candidate
    return None


def build_command(tool: Path, manifest: Path, extra: list[str]) -> list[str]:
    # v0.8.x: `rexglue codegen <manifest>`.
    return [str(tool), "codegen", str(manifest), *extra]


def main() -> int:
    parser = argparse.ArgumentParser(description="Genera el C++ recompilado con ReXGlue")
    parser.add_argument("--sdk", type=Path, default=ROOT / "tools" / "rexglue-sdk")
    parser.add_argument("--manifest", type=Path, default=ROOT / "game" / "simpsons_manifest.toml")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("extra", nargs=argparse.REMAINDER, help="argumentos extra para rexglue")
    args = parser.parse_args()

    if not args.manifest.is_file():
        print(f"ERROR: no existe el manifiesto {args.manifest}", file=sys.stderr)
        return 1

    tool = find_rexglue(args.sdk)
    if not tool and not args.dry_run:
        tool = shutil.which("rexglue")
        tool = Path(tool) if tool else None

    if not tool:
        print(f"ERROR: rexglue no encontrado en {args.sdk}", file=sys.stderr)
        print("Descarga el SDK: https://github.com/rexglue/rexglue-sdk/releases", file=sys.stderr)
        return 1

    extra = [arg for arg in args.extra if arg != "--"]
    command = build_command(Path(tool), args.manifest.resolve(), extra)
    print("Comando:", " ".join(command))

    if args.dry_run:
        return 0

    env = dict(os.environ)
    env.setdefault("REXGLUE_SDK_DIR", str(args.sdk.resolve()))
    result = subprocess.run(command, check=False, env=env)
    if result.returncode != 0:
        print(f"ERROR: codegen terminó con código {result.returncode}", file=sys.stderr)
        return result.returncode

    generated = ROOT / "game" / "generated"
    sources = list(generated.rglob("*.cpp"))
    print(f"OK: {len(sources)} archivos .cpp generados en {generated}")
    print("Siguiente paso: cmake --preset windows-full && cmake --build --preset windows-full")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
