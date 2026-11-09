#!/usr/bin/env python3
"""Extrae una ISO de Xbox 360 con extract-xiso.

Requiere `extract-xiso` en el PATH (o pasa --extract-xiso).

Uso:
    python tools/extract_iso.py "The Simpsons Game.iso" --out game/extracted
    python tools/extract_iso.py juego.iso --out game/extracted --dry-run
"""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def find_tool(explicit: str | None) -> str | None:
    if explicit:
        return explicit if Path(explicit).is_file() else None
    return shutil.which("extract-xiso")


def main() -> int:
    parser = argparse.ArgumentParser(description="Extrae una ISO de Xbox 360")
    parser.add_argument("iso", type=Path, help="ruta a la ISO")
    parser.add_argument("--out", type=Path, default=ROOT / "game" / "extracted")
    parser.add_argument("--extract-xiso", dest="tool", help="ruta al ejecutable extract-xiso")
    parser.add_argument("--dry-run", action="store_true", help="solo mostrar el comando")
    args = parser.parse_args()

    if not args.iso.is_file():
        print(f"ERROR: no existe la ISO: {args.iso}", file=sys.stderr)
        return 1

    tool = find_tool(args.tool)
    if not tool:
        print("ERROR: extract-xiso no encontrado.", file=sys.stderr)
        print("Descárgalo de https://github.com/XboxDev/extract-xiso y añádelo al PATH.", file=sys.stderr)
        return 1

    args.out.mkdir(parents=True, exist_ok=True)
    command = [tool, "-x", "-d", str(args.out), str(args.iso)]

    print("Comando:", " ".join(command))
    if args.dry_run:
        return 0

    result = subprocess.run(command, check=False)
    if result.returncode != 0:
        print(f"ERROR: extract-xiso terminó con código {result.returncode}", file=sys.stderr)
        return result.returncode

    xex = args.out / "default.xex"
    if not xex.is_file():
        print("AVISO: no se encontró default.xex. Revisa la estructura extraída.", file=sys.stderr)
        for entry in sorted(args.out.iterdir())[:20]:
            print("  ", entry.name)
        return 1

    print(f"OK: datos extraídos en {args.out}")
    print("Siguiente paso: python tools/codegen.py --sdk tools/rexglue-sdk")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
