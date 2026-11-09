#!/usr/bin/env python3
"""Ejecuta el codegen de ReXGlue sobre el manifiesto del proyecto.

La CLI del SDK puede cambiar entre versiones. Este script construye el comando,
lo muestra y permite añadir argumentos extra. Si tu versión usa otras flags,
ajusta `build_command()` o pasa `--extra-arg`.

Antes de invocar al SDK valida el manifiesto y comprueba que los XEX/DLL de
entrada existen. Esa comprobación se puede omitir con `--skip-input-check` y se
puede ejecutar sin SDK con `--verify-only`.

Uso:
    python tools/codegen.py --sdk tools/rexglue-sdk
    python tools/codegen.py --sdk tools/rexglue-sdk --dry-run
    python tools/codegen.py --verify-only
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


def _strip_comment(line: str) -> str:
    """Quita un comentario `#` sin tocar el contenido de una cadena."""
    quote: str | None = None
    escaped = False
    for index, char in enumerate(line):
        if escaped:
            escaped = False
            continue
        if char == "\\" and quote:
            escaped = True
            continue
        if char in ('"', "'"):
            if quote == char:
                quote = None
            elif quote is None:
                quote = char
            continue
        if char == "#" and quote is None:
            return line[:index]
    return line


def _string_value(raw: str) -> str | None:
    raw = raw.strip()
    if len(raw) >= 2 and raw[0] == raw[-1] and raw[0] in ('"', "'"):
        return raw[1:-1]
    return None


def read_manifest_inputs(manifest: Path) -> tuple[list[Path], list[str]]:
    """Devuelve los ficheros de entrada y los errores de estructura del manifiesto.

    Solo interpreta el subconjunto que necesita el codegen: secciones, claves
    `file_path` y `game_root`. El SDK sigue siendo quien valida el resto.
    """
    errors: list[str] = []
    inputs: list[Path] = []
    section = ""
    seen: set[str] = set()

    try:
        text = manifest.read_text(encoding="utf-8")
    except OSError as exc:
        return [], [f"no se puede leer {manifest}: {exc}"]

    for number, original in enumerate(text.splitlines(), start=1):
        line = _strip_comment(original).strip()
        if not line:
            continue
        if line.startswith("[") and line.endswith("]"):
            section = line.strip("[]").strip()
            continue
        if "=" not in line:
            errors.append(f"línea {number}: se esperaba clave = valor")
            continue

        key, _, value = line.partition("=")
        key = key.strip()
        if key != "file_path":
            continue

        path = _string_value(value)
        if path is None:
            errors.append(f"línea {number}: file_path debe ser una cadena entre comillas")
            continue
        if section not in ("entrypoint", "modules"):
            errors.append(f"línea {number}: file_path fuera de [entrypoint]/[[modules]]")
            continue

        seen.add(section)
        inputs.append((manifest.parent / path).resolve())

    if "entrypoint" not in seen:
        errors.append("falta [entrypoint] con file_path")

    return inputs, errors


def validate_inputs(inputs: list[Path]) -> list[Path]:
    missing = [path for path in inputs if not path.is_file()]
    if missing:
        for path in missing:
            print(f"ERROR: no existe el fichero de entrada {path}", file=sys.stderr)
        print(
            "Extrae tu ISO en game/extracted/ o usa --skip-input-check si el SDK "
            "resuelve las rutas de otra forma.",
            file=sys.stderr,
        )
    return missing


def main() -> int:
    parser = argparse.ArgumentParser(description="Genera el C++ recompilado con ReXGlue")
    parser.add_argument("--sdk", type=Path, default=ROOT / "tools" / "rexglue-sdk")
    parser.add_argument("--manifest", type=Path, default=ROOT / "game" / "simpsons_manifest.toml")
    parser.add_argument(
        "--verify-only",
        action="store_true",
        help="valida manifiesto y entradas y termina sin buscar ni ejecutar el SDK",
    )
    parser.add_argument(
        "--skip-input-check",
        action="store_true",
        help="no comprueba que los XEX/DLL indicados en el manifiesto existan",
    )
    parser.add_argument("--dry-run", action="store_true", help="muestra el comando sin ejecutarlo")
    parser.add_argument("extra", nargs=argparse.REMAINDER, help="argumentos extra para rexglue")
    args = parser.parse_args()

    manifest = args.manifest.resolve()
    if not manifest.is_file():
        print(f"ERROR: no existe el manifiesto {manifest}", file=sys.stderr)
        return 1

    inputs, manifest_errors = read_manifest_inputs(manifest)
    if manifest_errors:
        for error in manifest_errors:
            print(f"ERROR: manifiesto: {error}", file=sys.stderr)
        return 1

    if not args.skip_input_check:
        if validate_inputs(inputs):
            return 1
        print(f"OK: {len(inputs)} entrada(s) del manifiesto encontradas")

    if args.verify_only:
        print(f"OK: manifiesto válido {manifest}")
        return 0

    tool = find_rexglue(args.sdk)
    if not tool and not args.dry_run:
        tool = shutil.which("rexglue")
        tool = Path(tool) if tool else None

    if not tool:
        if not args.dry_run:
            print(f"ERROR: rexglue no encontrado en {args.sdk}", file=sys.stderr)
            print("Descarga el SDK: https://github.com/rexglue/rexglue-sdk/releases", file=sys.stderr)
            return 1
        tool = Path("rexglue")

    extra = [arg for arg in args.extra if arg != "--"]
    command = build_command(Path(tool), manifest, extra)
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
    if not sources:
        print(
            f"ERROR: el codegen terminó bien pero no generó ningún .cpp en {generated}",
            file=sys.stderr,
        )
        return 1

    print(f"OK: {len(sources)} archivos .cpp generados en {generated}")
    print("Siguiente paso: cmake --preset windows-full && cmake --build --preset windows-full")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
