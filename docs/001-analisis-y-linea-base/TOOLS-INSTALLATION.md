# Fase 1 — Herramientas e entorno

> **Estado:** en progreso · herramientas de fase 1 instaladas, ISO y baseline pendientes.

## Objetivo

Dejar constancia del estado del entorno de herramientas externas y de las
verificaciones preliminares ejecutadas antes de trabajar con el volcado del
juego.

## Herramientas instaladas

| Herramienta | Versión | Origen | Ubicación / comando | Estado |
| --- | --- | --- | --- | --- |
| Python | 3.12.10 | `winget install --id Python.Python.3.12` | `C:\Users\Yesu\AppData\Local\Programs\Python\Python312` | ✅ |
| 7-Zip | 26.06 | `winget install --id 7zip.7zip` | `C:\Program Files\7-Zip` | ✅ |
| CMake | 4.4.4 | `winget install --id Kitware.CMake` | PATH | ✅ |
| Ninja | 1.13.2 | `winget install --id Ninja-build.Ninja` | PATH | ✅ |
| JDK | 21.0.13 (Temurin) | ya presente | `C:\Program Files\Eclipse Adoptium\jdk-21.0.13.9-hotspot\bin` | ✅ |
| Git / gh / Node / npm / dotnet | — | ya presentes | PATH | ✅ |
| `extract-xiso` | build `2026-09-11` | release de [XboxDev/extract-xiso](https://github.com/XboxDev/extract-xiso/releases) | `tools/extract-xiso/extract-xiso.exe` | ✅ |
| Ghidra | 12.2 (`Ghidra_12.2_build`) | release de [NSA/Ghidra](https://github.com/NationalSecurityAgency/ghidra/releases) | `C:\Users\Yesu\tools\ghidra\ghidra_12.2_PUBLIC` | ✅ |
| XEXLoaderWV | 13.0.0 | release de [SaveEditors/XEXLoaderWV](https://github.com/SaveEditors/XEXLoaderWV/releases) | `...\extensions\Ghidra\Extensions\XEXLoaderWV` | ✅ |
| Xenia Canary | — | pendiente | — | ⬜ |
| XenonAnalyse | — | pendiente | `tools/XenonRecomp/` | ⬜ |
| SDK ReXGlue | 0.8.x | pendiente | `tools/rexglue-sdk/` | ⬜ |

## Verificaciones ejecutadas

- `python -m compileall -q tools` → ✅ sin errores.
- `python tools/check_docs.py` → ✅ OK: 30 documentos con enlaces válidos.
- `python tools/bootstrap.py --check` → ⚠️ Python, Git y manifiesto OK; CMake/Ninja
  instalados (el WARN era por el PATH de la sesión); `clang++` FALTA; SDK ReXGlue FALTA;
  `default.xex` FALTA (pendiente de la ISO).
- `python tools/extract_iso.py --help` → ✅ OK (con `extract-xiso` en PATH).
- `python tools/verify_dump.py --help` → ✅ OK.
- `python tools/codegen.py --help` → ✅ OK.
- `python tools/triage.py --help` → ✅ OK.
- `python tools/package_release.py --help` → ✅ OK.

## Pendiente para la fase 1

- [ ] ISO legal de The Simpsons Game (Xbox 360) → `game/extracted/`.
- [ ] Xenia Canary → línea base de arranque.
- [ ] XenonAnalyse → fronteras de funciones (`game/re/analysis.toml`).
- [ ] SDK ReXGlue → fase 2.

## Próximos pasos

1. Colocar la ISO en una ruta tipo `D:/dumps/The Simpsons Game.iso`.
2. `python tools/extract_iso.py "<iso>" --out game/extracted`.
3. `python tools/verify_dump.py --root game/extracted --md game/re/dump-report.md`.

## Enlaces

- [PHASE-1-ANALYSIS.md](PHASE-1-ANALYSIS.md)
- [BUILDING.md](../BUILDING.md)
- [ROADMAP](../ROADMAP.md)
