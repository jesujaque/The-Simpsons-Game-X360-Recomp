# Formatos de archivo

## Contenedores observados

| Extensión | Uso probable | Estado |
| --- | --- | --- |
| `.str` | Paquete de streaming (frontend, niveles) | Sin documentar |
| `.pkg` | Paquete de datos | Sin documentar |
| `.lvl` | Nivel cargable por el content manager | Parcial |
| `.costume` | Traje alternativo | Parcial |
| `.lua` | Script de gameplay | Texto/bytecode por confirmar |

## Convenciones de documentación

Para cada formato documenta:

1. Cabecera (magic, versión, endianness).
2. Tabla de offsets y tamaños.
3. Compresión/cifrado, si existe.
4. Cómo se referencia desde el ejecutable.
5. Herramienta de extracción propia (si la hay).

## Notas

- RenderWare usa habitualmente *clumps* para modelos y *texture dictionaries*; confirmar variantes.
- El frontend contiene texturas de DLC no usado (`episode_download_*.png` en la UI).
- `cmd.txt` se lee de la raíz del disco al arrancar.
