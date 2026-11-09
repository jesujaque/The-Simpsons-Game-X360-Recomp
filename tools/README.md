# Herramientas

Scripts de apoyo al pipeline. Todos usan solo la biblioteca estándar de Python 3.10+.

| Script | Función |
| --- | --- |
| `bootstrap.py` | Comprueba entorno, SDK y datos del juego |
| `extract_iso.py` | Extrae una ISO con `extract-xiso` |
| `codegen.py` | Ejecuta el codegen de ReXGlue sobre el manifiesto |
| `triage.py` | Convierte llamadas no resueltas del log o de `hook-report.md` en entradas de `functions_patch.toml` |
| `verify_dump.py` | Verifica el volcado extraído y genera `game/re/dump-report.md` |
| `check_docs.py` | Comprueba que los enlaces relativos de la documentación existen |
| `package_release.py` | Empaqueta una release sin datos del juego |

## Flujo típico

```bash
python tools/bootstrap.py --check
python tools/extract_iso.py juego.iso --out game/extracted
python tools/verify_dump.py --root game/extracted --md game/re/dump-report.md
python tools/codegen.py --verify-only        # valida manifiesto y XEX sin SDK
python tools/codegen.py --sdk tools/rexglue-sdk
# compilar, ejecutar y, si falla:
python tools/triage.py crash.log --write
python tools/codegen.py --sdk tools/rexglue-sdk
```

## Reglas

- Los scripts nunca descargan ni distribuyen contenido del juego.
- `--dry-run` disponible en las operaciones que escriben.
- `codegen.py` valida el manifiesto y que los XEX/DLL de entrada existan antes
  de invocar al SDK. Usa `--verify-only` para comprobarlo sin SDK o
  `--skip-input-check` si el SDK resuelve las rutas de otra forma.
- Si la CLI de ReXGlue cambia, ajusta `build_command()` en `codegen.py`.
