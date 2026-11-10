---
name: sdd-task
description: >
  Fase de creación de tareas del flujo SDD en The-Simpsons-Game-X360-Recomp. Convierte el plan
  aprobado en un backlog de tareas atómicas con criterios de aceptación verificables.
---

# SDD · Task

## Propósito

Convertir el **plan** aprobado en un backlog de **tareas atómicas** con criterios de aceptación,
para que la fase de implementación (`sdd-implements`) pueda avanzar paso a paso y verificar cada
cambio.

## Precondiciones

- El **plan** (`sdd-plan`) está aprobado.
- La **constitución** (`sdd-constitution`) está aplicada.

## Formato de una tarea

```markdown
### T-XX — <título corto>

- **Origen:** spec / plan / constitución.
- **Alcance:** qué cambia y qué no.
- **Archivos:** rutas previstas.
- **Criterios de aceptación:**
  - [ ] Build doctor pasa (`cmake --preset linux-dev && cmake --build --preset linux-dev`).
  - [ ] `ctest` pasa.
  - [ ] Sin regresiones en stubs/parches.
- **Verificación:** comandos concretos.
- **Dependencias:** T-YY, ...
```

## Reglas

- Una tarea = **un cambio lógico**; si el diff supera ~400 líneas, dividirla.
- Tareas pequeñas, con criterios de aceptación **verificables**.
- No mezclar refactor con fix ni docs con feat en una misma tarea sin separación explícita.
- Priorizar: contratos → hooks/stubs → patches → render/audio/input/UI → docs.

## Definition of Done del stage

- [ ] Plan convertido en tareas numeradas (T-XX).
- [ ] Cada tarea con criterios de aceptación y verificación.
- [ ] Dependencias correctas entre tareas.
