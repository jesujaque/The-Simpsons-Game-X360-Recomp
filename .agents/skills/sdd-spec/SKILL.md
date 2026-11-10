---
name: sdd-spec
description: >
  Fase de especificación del flujo SDD en The-Simpsons-Game-X360-Recomp. Guía la redacción de la
  spec (objetivo, alcance, requisitos, criterios de aceptación) antes de planificar o implementar.
---

# SDD · Spec

## Propósito

Redactar la **especificación** de un cambio antes de planificar o escribir código. La spec convierte
la necesidad en requisitos claros, con criterios de aceptación medibles, para que el plan y las tareas
sean derivables sin ambigüedades.

## Precondiciones

- La **constitución** (`sdd-constitution`) ha sido aplicada.
- Se conoce la fase del roadmap afectada (`docs/ROADMAP.md`).

## Pasos

1. **Contexto**: leer la fase y los runbooks implicados
   (`docs/001-…/PHASE-1-ANALYSIS.md`, `docs/002-…/PHASE-2-IMPLEMENTATION.md`,
   `docs/003-…/PHASE-3-IMPLEMENTATION.md`).
2. **Problema y objetivo**: qué se quiere lograr y por qué; en términos de comportamiento del port.
3. **Alcance**: qué incluye y qué queda fuera (out of scope).
4. **Requisitos**: funcionales (RF) y no funcionales (RNF).
5. **Criterios de aceptación**: verificables con comandos, trazas o builds del repositorio.
6. **Restricciones**: aplicar la constitución (C++20, CMake+Ninja, tipos fijos, stubs).
7. **Registro**: documentar la spec en la carpeta de fase correspondiente (`docs/NNN-titulo/`) o en
   la raíz de `docs/` si es transversal.

## Reglas duras

- No se escribe código hasta que la spec exista y esté aprobada.
- Los criterios de aceptación deben ser **medibles**: builds doctor, `ctest`, trazas `--trace-out`,
  `tools/triage.py`, `game/re/addresses.md`.
- Los stubs solo pueden apuntar a direcciones **confirmadas** con evidencia.
- Los parches de gameplay deben poder desactivarse por configuración (`PatchSettings` / `PatchConfig`).

## Definition of Done del stage

- [ ] Spec redactada con objetivo, alcance, requisitos y criterios de aceptación.
- [ ] Criterios de aceptación verificables sin ambigüedad.
- [ ] Restricciones de la constitución identificadas.
