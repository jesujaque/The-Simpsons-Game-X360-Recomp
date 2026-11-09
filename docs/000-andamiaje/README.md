# Fase 0 — Andamiaje

> **Estado:** completada.

## Alcance

Preparar el repositorio para que las fases siguientes no pierdan tiempo en
infraestructura: licencia, README, manifiesto de ReXGlue, herramientas de
extracción/verificación/triaje, CI y modelo de ramas.

## Entregables

- Repositorio con licencia y documentación base.
- `game/simpsons_manifest.toml` y configuración de ejemplo.
- Scripts de `tools/` con `--help` y `--dry-run` cuando aplica.
- CI que compila `SimpsonsDoctor` en Windows y Linux.
- Ramas `develop`/`main`, PRs y releases por tag.

## Criterio de salida

CI en verde y `SimpsonsDoctor` detecta correctamente un volcado.

## Enlaces

- [Roadmap](../ROADMAP.md)
- [Arquitectura](../ARCHITECTURE.md)
- [Compilación](../BUILDING.md)
- [Contribuir](../../CONTRIBUTING.md)
