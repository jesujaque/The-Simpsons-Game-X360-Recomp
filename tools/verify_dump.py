#!/usr/bin/env python3
"""Inspecciona el volcado extraído y genera un informe reproducible.

Uso:
    python tools/verify_dump.py --root game/extracted
    python tools/verify_dump.py --root game/extracted --md game/re/dump-report.md
    python tools/verify_dump.py --root game/extracted --json build/dump.json
"""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_ROOT = ROOT / "game" / "extracted"

KEY_FILES = [
    "default.xex",
    "Frontend.str",
    "simpsons_gameflow.lua",
    "simpsons_gameflow_helpers.lua",
    "simpsons_scores.lua",
    "cmd.txt",
]

SUSPICIOUS_SUFFIXES = {".iso", ".xiso", ".xex", ".bin", ".7z", ".zip", ".rar"}


def human_size(size: int) -> str:
    value = float(size)
    for unit in ("B", "KiB", "MiB", "GiB", "TiB"):
        if value < 1024 or unit == "TiB":
            return f"{value:.2f} {unit}" if unit != "B" else f"{int(value)} B"
        value /= 1024
    return f"{size} B"


def hash_file(path: Path, algorithm: str, chunk_size: int = 1024 * 1024) -> str:
    digest = hashlib.new(algorithm)
    with path.open("rb") as handle:
        while chunk := handle.read(chunk_size):
            digest.update(chunk)
    return digest.hexdigest()


def inventory(root: Path) -> dict:
    files: list[Path] = []
    total = 0
    for path in root.rglob("*"):
        if path.is_file():
            files.append(path)
            try:
                total += path.stat().st_size
            except OSError:
                pass

    by_extension: Counter[str] = Counter()
    for path in files:
        suffix = path.suffix.lower() or "(sin extensión)"
        by_extension[suffix] += 1

    largest = sorted(files, key=lambda p: p.stat().st_size if p.exists() else 0, reverse=True)[:15]

    return {
        "file_count": len(files),
        "total_size": total,
        "by_extension": by_extension,
        "largest": largest,
        "files": files,
    }


def build_report(root: Path) -> dict:
    info = inventory(root)
    files = info["files"]
    by_name = {path.name.lower(): path for path in files}

    xex = by_name.get("default.xex")
    xex_info = None
    if xex:
        xex_info = {
            "size": xex.stat().st_size,
            "md5": hash_file(xex, "md5"),
            "sha256": hash_file(xex, "sha256"),
        }

    key_files = []
    for name in KEY_FILES:
        found = by_name.get(name.lower())
        key_files.append({"name": name, "present": found is not None, "path": str(found.relative_to(root)) if found else ""})

    lua_count = sum(1 for path in files if path.suffix.lower() == ".lua")
    str_count = sum(1 for path in files if path.suffix.lower() == ".str")
    pkg_count = sum(1 for path in files if path.suffix.lower() == ".pkg")

    suspicious = [
        str(path.relative_to(root))
        for path in files
        if path.suffix.lower() in SUSPICIOUS_SUFFIXES and path.name.lower() != "default.xex"
    ]

    top_by_extension = []
    for suffix, count in info["by_extension"].most_common(15):
        size = sum(path.stat().st_size for path in files if (path.suffix.lower() or "(sin extensión)") == suffix)
        top_by_extension.append({"extension": suffix, "count": count, "size": size})

    return {
        "generated_at": datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC"),
        "root_name": root.name,
        "xex": xex_info,
        "file_count": info["file_count"],
        "total_size": info["total_size"],
        "by_extension": top_by_extension,
        "key_files": key_files,
        "counts": {"lua": lua_count, "str": str_count, "pkg": pkg_count},
        "largest": [
            {"path": str(path.relative_to(root)), "size": path.stat().st_size} for path in info["largest"]
        ],
        "suspicious": suspicious,
        "files": [str(path.relative_to(root)) for path in files],
    }


def render_markdown(report: dict) -> str:
    lines: list[str] = []
    lines.append("# Informe del volcado")
    lines.append("")
    lines.append(f"- **Generado:** {report['generated_at']}")
    lines.append(f"- **Raíz analizada:** `{report['root_name']}/`")
    lines.append(f"- **Archivos:** {report['file_count']}")
    lines.append(f"- **Tamaño total:** {human_size(report['total_size'])}")
    lines.append("")

    lines.append("## default.xex")
    lines.append("")
    xex = report["xex"]
    if xex:
        lines.append(f"- **Tamaño:** {human_size(xex['size'])}")
        lines.append(f"- **MD5:** `{xex['md5']}`")
        lines.append(f"- **SHA-256:** `{xex['sha256']}`")
    else:
        lines.append("**NO ENCONTRADO.** Revisa la extracción antes de continuar.")
    lines.append("")

    lines.append("## Archivos clave")
    lines.append("")
    lines.append("| Archivo | Presente | Ruta |")
    lines.append("| --- | --- | --- |")
    for entry in report["key_files"]:
        mark = "Sí" if entry["present"] else "No"
        lines.append(f"| `{entry['name']}` | {mark} | `{entry['path'] or '—'}` |")
    lines.append("")
    lines.append(
        f"Conteo: **{report['counts']['str']}** `.str`, "
        f"**{report['counts']['pkg']}** `.pkg`, **{report['counts']['lua']}** `.lua`."
    )
    lines.append("")

    lines.append("## Inventario por extensión")
    lines.append("")
    lines.append("| Extensión | Archivos | Tamaño |")
    lines.append("| --- | ---: | ---: |")
    for entry in report["by_extension"]:
        lines.append(f"| `{entry['extension']}` | {entry['count']} | {human_size(entry['size'])} |")
    lines.append("")

    lines.append("## Archivos más grandes")
    lines.append("")
    lines.append("| Archivo | Tamaño |")
    lines.append("| --- | ---: |")
    for entry in report["largest"]:
        lines.append(f"| `{entry['path']}` | {human_size(entry['size'])} |")
    lines.append("")

    lines.append("## Revisión")
    lines.append("")
    if report["suspicious"]:
        lines.append("Archivos que conviene revisar (pueden ser restos de empaquetado):")
        lines.append("")
        for path in report["suspicious"]:
            lines.append(f"- `{path}`")
    else:
        lines.append("No se detectaron ISOs ni contenedores comprimidos dentro de la extracción.")
    lines.append("")
    lines.append("> Este informe no contiene datos del juego. No adjuntes el XEX ni los assets a un issue.")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description="Verifica y documenta el volcado extraído")
    parser.add_argument("--root", type=Path, default=DEFAULT_ROOT)
    parser.add_argument("--md", type=Path, help="escribir informe Markdown")
    parser.add_argument("--json", type=Path, help="escribir informe JSON")
    parser.add_argument("--strict", action="store_true", help="fallar si falta default.xex")
    args = parser.parse_args()

    root = args.root.resolve()
    if not root.is_dir():
        print(f"ERROR: no existe el directorio {root}", file=sys.stderr)
        return 1

    report = build_report(root)

    print(f"Raíz: {root}")
    print(f"Archivos: {report['file_count']} ({human_size(report['total_size'])})")
    if report["xex"]:
        print(f"default.xex: {human_size(report['xex']['size'])}")
        print(f"  MD5:    {report['xex']['md5']}")
        print(f"  SHA256: {report['xex']['sha256']}")
    else:
        print("AVISO: no se encontró default.xex", file=sys.stderr)

    missing = [entry["name"] for entry in report["key_files"] if not entry["present"]]
    if missing:
        print(f"AVISO: faltan archivos clave: {', '.join(missing)}")

    if report["suspicious"]:
        print(f"AVISO: {len(report['suspicious'])} archivos sospechosos en la extracción")

    if args.md:
        args.md.parent.mkdir(parents=True, exist_ok=True)
        args.md.write_text(render_markdown(report), encoding="utf-8")
        print(f"Informe Markdown: {args.md}")

    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"Informe JSON: {args.json}")

    if args.strict and report["xex"] is None:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
