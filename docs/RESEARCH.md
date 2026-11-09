# Investigación

Notas técnicas sobre el binario y los datos del juego. Todo se obtiene de una copia propia y se
documenta aquí para no repetir trabajo. **No se incluyen binarios ni datos extraídos.**

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

## Herramientas

| Herramienta | Uso |
| --- | --- |
| Ghidra + Xenon loader | Desensamblado y decompilado |
| XenonAnalyse | Detección de funciones y tablas |
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

> Sustituye estos ejemplos por las direcciones que confirmes en tu volcado. Las direcciones pueden
> variar entre revisiones.

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

## Pendiente

- [ ] Firmas de las funciones de render (RenderWare: `RwEngineInit`, cámara, world).
- [ ] Formato exacto de `.str` y tabla de recursos.
- [ ] Localización de la tabla de episodios en memoria.
- [ ] Identificar el timestep del juego para el parche de 60 FPS.
