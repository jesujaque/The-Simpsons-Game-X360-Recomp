# Roadmap

Objetivo: un build jugable de principio a fin en el menor tiempo razonable, priorizando siempre el
camino crítico. Las estimaciones suponen una persona con experiencia en C++ y conocimientos básicos
de recompilación, trabajando a tiempo parcial.

## Resumen

| Fase | Hito | Duración | Entregable principal |
| --- | --- | --- | --- |
| 0 | Andamiaje | — | Repo, CI y documentación |
| 1 | Análisis | 3–5 días | `game/re/*` + candidatos de funciones |
| 2 | Primer enlace | 3–7 días | Ventana + runtime inicializado |
| 3 | Menú principal | 1–3 semanas | Frontend navegable |
| 4 | Primer nivel | 2–6 semanas | Episodio jugable |
| 5 | Estabilidad | 3–8 semanas | Campaña completa |
| 6 | Mejoras de port | continuo | 60 FPS, ultrawide, launcher |

## Fase 0 — Andamiaje (hecho)

> Documentación de la fase: **[000-andamiaje](000-andamiaje/README.md)**.

- [x] Repositorio, licencia, README y documentación.
- [x] Manifiesto y configuración de ReXGlue.
- [x] Herramientas de extracción, verificación, generación y triaje.
- [x] CI que compila el modo doctor en Windows y Linux.
- [x] Modelo de ramas `develop`/`main`, PRs y releases por tag.

**Criterio de salida:** CI en verde y `SimpsonsDoctor` detecta correctamente un volcado.

## Fase 1 — Análisis y línea base (3–5 días)

> Documentación de la fase: **[001-analisis-y-linea-base](001-analisis-y-linea-base/README.md)**.
> Runbook completo: **[PHASE-1-ANALYSIS.md](001-analisis-y-linea-base/PHASE-1-ANALYSIS.md)**.

### Tareas

- [ ] Instalar Ghidra + JDK 21 + XEXLoaderWV + XenonAnalyse + Xenia Canary.
- [ ] Extraer la ISO USA y verificar el `default.xex` con `tools/verify_dump.py`.
- [ ] Registrar hashes, Title ID, región y tamaño del volcado.
- [ ] Inventariar archivos: `.str`, `.pkg`, `.lvl`, `.lua`, `Frontend.str`, `cmd.txt`.
- [ ] Ejecutar el juego en Xenia y resumir la secuencia de arranque.
- [ ] Importar el XEX en Ghidra y confirmar la base de memoria.
- [ ] Documentar imports de XAM, kernel, XInput, XAudio y XMA.
- [ ] Localizar entrypoint, CRT/constructores, heap y sistema de archivos.
- [ ] Localizar init de RenderWare, audio, input y frontend.
- [ ] Ejecutar XenonAnalyse y cruzar funciones con Ghidra.
- [ ] Documentar al menos 25 funciones con evidencia y confianza.
- [ ] Añadir candidatos a `game/functions_patch.toml`.
- [ ] Abrir PR a `develop` con toda la documentación.

### Entregables

| Artefacto | Ubicación |
| --- | --- |
| Informe del volcado | `game/re/dump-report.md` |
| Secuencia de arranque | `game/re/boot-sequence.md` |
| Direcciones y firmas | `game/re/addresses.md` |
| Formatos | `game/re/formats.md` |
| Candidatos de funciones | `game/functions_patch.toml` |

**Criterio de salida:** el camino de arranque está explicado de principio a fin, los imports están
documentados y el codegen de la fase 2 puede ejecutarse sin incógnitas estructurales.

## Fase 2 — Primer enlace (3–7 días)

> Documentación de la fase: **[002-primer-enlace](002-primer-enlace/README.md)**.
> Runbook de implementación: **[PHASE-2-IMPLEMENTATION.md](002-primer-enlace/PHASE-2-IMPLEMENTATION.md)**.
> La infraestructura sin SDK está implementada; el enlace real requiere el volcado y el SDK.
> Seguimiento: [#2 Codegen + primer enlace](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/2)
> y [#5 Hooks confirmados](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/5).

### Tareas

- [x] Contrato de runtime (`RuntimeBackend`) y backend nulo de desarrollo.
- [x] Carga de `config.toml` y validación de rangos.
- [x] Sistema de archivos virtual con montajes `d/device/game/save/cache`.
- [x] Registro de hooks/stubs y acumulación de llamadas no resueltas.
- [x] Traza de arranque con informe Markdown (`--trace-out`).
- [x] Ciclo de vida de la ventana/present (simulado sin SDK).
- [ ] Generar el código con ReXGlue (requiere `default.xex` + SDK).
- [ ] Enlazar el runtime real y llegar a `main`.
- [ ] Resolver las primeras llamadas no registradas con `tools/triage.py`.
- [ ] Conseguir el primer clear/present real.

### Entregables

- Build `SimpsonsRecomp` reproducible en local.
- Lista de hooks/stubs en `game/src/hooks/` y `game/src/patches/`.
- `game/re/addresses.md` actualizado con lo aprendido.

**Criterio de salida:** el proceso arranca, crea ventana (o modo headless) y no muere antes del
primer frame; la traza marca `Entrypoint`…`Input` como alcanzadas.

## Fase 3 — Menú principal (1–3 semanas)

> Documentación de la fase: **[003-menu-principal](003-menu-principal/README.md)**.
> Runbook de implementación: **[PHASE-3-IMPLEMENTATION.md](003-menu-principal/PHASE-3-IMPLEMENTATION.md)**.
> La infraestructura de assets, rutas, entrada y audio está implementada y probada.
> Seguimiento: [#4 Assets y frontend](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/4)
> y [#3 Navegación y audio](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/3).

### Tareas

- [x] Catálogo e inventario de assets; validación de `Frontend.str`.
- [x] Sondeo de cabeceras (`.str`, `.pkg`, `.lvl`, `.lua`, XEX, BIK).
- [x] Hooks de sistema de archivos con resolución tolerante a mayúsculas.
- [x] Mapa de acciones mando/teclado configurable (`[input.bindings]`).
- [x] Audio configurable (volúmenes y estado) sobre el mixer stub.
- [ ] Depurar el boot hasta el logo de EA.
- [ ] Corregir el cargador real de assets (`.str`, `.pkg`).
- [ ] Renderizar el frontend sin basura de VRAM.
- [ ] Audio básico en menús con XMA real.
- [ ] Confirmar navegación con mando y teclado.

**Criterio de salida:** menú principal navegable con música; la traza marca `Frontend` como
alcanzada.

## Fase 4 — Primer nivel jugable (2–6 semanas)

> Documentación de la fase: **[004-primer-nivel-jugable](004-primer-nivel-jugable/README.md)**.

### Tareas

- [ ] Cargar el primer episodio (Homer).
- [ ] Comprobar física, colisiones y scripting Lua.
- [ ] Arreglar shaders concretos que fallen en el traductor.
- [ ] Input de mando completo y mapeo de teclado.
- [ ] Guardado y carga.

**Criterio de salida:** primer nivel completable sin crashes bloqueantes.

## Fase 5 — Estabilidad (3–8 semanas)

> Documentación de la fase: **[005-estabilidad](005-estabilidad/README.md)**.

### Tareas

- [ ] Recorrer los 18 episodios con un save completo.
- [ ] Corregir regresiones de audio y sincronización.
- [ ] Automatizar pruebas de arranque por episodio.
- [ ] Launcher e instalador desde ISO.
- [ ] Reducir warnings y deuda técnica.

**Criterio de salida:** campaña completa y cierre limpio.

## Fase 6 — Mejoras de port (continuo)

> Documentación de la fase: **[006-mejoras-de-port](006-mejoras-de-port/README.md)**.

- [ ] 60 FPS con lógica a velocidad original (timestep fijo).
- [ ] Ultrawide y FOV ajustable.
- [ ] Escalado de resolución, AA y filtrado anisotrópico.
- [ ] Skipping de logos, selector de episodios.
- [ ] Accesibilidad: subtítulos, tamaño de UI, remapeo completo.
- [ ] Renderer nativo progresivo (sustituir rutas de Xenos).

## Riesgos

| Riesgo | Impacto | Mitigación |
| --- | --- | --- |
| Funciones no detectadas por el analizador | Alto | Triaje iterativo + escaneo de punteros en secciones de datos |
| Shaders no traducibles | Alto | XenosRecomp + fallback; aislar por episodio |
| Diferencias entre versiones USA/EU | Medio | Fijar USA como objetivo, EU como *best effort* |
| Falta de documentación del motor | Medio | Comparar con Xenia, RenderWare docs y notas de la comunidad |
| Tiempo | Alto | Reutilizar ReXGlue y el prior art; no reescribir runtime |

## Definition of Done (todas las fases)

- [ ] El cambio está en una rama y pasa por PR a `develop`.
- [ ] CI en verde.
- [ ] Documentación actualizada (`game/re/` si es investigación).
- [ ] Sin assets ni binarios del juego en el commit.
- [ ] El build doctor sigue funcionando.
