#!/usr/bin/env python3
"""Comprueba que los enlaces relativos de la documentación existen.

Uso:
    python tools/check_docs.py
    python tools/check_docs.py --root docs --verbose
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parent.parent
LINK_PATTERN = re.compile(r"\[[^\]]*\]\(([^)]+)\)")
SKIP_DIRS = {".git", "build", "dist", "out", "venv", ".venv", "__pycache__", "node_modules"}


def iter_markdown(root: Path):
    for path in root.rglob("*.md"):
        if any(part in SKIP_DIRS for part in path.parts):
            continue
        yield path


def check_file(path: Path) -> list[str]:
    problems: list[str] = []
    text = path.read_text(encoding="utf-8", errors="replace")
    for match in LINK_PATTERN.finditer(text):
        raw = match.group(1).strip()
        if not raw or raw.startswith(("http://", "https://", "mailto:", "#")):
            continue
        link = unquote(raw.split("#", 1)[0].strip())
        if not link:
            continue
        target = (path.parent / link).resolve()
        if not target.exists():
            problems.append(f"{path}: enlace roto -> {raw}")
    return problems


def main() -> int:
    parser = argparse.ArgumentParser(description="Valida enlaces relativos en Markdown")
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--verbose", action="store_true")
    args = parser.parse_args()

    root = args.root.resolve()
    files = list(iter_markdown(root))
    problems: list[str] = []
    for path in files:
        problems.extend(check_file(path))

    if problems:
        print(f"Enlaces rotos: {len(problems)}", file=sys.stderr)
        for problem in problems:
            print(f"  {problem}", file=sys.stderr)
        return 1

    if args.verbose:
        for path in files:
            print(f"OK: {path.relative_to(root)}")
    print(f"OK: {len(files)} documentos con enlaces válidos")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
