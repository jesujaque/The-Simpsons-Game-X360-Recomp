# Fase 2 — Primer enlace

> **Estado:** infraestructura implementada; enlace real pendiente del XEX y del
> SDK de ReXGlue.

## Alcance

Generar el C++ recompilado, enlazarlo con el runtime y llegar a `main` sin que
el proceso muera antes del primer frame.

## Documentos

| Documento | Contenido |
| --- | --- |
| [PHASE-2-IMPLEMENTATION.md](PHASE-2-IMPLEMENTATION.md) | Contrato del runtime, VFS, hooks, triaje y activación de ReXGlue |

## Tareas abiertas

- [ ] Generar el código con ReXGlue (requiere `default.xex` + SDK).
- [ ] Enlazar el runtime real y llegar a `main`.
- [ ] Resolver las primeras llamadas no registradas con `tools/triage.py`.
- [ ] Conseguir el primer clear/present real.

## Criterio de salida

El proceso arranca, crea ventana (o modo headless) y no muere antes del primer
frame; la traza marca `Entrypoint`…`Input` como alcanzadas.

## Seguimiento

| Issue | Contenido |
| --- | --- |
| [#2](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/2) | Codegen + primer enlace de ReXGlue |
| [#5](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/5) | Activar hooks confirmados desde `addresses.md` |

## Enlaces

- [Roadmap](../ROADMAP.md)
- [Fase 1](../001-analisis-y-linea-base/README.md)
- [Fase 3](../003-menu-principal/README.md)
