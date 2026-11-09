# Investigación

Notas técnicas sobre el binario y los datos del juego. Todo se obtiene de una copia propia y se
documenta aquí para no repetir trabajo. **No se incluyen binarios ni datos extraídos.**

> **Fase actual:** [Fase 1 — Análisis y línea base](001-analisis-y-linea-base/README.md). El runbook está en
> [PHASE-1-ANALYSIS.md](001-analisis-y-linea-base/PHASE-1-ANALYSIS.md). Ese documento es el
> runbook operativo; esta página es el índice de hallazgos.

## Identificación

| Campo | Valor |
| --- | --- |
| Título | The Simpsons Game |
| Plataforma | Xbox 360 |
| Desarrollador | EA Redwood Shores |
| Distribuidor | Electronic Arts |
| Lanzamiento | 30/10/2007 (USA) |
| Motor | RenderWare |
| Title ID (USA) | `45410809` |
| Ejecutable | `default.xex` (`simpsons_f.exe`) |
| Región objetivo | USA |

## Metodología

1. **Verificar** el volcado (`tools/verify_dump.py`) y registrar hashes.
2. **Observar** el juego en Xenia Canary y anotar la secuencia de arranque.
3. **Importar** el XEX en Ghidra con XEXLoaderWV.
4. **Detectar** funciones con XenonAnalyse y cruzarlas con Ghidra.
5. **Documentar** dirección, firma, evidencia y confianza.
6. **Clasificar** qué necesita el runtime y qué basta con un hook.
7. **Convertir** las llamadas indirectas en entradas de `functions_patch.toml`.

Regla: una hipótesis sin evidencia se marca como hipótesis. No se inventan nombres ni firmas.

## Documentos

| Documento | Contenido |
| --- | --- |
| [PHASE-1-ANALYSIS.md](001-analisis-y-linea-base/PHASE-1-ANALYSIS.md) | Runbook completo de la fase 1 |
| [../game/re/addresses.md](../game/re/addresses.md) | Funciones identificadas |
| [../game/re/boot-sequence.md](../game/re/boot-sequence.md) | Orden de inicialización |
| [../game/re/formats.md](../game/re/formats.md) | Contenedores y estructuras |

El informe del volcado (`game/re/dump-report.md`) se genera localmente con
`tools/verify_dump.py`. Se puede versionar, pero revisa que no contenga rutas personales.

## Herramientas

| Herramienta | Uso |
| --- | --- |
| Ghidra + XEXLoaderWV | Desensamblado y decompilado |
| XenonAnalyse | Detección de funciones, jump tables y vtables |
| ReXGlue | Codegen y runtime |
| Xenia Canary | Referencia de ejecución y logs |
| RenderDoc | Captura de frames |
| 010 Editor / hex editor | Formatos propietarios |

## Puntos de entrada conocidos

Anota aquí direcciones absolutas y el módulo al que pertenecen. Ejemplo de formato:

```
0x82A71810  xstart / constructor de arranque   (ver functions_patch.toml)
0x828385B8  handler de contenido *.lvl
0x82837EC0  handler de contenido *.costume
0x82836670  handler de contenido *.lua
```

> Estas direcciones son **candidatas sin verificar** hasta que las confirmes en tu volcado. Las
> direcciones pueden variar entre revisiones y regiones.

## Sistema de archivos

- El juego lee `cmd.txt` de la raíz del disco al arrancar; contiene flags separados por espacios.
- El flujo de episodios está definido en `simpsons_gameflow.lua` y sus helpers.
- Los paquetes grandes usan extensiones `.str`; el frontend vive en `Frontend.str`.

## Sistema de contenido (DLC cancelado)

En el boot se registra un content manager con handlers para `.lvl`, `.costume` y `.lua`, y un
state machine que enumera paquetes instalados mediante `XamContentCreateEnumerator`. Aunque el DLC
nunca salió, el código sigue presente y es útil para entender la carga de niveles.

## Guardado

- El guardado comprueba integridad y dependencias de paquetes de contenido.
- Si un save referencia contenido ausente, se muestra un aviso y se rechaza la carga.

## Hallazgos abiertos

- [ ] Firmas de las funciones de render (RenderWare: `RwEngineInit`, cámara, world).
- [ ] Formato exacto de `.str` y tabla de recursos.
- [ ] Localización de la tabla de episodios en memoria.
- [ ] Identificar el timestep del juego para el parche de 60 FPS.
- [ ] Confirmar si `.lua` se carga como texto o bytecode.

## Cómo contribuir hallazgos

- Añade una fila a `game/re/addresses.md` con evidencia y confianza.
- Si la función era una llamada indirecta, añádela a `game/functions_patch.toml` con un comentario.
- Si cambia el mapa de arranque, actualiza `game/re/boot-sequence.md`.
- Abre un PR a `develop` (ver [CONTRIBUTING.md](../CONTRIBUTING.md)).
