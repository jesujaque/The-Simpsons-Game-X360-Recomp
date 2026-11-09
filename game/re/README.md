# Ingeniería inversa

Notas de investigación del ejecutable y los datos del juego. Todo aquí es original: nada de
binarios, capturas de assets ni volcados de código propietario.

- [addresses.md](addresses.md) — funciones identificadas y su propósito.
- [boot-sequence.md](boot-sequence.md) — orden de inicialización observado y confirmado.
- [formats.md](formats.md) — formatos de archivo y estructura de datos.

## Convenciones

- Direcciones en hexadecimal con `0x` y 8 dígitos (`0x828385B8`).
- Indica la versión/región del volcado cuando una dirección dependa de ella.
- Nombra las funciones en `snake_case` con prefijo del subsistema: `rw_`, `xam_`, `game_`, `lua_`.
- Si una firma es una hipótesis, márcala con `?`.

## Herramientas

Ghidra + Xenon loader, XenonAnalyse, ReXGlue, Xenia Canary, RenderDoc.
