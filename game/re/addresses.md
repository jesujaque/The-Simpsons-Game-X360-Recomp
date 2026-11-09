# Funciones identificadas

> Región de referencia: USA (`45410809`). Las direcciones pueden variar entre revisiones y entre
> regiones. Las filas marcadas como **ejemplo** son candidatas sin verificar: sustitúyelas por tus
> propios hallazgos.

## Leyenda

| Confianza | Significado |
| --- | --- |
| **Confirmada** | Verificada en ejecución o por dos fuentes independientes |
| **Probable** | String/xref/patrón fuerte, sin prueba runtime |
| **Hipótesis** | Solo nombre o patrón; pendiente de confirmar |

## Tabla

| Dirección | Nombre | Subsistema | Evidencia | Confianza |
| --- | --- | --- | --- | --- |
| `0x82A71810` | `boot_init_callback` (ejemplo) | boot | xref desde tabla de constructores | Hipótesis |
| `0x828385B8` | `content_load_level` (ejemplo) | fs/content | handler de `*.lvl` | Hipótesis |
| `0x82837EC0` | `content_load_costume` (ejemplo) | fs/content | handler de `*.costume` | Hipótesis |
| `0x82836670` | `content_run_lua` (ejemplo) | fs/content | handler de `*.lua` | Hipótesis |

## Objetivo mínimo de fase 1

- [ ] Entrypoint del XEX.
- [ ] Función de arranque del CRT / constructores.
- [ ] Init de heap.
- [ ] Apertura/montaje del sistema de archivos.
- [ ] `RwEngineInit` y creación de device.
- [ ] Init de audio.
- [ ] Init de input.
- [ ] Carga de `Frontend.str`.
- [ ] Entrada del gameflow Lua.
- [ ] Al menos 25 funciones documentadas en total.

## Plantilla de función

```markdown
### `0x00000000` — `nombre_propuesto`

- **Firma hipotética:** `void nombre_propuesto(int a, const char* b)`
- **Llamadores:** `0x...`
- **Llamadas:** `0x...`
- **Evidencia:** string `...`, xref en `.rdata`, log de Xenia.
- **Confianza:** probable
- **Notas:** candidata a `functions_patch.toml`; el runtime necesita un stub de `Xam...`.
```

## Imports relevantes

| Módulo | Función | Stub | Uso esperado |
| --- | --- | --- | --- |
| `xam.xex` | `XamUserGetSigninState` | | Perfil |
| `xam.xex` | `XamContentCreateEnumerator` | | Contenido |
| `xam.xex` | `XamSave*` | | Guardado |
| `xinput1_3.dll` | `XInputGetState` | | Mando |
| `xaudio2_*.dll` / `xapilib` | `XMA*` | | Audio |
| `ntdll` | `NtCreateFile` | | Archivos |

## Convenciones

- Direcciones de 8 dígitos con `0x`.
- Prefijos: `boot_`, `fs_`, `xam_`, `rw_`, `gpu_`, `audio_`, `input_`, `lua_`, `game_`.
- Una fila por función; si hay varias hipótesis, usa notas en la sección de detalle.

## Cómo añadir una entrada

1. Confirma la dirección en Ghidra (o con XenonAnalyse) y anota la firma.
2. Añade la fila a la tabla con evidencia y confianza.
3. Si era una llamada indirecta, añádela a `game/functions_patch.toml`.
4. Si el fallo es de comportamiento, prepara un hook en `game/src/hooks/`.
