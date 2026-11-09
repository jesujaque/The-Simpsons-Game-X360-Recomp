#!/usr/bin/env python3
"""Extrae direcciones no resueltas de un log y las añade a functions_patch.toml.

Busca patrones habituales del runtime:
    Unresolved call 0x82A71810
    unresolved indirect call at 0x82A71810
    [error] No function registered for 0x82A71810

Uso:
    python tools/triage.py crash.log
    python tools/triage.py crash.log --write
    python tools/triage.py crash.log --write --dry-run
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PATCH_FILE = ROOT / "game" / "functions_patch.toml"

ADDRESS_PATTERNS = [
    re.compile(r"unresolved\s+(?:indirect\s+)?call(?:\s+at)?\s*[:\s]\s*(0x[0-9A-Fa-f]{8})", re.I),
    re.compile(r"no function registered for\s*(0x[0-9A-Fa-f]{8})", re.I),
    re.compile(r"unknown function\s*(0x[0-9A-Fa-f]{8})", re.I),
    re.compile(r"llamada no resuelta\s*(0x[0-9A-Fa-f]{8})", re.I),
    re.compile(r"^\|\s*`?(0x[0-9A-Fa-f]{8})`?\s*\|", re.M),
]


def collect_addresses(text: str) -> list[str]:
    found: set[str] = set()
    for pattern in ADDRESS_PATTERNS:
        for match in pattern.finditer(text):
            found.add(match.group(1).upper().replace("0X", "0x"))
    return sorted(found)


def existing_addresses(text: str) -> set[str]:
    return {m.group(1).upper().replace("0X", "0x") for m in re.finditer(r'"(0x[0-9A-Fa-f]{8})"', text)}


def append_entries(path: Path, addresses: list[str]) -> None:
    text = path.read_text(encoding="utf-8")
    known = existing_addresses(text)
    new = [address for address in addresses if address.upper() not in known]
    if not new:
        print("No hay direcciones nuevas.")
        return

    if "[functions]" not in text:
        text += "\n[functions]\n"

    lines = [f'"{address}" = {{}}  # triage {Path(sys.argv[1]).name}' for address in new]
    text = text.rstrip() + "\n" + "\n".join(lines) + "\n"
    path.write_text(text, encoding="utf-8")
    print(f"Añadidas {len(new)} direcciones a {path}")
    for line in lines:
        print("  ", line)


def main() -> int:
    parser = argparse.ArgumentParser(description="Triaje de llamadas no resueltas")
    parser.add_argument("log", type=Path)
    parser.add_argument("--write", action="store_true", help="modificar functions_patch.toml")
    parser.add_argument("--dry-run", action="store_true", help="no escribir nada")
    parser.add_argument("--patch-file", type=Path, default=PATCH_FILE)
    args = parser.parse_args()

    if not args.log.is_file():
        print(f"ERROR: no existe {args.log}", file=sys.stderr)
        return 1

    text = args.log.read_text(encoding="utf-8", errors="replace")
    addresses = collect_addresses(text)
    if not addresses:
        print("No se encontraron direcciones no resueltas.")
        return 0

    print(f"Encontradas {len(addresses)} direcciones:")
    for address in addresses:
        print("  ", address)

    if args.write:
        if args.dry_run:
            print("[dry-run] no se modifica el archivo.")
        else:
            append_entries(args.patch_file, addresses)
    else:
        print("\nUsa --write para añadirlas a game/functions_patch.toml")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
