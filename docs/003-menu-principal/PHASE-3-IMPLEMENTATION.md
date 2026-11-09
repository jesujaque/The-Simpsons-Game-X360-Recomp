# Fase 3 — Menú principal

> **Estado:** infraestructura de assets, rutas, entrada y audio implementada y
> probada sin SDK ni XEX. El arranque real hasta el frontend requiere el
> `default.xex` del usuario y el runtime de ReXGlue enlazado.
>
> Runbook anterior: [fase 2 — primer enlace](../002-primer-enlace/PHASE-2-IMPLEMENTATION.md).
> Hoja de ruta: [ROADMAP.md](../ROADMAP.md).

## 1. Qué se ha implementado

| Área | Código | Qué hace |
| --- | --- | --- |
| Assets | `game/src/assets/asset_catalog.*` | Inventario, `Frontend.str`, `.str/.pkg/.lvl/.lua` y sondeo de cabecera. |
| Sistema de archivos | `game/src/platform/vfs.*` | Traduce rutas Xbox y resuelve mayúsculas componente a componente. |
| Entrada | `game/src/input/keymap.*` | Mapa acción → código de mando/teclado desde `[input.bindings]`. |
| Audio | `game/src/audio/audio.*` | Volúmenes y estado; stub del mixer XMA. |
| Traza de arranque | `game/src/app/boot_trace.*` | Marca `Frontend`, `Gameflow` y `Level`, o las omite con motivo. |

El build doctor (`SIMPSONS_FULL_BUILD=OFF`) compila todo este núcleo y las
pruebas. El frontend real solo se puede validar con el runtime enlazado.

## 2. Flujo del frontend

```
Application::Run
  ├─ VirtualFileSystem::Mount
  ├─ AssetCatalog::Scan
  │    ├─ inventario de .str/.pkg/.lvl/.lua
  │    ├─ validación de Frontend.str
  │    └─ sondeo de cabeceras (XEX, Lua, STR, PKG, LVL, BIK)
  ├─ input::Initialize
  │    └─ Keymap desde [input.bindings] + bindings por defecto
  ├─ audio::Initialize
  │    └─ volúmenes + estado del mixer
  ├─ CreateRuntimeBackend
  │    └─ Run(FrameCallbacks{poll_input, draw_ui, present})
  └─ BootTrace::WriteMarkdown
```

El backend nulo deja `Frontend` como **omitida** con el motivo
“requiere el entrypoint recompilado del XEX”. Un build sin XEX no puede declarar
la fase 3 por accidente.

## 3. Assets y rutas

El juego original usa rutas como `d:\Frontend.str`, `device:\...` o rutas
relativas a la raíz del disco. `VirtualFileSystem` monta:

| Dispositivo | Destino |
| --- | --- |
| `d`, `device`, `game` | `--game-root` |
| `save` | `<game-root>/<paths.save_dir>` |
| `cache` | `<game-root>/<paths.cache_dir>` |
| rutas absolutas (`\foo`) y dispositivos desconocidos | `--game-root` |

Reglas de carga:

- `ResolveExisting` corrige mayúsculas componente a componente; es necesario en
  Linux y útil en Windows con datos copiados.
- Cualquier `..` que intente salir de la raíz devuelve `nullopt` y debe tratarse
  como fallo de carga.
- `Frontend.str` es un asset requerido: si falta, la traza lo registra como
  `[fs] falta un asset requerido` y la fase 3 no se puede cerrar.
- El sondeo de cabecera clasifica el archivo sin cargarlo entero en memoria.

## 4. Entrada de mando y teclado

`game/src/input/keymap.*` define un mapa acción → código físico:

- Bindings por defecto para mando y teclado.
- `[input.bindings]` en `config.toml` permite sobrescribir acciones.
- El runtime real debe entregar códigos físicos; el backend nulo solo registra
  el mapa y advierte de que no hay dispositivo.
- Las acciones desconocidas se registran en el log para no fallar en silencio.

## 5. Audio

`game/src/audio/audio.*` implementa la parte configurable:

- Volúmenes `master`, `music` y `sfx` validados desde `config.toml`.
- Estado del mixer y mensaje explícito de que el decodificador XMA sigue
  pendiente del runtime.
- El stub no simula audio: solo deja el estado listo para el mixer real.

## 6. Criterios de salida medibles

| Criterio | Cómo se comprueba |
| --- | --- |
| `Frontend.str` cargado | `Frontend` alcanzada y sin `[fs] falta un asset requerido` |
| Navegación mando/teclado | `[input] mapa con N bindings` y acciones del runtime |
| Audio de menús | `[audio] mixer inicializado`; `Audio` alcanzada |
| Render real del menú | Frames presentados por el backend real, no por el present simulado |

## 7. Pruebas

`tests/test_main.cpp` cubre el núcleo sin SDK:

- Assets: clasificación, inventario, `Frontend.str`, magic XEX/Lua.
- VFS: montajes, rutas absolutas y relativas, `..`, mayúsculas.
- Entrada: bindings por defecto y desde configuración.
- Audio: volúmenes y estado inicial.

```bash
cmake --preset windows-dev
cmake --build --preset windows-dev
ctest --test-dir build/windows-dev --output-on-failure
```

## 8. Tareas que siguen dependiendo del volcado

Estas casillas del [ROADMAP.md](../ROADMAP.md) siguen abiertas a propósito:

- [ ] Depurar el boot hasta el logo de EA.
- [ ] Corregir el cargador de assets y renderizar el frontend real.
- [ ] Confirmar navegación con mando y teclado.
- [ ] Audio básico en menús con XMA real.

Cada una necesita ejecutar el binario del usuario; el repositorio no incluye
`default.xex`, paquetes ni código generado.

## 9. Seguimiento

| Issue | Contenido |
| --- | --- |
| [#4](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/4) | Cargador de `.str/.pkg` y render del frontend |
| [#3](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/3) | Navegación y audio de menús |
