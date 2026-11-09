# Roadmap

Objetivo: un build jugable de principio a fin en el menor tiempo razonable, priorizando siempre el
camino crítico. Las estimaciones suponen una persona con experiencia en C++ y conocimientos básicos
de recompilación, trabajando a tiempo parcial.

## Fase 0 — Andamiaje (hecho)

- [x] Repositorio, licencia, README y documentación.
- [x] Manifiesto y configuración de ReXGlue.
- [x] Herramientas de extracción, generación y triaje.
- [x] CI que compila el modo doctor en Windows y Linux.

**Criterio de salida:** `ctest`/CI en verde y `SimpsonsDoctor` detecta correctamente un volcado.

## Fase 1 — Análisis (2–4 días)

- [ ] Extraer la ISO USA y verificar el hash del `default.xex`.
- [ ] Cargar el XEX en Ghidra con el plugin de Xenon.
- [ ] Mapear secciones, entrypoint, tablas de saltos y vtables.
- [ ] Documentar en `game/re/addresses.md` las funciones clave (boot, filesystem, render init).
- [ ] Ejecutar el juego en Xenia Canary y capturar un log de referencia.

**Criterio de salida:** codegen completo sin funciones huérfanas conocidas y un log de Xenia con la
secuencia de inicialización.

## Fase 2 — Primer enlace (3–7 días)

- [ ] Generar el código con ReXGlue.
- [ ] Enlazar el runtime y llegar a `main`.
- [ ] Resolver las primeras llamadas no registradas.
- [ ] Conseguir que se cree la ventana y se inicialice el sistema de archivos.

**Criterio de salida:** el proceso arranca, crea ventana y no muere antes del primer frame.

## Fase 3 — Menú principal (1–3 semanas)

- [ ] Depurar el boot hasta el logo de EA.
- [ ] Corregir el cargador de assets (`.str`, `.pkg`).
- [ ] Implementar hooks de sistema de archivos si las rutas no coinciden.
- [ ] Renderizar el frontend sin basura de VRAM.
- [ ] Audio básico en menús.

**Criterio de salida:** menú principal navegable con música.

## Fase 4 — Primer nivel jugable (2–6 semanas)

- [ ] Cargar el primer episodio (Homer).
- [ ] Comprobar física, colisiones y scripting Lua.
- [ ] Arreglar shaders concretos que fallen en el traductor.
- [ ] Input de mando completo y mapeo de teclado.
- [ ] Guardado y carga.

**Criterio de salida:** primer nivel completable sin crashes bloqueantes.

## Fase 5 — Estabilidad (3–8 semanas)

- [ ] Recorrer los 18 episodios con un save completo.
- [ ] Corregir regresiones de audio y sincronización.
- [ ] Automatizar pruebas de arranque por episodio.
- [ ] Launcher e instalador desde ISO.

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
