# Changelog

Todas las versiones notables del proyecto se documentan aquí.
El formato sigue [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/)
y el versionado [SemVer](https://semver.org/lang/es/).

## [No publicado]

### Añadido

- Infraestructura de fases 2–3 sin SDK: runtime nulo, VFS, hooks/stubs, traza de arranque,
  catálogo de assets, mapa de entrada y audio configurable.
- `docs/002-primer-enlace/PHASE-2-IMPLEMENTATION.md` y
  `docs/003-menu-principal/PHASE-3-IMPLEMENTATION.md` con el contrato del runtime y los pasos de enlace.
- Carga de `config.toml` con validación de rangos (sin dependencias externas).
- Pruebas de núcleo (`SimpsonsCoreTests`) para config, VFS, traza, hooks, assets y entrada.
- `tools/triage.py` reconoce también las llamadas no resueltas del `HookRegistry`.
- `tools/codegen.py` valida manifiesto y entradas, admite `--verify-only` y falla si el codegen no genera `.cpp`.
- Documentación por fases en `docs/NNN-titulo-fase/`, con índice y convención en `docs/README.md`.
- Runbook completo de la fase 1 (`docs/001-analisis-y-linea-base/PHASE-1-ANALYSIS.md`) y plantillas de investigación.
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
