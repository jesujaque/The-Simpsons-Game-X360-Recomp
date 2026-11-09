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

- [x] Repositorio, licencia, README y documentación.
- [x] Manifiesto y configuración de ReXGlue.
- [x] Herramientas de extracción, verificación, generación y triaje.
- [x] CI que compila el modo doctor en Windows y Linux.
- [x] Modelo de ramas `develop`/`main`, PRs y releases por tag.

**Criterio de salida:** CI en verde y `SimpsonsDoctor` detecta correctamente un volcado.

## Fase 1 — Análisis y línea base (3–5 días)

> Runbook completo: **[PHASE-1-ANALYSIS.md](PHASE-1-ANALYSIS.md)**.

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

### Tareas

- [ ] Generar el código con ReXGlue.
- [ ] Enlazar el runtime y llegar a `main`.
- [ ] Resolver las primeras llamadas no registradas con `tools/triage.py`.
- [ ] Crear la ventana e inicializar el sistema de archivos.
- [ ] Implementar stubs mínimos de XAM/kernel que falten.
- [ ] Conseguir el primer clear/present sin crash.

### Entregables

- Build `SimpsonsRecomp` reproducible en local.
- Lista de hooks/stubs en `game/src/hooks/` y `game/src/patches/`.
- `game/re/addresses.md` actualizado con lo aprendido.

**Criterio de salida:** el proceso arranca, crea ventana y no muere antes del primer frame.

## Fase 3 — Menú principal (1–3 semanas)

### Tareas

- [ ] Depurar el boot hasta el logo de EA.
- [ ] Corregir el cargador de assets (`.str`, `.pkg`).
- [ ] Implementar hooks de sistema de archivos si las rutas no coinciden.
- [ ] Renderizar el frontend sin basura de VRAM.
- [ ] Audio básico en menús.
- [ ] Confirmar navegación con mando y teclado.

**Criterio de salida:** menú principal navegable con música.

## Fase 4 — Primer nivel jugable (2–6 semanas)

### Tareas

- [ ] Cargar el primer episodio (Homer).
- [ ] Comprobar física, colisiones y scripting Lua.
- [ ] Arreglar shaders concretos que fallen en el traductor.
- [ ] Input de mando completo y mapeo de teclado.
- [ ] Guardado y carga.

**Criterio de salida:** primer nivel completable sin crashes bloqueantes.

## Fase 5 — Estabilidad (3–8 semanas)

### Tareas

- [ ] Recorrer los 18 episodios con un save completo.
- [ ] Corregir regresiones de audio y sincronización.
- [ ] Automatizar pruebas de arranque por episodio.
- [ ] Launcher e instalador desde ISO.
- [ ] Reducir warnings y deuda técnica.

**Criterio de salida:** campaña completa y cierre limpio.

## Fase 6 — Mejoras de port (continuo)

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
