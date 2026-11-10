# Fase 1 — Análisis y línea base

> **Estado:** pendiente del volcado del usuario.

## Alcance

Instalar el entorno de análisis, verificar la ISO, observar el arranque en
Xenia, importar el XEX en Ghidra y documentar el camino crítico hasta el menú
principal.

## Documentos

| Documento | Contenido |
| --- | --- |
| [PHASE-1-ANALYSIS.md](PHASE-1-ANALYSIS.md) | Runbook operativo completo de la fase 1 |
| [TOOLS-INSTALLATION.md](TOOLS-INSTALLATION.md) | Estado de herramientas e entorno instalados |

## Entregables

| Artefacto | Ubicación |
| --- | --- |
| Informe del volcado | `game/re/dump-report.md` |
| Secuencia de arranque | `game/re/boot-sequence.md` |
| Direcciones y firmas | `game/re/addresses.md` |
| Formatos | `game/re/formats.md` |
| Candidatos de funciones | `game/functions_patch.toml` |

## Criterio de salida

El camino de arranque está explicado de principio a fin, los imports están
documentados y el codegen de la fase 2 puede ejecutarse sin incógnitas
estructurales.

## Enlaces

- [Roadmap](../ROADMAP.md)
- [Investigación](../RESEARCH.md)
- [Compilación](../BUILDING.md)
