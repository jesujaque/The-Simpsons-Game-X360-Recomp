# Formatos de archivo

Notas sobre contenedores y estructuras de datos. Ningún binario ni asset se versiona aquí.

## Prioridad de fase 1

| Extensión | Uso probable | Qué necesitamos saber |
| --- | --- | --- |
| `.str` | Paquete de streaming (frontend, niveles) | Cabecera, tabla de recursos, compresión |
| `.pkg` | Paquete de datos | Índice y offsets |
| `.lvl` | Nivel cargable por el content manager | Relación con `.str` y scripts |
| `.costume` | Traje alternativo | Estructura y dependencias |
| `.lua` | Script de gameplay | ¿Texto o bytecode? ¿Versión de Lua? |

## Método de inspección

1. **Tamaño y tipo:** `file`, extensión y primeros 64 bytes con `xxd`.
2. **Strings:** `strings -n 6` para nombres de recursos y rutas.
3. **Entropía:** si es alta, probablemente comprimido o cifrado.
4. **Cabecera:** busca magic, versión, endianness (`FF FE`, `FE FF`, `XFIR`, etc.).
5. **Referencias en el ejecutable:** localiza la función que abre el archivo y el string de extensión.
6. **Documenta** en este archivo: offset, tipo, tamaño, significado y evidencia.

Comandos útiles:

```bash
xxd -l 128 archivo.str
strings -n 6 archivo.str | head -50
python - <<'PY'
from pathlib import Path
data = Path("archivo.str").read_bytes()[:64]
print(data.hex(" "))
PY
```

## Sondeo automático en el port

`SimpsonsRecomp` inventaría los datos extraídos al arrancar y puede volcar los
primeros 64 bytes de un asset para clasificarlo:

```
[fs] montajes: d/device/game -> game/extracted, save -> ..., cache -> ...
[fs] falta un asset requerido: Frontend.str
```

`AssetCatalog::Probe` reconoce `XEX2`, `BIK` y bytecode Lua (`1B 4C 75 61`), y
marca el resto como texto o binario sin inventar un formato. Usa ese resultado
para rellenar las secciones de abajo; no sustituye al análisis de Ghidra.

## Contenedores observados

### `.str`

- **Estado:** sin documentar.
- **Hipótesis:** paquete de recursos con tabla de offsets; RenderWare suele usar *texture
  dictionaries* y *clumps*.
- **Pendiente:** magic, versión, tabla de recursos, compresión.

### `.pkg`

- **Estado:** sin documentar.
- **Pendiente:** índice, offsets y relación con el sistema de contenido.

### `.lvl`

- **Estado:** parcial.
- **Notas:** el content manager registra un handler para esta extensión. El DLC cancelado dejó código
  de carga adicional.
- **Pendiente:** cabecera y dependencias.

### `.costume`

- **Estado:** parcial.
- **Notas:** handler registrado junto a `.lvl` y `.lua`; los costumes se referencian desde el gameflow.
- **Pendiente:** estructura y validación.

### `.lua`

- **Estado:** por confirmar.
- **Notas:** `simpsons_gameflow.lua`, `simpsons_gameflow_helpers.lua` y `simpsons_scores.lua`
  configuran episodios, costumes y puntuaciones. Comprueba si el juego carga texto o bytecode.
- **Pendiente:** versión de Lua y API expuesta al juego.

## Ficheros de configuración

### `cmd.txt`

- Se lee de la raíz del disco al arrancar.
- Contiene flags separados por espacios.
- Útil para forzar modos de arranque y saltar partes del flujo.
- **No** se versiona: es datos del usuario.

## Convenciones de documentación

Para cada formato documenta:

1. Cabecera (magic, versión, endianness).
2. Tabla de offsets y tamaños.
3. Compresión/cifrado, si existe.
4. Cómo se referencia desde el ejecutable.
5. Herramienta de extracción propia, si la hay.
6. Ejemplo de estructura en pseudocódigo, nunca un volcado completo.

## Pendiente

- [ ] Confirmar magic de `.str` y `.pkg`.
- [ ] Localizar la tabla de recursos del frontend.
- [ ] Determinar si `.lua` es texto o bytecode.
- [ ] Documentar la relación `.lvl` ↔ `.str`.
