---
name: sdd-implements
description: >
  Fase de implementación del flujo SDD en The-Simpsons-Game-X360-Recomp. Ejecuta las tareas del
  backlog (sdd-task) respetando la constitución, con verificación en cada paso y Definition of Done
  al finalizar.
---

# SDD · Implementa

## Propósito

Ejecutar el **backlog de tareas** (`sdd-task`) derivado del plan, respetando la constitución
(`sdd-constitution`) y verificando cada cambio antes de avanzar.

## Precondiciones

- Constitución, spec, plan y tareas completados (`sdd-constitution`, `sdd-spec`, `sdd-plan`, `sdd-task`).
- Tarea actual lista con criterios de aceptación y comandos de verificación.

## Flujo por tarea

1. Leer la tarea (`T-XX`) con sus criterios de aceptación.
2. Implementar solo el alcance definido; no ampliar sin plan.
3. Verificar con los comandos de la constitución:
   - `python -m compileall tools`
   - `python tools/check_docs.py`
   - `python tools/codegen.py --verify-only`
   - `cmake --preset linux-dev && cmake --build --preset linux-dev`
   - `ctest --test-dir build/linux-dev --output-on-failure`
   - `./build/linux-dev/bin/SimpsonsDoctor --help`
   - Full: `cmake --preset windows-full && cmake --build --preset windows-full`
   - `python tools/triage.py game/re/logs/crash.log --write`
4. Actualizar documentación de fase / `game/re/` si el cambio lo requiere.
5. Commit con Conventional Commits (`feat:`, `fix:`, ...) en rama `feat/`, `fix/`, etc. desde `develop`.
6. Subir y abrir PR contra `develop` con CI en verde.

## Reglas

- No saltarse etapas del flujo SDD (constitución → spec → plan → task → implementa).
- Un commit = un cambio lógico; un PR = un cambio lógico.
- Si una tarea falla sus criterios, no avanzar: corregir o revertir, no ampliar alcance.
- Los stubs: solo direcciones confirmadas; llamadas no resueltas → triage → `functions_patch.toml`.

## Definition of Done del stage

- [ ] Todas las tareas del plan ejecutadas.
- [ ] Build doctor pasa (Linux y Windows), `ctest` en verde.
- [ ] Docs y `game/re/` actualizados.
- [ ] PR a `develop` con CI verde, sin assets/binarios del juego y doctor funcional.
