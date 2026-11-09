# Funciones identificadas

> Región de referencia: USA (`45410809`). Las direcciones pueden variar entre revisiones.

| Dirección | Nombre propuesto | Subsistema | Notas |
| --- | --- | --- | --- |
| `0x82A71810` | `xstart_init_callback` | boot | Callback de constructor global (ejemplo) |
| `0x828385B8` | `content_load_level` | fs/content | Handler de `*.lvl` |
| `0x82837EC0` | `content_load_costume` | fs/content | Handler de `*.costume` |
| `0x82836670` | `content_run_lua` | fs/content | Handler de `*.lua` |

## Pendientes

- `game_main` / bucle principal.
- `rw_engine_init`, `rw_world_open`, `rw_camera_*`.
- Carga de `Frontend.str`.
- Timestep del juego (para el parche de FPS).

## Cómo añadir una entrada

1. Confirma la dirección en Ghidra y anota la firma.
2. Añádela a `game/functions_patch.toml` si era una llamada indirecta.
3. Actualiza esta tabla con el nombre y el contexto.
