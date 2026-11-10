---
name: sdd-validate
description: >
  Fase de validación del flujo SDD en The-Simpsons-Game-X360-Recomp. Ejecuta el build de CMake
  (cmake --build build) en la terminal de Windows, comprueba errores de compilación C++ o faltas de
  stubs, y reporta si la tarea se ha validado correctamente.
---

# SDD · Validate

## Propósito

Validar, tras implementar las tareas del backlog (`sdd-task` dentro del flujo de `sdd-implements`),
que el cambio compila correctamente en el entorno de Windows y que no deja la cadena de arranque
con **faltas de stubs** pendientes.

## Precondiciones

- Las tareas correspondientes del backlog (`sdd-task`) están implementadas.
- El directorio de build existe y está configurado (`build/`).

## Pasos

1. **Ejecutar el build en la terminal de Windows:**

   ```bash
   cmake --build build
   ```

2. **Comprobar errores de compilación C++:** revisar el output del build y buscar:
   - Errores de compilación: `error Cxxxxxxx` (MSVC), `error:` (Clang), fallos de Ninja
     (`FAILED`, `[!] Building CXX object`, etc.).
   - Warnings que, con `SIMPSONS_WARNINGS_AS_ERRORS`, romperían el build (`-Werror` / `/WX`).
   - Si hay errores: no se emite veredicto de validación; hay que corregirlos antes de continuar.

3. **Comprobar faltas de stubs:** tras un build correcto, verificar que no queden llamadas no
   resueltas relacionadas con el cambio:
   - Revisar `game/re/logs/` y el informe de hooks (`hook-report.md`) generado por el runtime.
   - Si aparecen llamadas no resueltas, usar el triaje:
     `python tools/triage.py game/re/logs/crash.log --write`.
   - Los stubs solo deben apuntar a direcciones **confirmadas** en `game/re/addresses.md`.

4. **Emitir el reporte de validación:** resumir el resultado del build, los errores encontrados
   (si los hubo) y el estado de los stubs.

## Reporte de validación

- **Build:** `cmake --build build` → OK / FALLO.
- **Errores C++:** lista de errores (o "ninguno").
- **Stubs:** llamadas no resueltas detectadas (o "ninguna").
- **Veredicto:** la tarea **sí está validada correctamente** / **no está validada**.

## Definition of Done del stage

- [ ] `cmake --build build` en Windows sin errores de compilación C++.
- [ ] No hay stubs pendientes que bloqueen el arranque del cambio.
- [ ] Se ha emitido el reporte de validación con el veredicto.
