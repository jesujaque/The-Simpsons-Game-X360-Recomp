# Fase 3 — Menú principal

> **Estado:** infraestructura implementada; frontend real pendiente del runtime
> enlazado y de los assets del usuario.

## Alcance

Arrancar hasta el logo de EA, cargar `Frontend.str`, renderizar el menú
principal y confirmar navegación con mando/teclado y audio de menús.

## Documentos

| Documento | Contenido |
| --- | --- |
| [PHASE-3-IMPLEMENTATION.md](PHASE-3-IMPLEMENTATION.md) | Catálogo de assets, VFS, keymap, audio y criterios del frontend |

## Tareas abiertas

- [ ] Depurar el boot hasta el logo de EA.
- [ ] Corregir el cargador real de assets (`.str`, `.pkg`).
- [ ] Renderizar el frontend sin basura de VRAM.
- [ ] Audio básico en menús con XMA real.
- [ ] Confirmar navegación con mando y teclado.

## Criterio de salida

Menú principal navegable con música; la traza marca `Frontend` como alcanzada.

## Seguimiento

| Issue | Contenido |
| --- | --- |
| [#4](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/4) | Cargador de `.str/.pkg` y render del frontend |
| [#3](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/3) | Navegación y audio de menús |

## Enlaces

- [Roadmap](../ROADMAP.md)
- [Fase 2](../002-primer-enlace/README.md)
- [Fase 4](../004-primer-nivel-jugable/README.md)
