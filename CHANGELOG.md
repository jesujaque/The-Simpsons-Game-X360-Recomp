# Changelog

Todas las versiones notables del proyecto se documentan aquí.
El formato sigue [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/)
y el versionado [SemVer](https://semver.org/lang/es/).

## [No publicado]

### Añadido

- Runbook completo de la fase 1 (`docs/PHASE-1-ANALYSIS.md`) y plantillas de investigación.
- `tools/verify_dump.py` para validar el volcado y generar `game/re/dump-report.md`.
- Modelo de ramas `develop`/`main` con PRs obligatorios y CI en verde.
- Workflow de release por tag `v*` con validación de versión y rama.
- Herramienta `package_release.py` y guía [docs/RELEASING.md](docs/RELEASING.md).
- Estructura inicial del proyecto: CMake, presets, CI y documentación.
- Manifiesto de ReXGlue y tabla de parches de funciones.
- Herramientas `bootstrap`, `extract_iso`, `codegen` y `triage`.
- Utilidad `SimpsonsDoctor` para validar entorno y datos del juego.
- Capas base de aplicación: hooks, parches, render, audio, input y UI.
- Documentación de arquitectura, compilación, investigación, roadmap y legal.

## [0.1.0] - 2026-11-09

- Primer commit del repositorio.
