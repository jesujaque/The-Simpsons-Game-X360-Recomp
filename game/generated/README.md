# Código generado

Este directorio está **vacío a propósito** y se ignora en git.

`python tools/codegen.py` ejecuta el recompilador sobre tu `default.xex` y escribe aquí el C++
equivalente a las funciones PowerPC del juego. Ese código deriva del binario original, así que no se
distribuye con el repositorio.

No edites nada de `generated/` a mano: se sobrescribe en cada pasada. Los cambios manuales van en
`game/src/` (hooks y parches) o en `game/functions_patch.toml` (fronteras de funciones).
