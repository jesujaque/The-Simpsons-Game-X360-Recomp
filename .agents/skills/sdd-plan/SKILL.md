---
name: sdd-plan
description: >
  Fase de planificación del flujo SDD en The-Simpsons-Game-X360-Recomp. Convierte la spec aprobada
  en un plan de implementación con milestones, decisiones técnicas, riesgos y criterios de salida.
---

# SDD · Plan

## Propósito

Convertir la **spec** aprobada en un **plan de implementación** ejecutable: qué se hará, en qué orden,
con qué decisiones técnicas y con qué criterios de salida.

## Precondiciones

- La **spec** (`sdd-spec`) está redactada y aprobada.
- La **constitución** (`sdd-constitution`) ha sido aplicada.

## Pasos

1. **Descomposición**: partir el objetivo de la spec en milestones o bloques lógicos.
2. **Mapeo de capas** según `docs/ARCHITECTURE.md`:
   plataforma (`game/src/platform`), runtime (`game/src/app/runtime.*`),
   arranque/traza (`game/src/app/boot_trace.*`), assets (`game/src/assets`),
   hooks/parches (`game/src/hooks`, `game/src/patches`), render/audio/input/UI
   (`game/src/renderer`, `game/src/audio`, `game/src/input`, `game/src/ui`).
3. **Contratos a tocar**: `RuntimeBackend`, `VirtualFileSystem`, `BootTrace`, `HookRegistry`,
   `GameConfig` / `PatchSettings` / `PatchConfig`, `config.toml`.
4. **Diseño**: para cada bloque, archivos, firmas, comandos de verificación y criterios de salida.
5. **Decisiones técnicas**: listar alternativa → elección → motivo.
6. **Riesgos y mitigaciones**, con dependencias entre bloques y con otras fases/issues.

## Reglas

- El plan se basa en la spec; no redefine el alcance sin justificación.
- Cada paso debe dejar un punto de verificación compilable (build doctor, `ctest`, trazas).
- Los stubs deben resolverse con el patrón de la constitución (R4).

## Definition of Done del stage

- [ ] Plan redacto con milestones, decisiones y criterios de salida.
- [ ] Contratos identificados.
- [ ] Sin implementación iniciada sin plan aprobado.
