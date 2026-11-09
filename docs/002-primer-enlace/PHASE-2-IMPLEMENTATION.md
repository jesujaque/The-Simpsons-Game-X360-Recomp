# Fase 2 — Primer enlace

> **Estado:** infraestructura del port implementada y probada sin SDK ni XEX.
> El enlace real con ReXGlue requiere tu `default.xex` y una release del SDK; no
> se versionan en el repositorio.
>
> Runbook anterior: [fase 1](../001-analisis-y-linea-base/PHASE-1-ANALYSIS.md).
> Siguiente fase: [fase 3 — menú principal](../003-menu-principal/PHASE-3-IMPLEMENTATION.md).
> Hoja de ruta: [ROADMAP.md](../ROADMAP.md).

## 1. Qué se ha implementado

La fase 2 tiene dos mitades: una infraestructura que puede vivir en el
repositorio y un enlace que solo se puede validar con el binario del usuario.
Esta entrega cubre la primera mitad y deja costuras explícitas para la segunda.

| Área | Código | Qué hace |
| --- | --- | --- |
| Configuración | `game/src/app/config.*` | Lee `config.toml` (TOML reducido, sin dependencias) y valida rangos. |
| Sistema de archivos | `game/src/platform/vfs.*` | Traduce `d:\`, `device:\`, `save:\`, `cache:\` a rutas del host. |
| Traza de arranque | `game/src/app/boot_trace.*` | Registra cada etapa, fallos y omisiones; genera Markdown. |
| Hooks y stubs | `game/src/hooks/hook_registry.*` | Registra sustituciones y acumula llamadas no resueltas. |
| Runtime | `game/src/app/runtime.*` | Contrato `RuntimeBackend` + backend nulo; fábrica para ReXGlue. |

El build doctor (`SIMPSONS_FULL_BUILD=OFF`) compila todo el núcleo y las
pruebas. El build completo sigue necesitando el SDK y el código generado.

## 2. Flujo de arranque

```
main.cpp
  └─ Application::Run
       ├─ LoadConfig(config.toml)            [config.*]
       ├─ FindGameRoot / LooksLikeGameRoot   [platform/paths.*]
       ├─ VirtualFileSystem::Mount           [platform/vfs.*]
       ├─ AssetCatalog::Scan                 [assets/asset_catalog.*]
       ├─ HookRegistry + InstallHooks        [hooks/*]
       ├─ renderer::Initialize               [renderer/*]
       ├─ audio::Initialize                  [audio/*]
       ├─ input::Initialize                  [input/*]
       ├─ CreateRuntimeBackend               [app/runtime.*]
       │    ├─ backend nulo (por defecto)
       │    └─ ReXGlue real (SIMPSONS_HAVE_REXGLUE_API)
       └─ BootTrace::WriteMarkdown           [app/boot_trace.*]
```

Ejecución con backend nulo:

```bash
./build/bin/SimpsonsRecomp --game-root game/extracted \
  --config config.toml --trace-out game/re/logs/boot-trace.md
```

El backend nulo simula tres frames, marca `Entrypoint`, `CRT` y `Heap`, y deja
`Frontend`, `Gameflow` y `Level` como **omitidas** con el motivo
“requiere el entrypoint recompilado del XEX”. Así el criterio de salida es
medible y no se declara un hito que no se ha alcanzado.

## 3. Rutas del juego

El juego original usa rutas como `d:\Frontend.str`, `device:\...` o rutas
relativas a la raíz del disco. `VirtualFileSystem` monta:

| Dispositivo | Destino |
| --- | --- |
| `d`, `device`, `game` | `--game-root` |
| `save` | `<game-root>/<paths.save_dir>` |
| `cache` | `<game-root>/<paths.cache_dir>` |
| rutas absolutas (`\foo`) y dispositivos desconocidos | `--game-root` |

`ResolveExisting` corrige mayúsculas componente a componente, necesario en
Linux (y útil en Windows con datos copiados desde otros sistemas). Cualquier
`..` que intente salir de la raíz devuelve `nullopt`: el runtime debe tratar eso
como un fallo de carga, no como una ruta válida.

## 4. Hooks y triaje

`HookRegistry` distingue dos casos:

1. **Hook confirmado:** dirección verificada en `game/re/addresses.md` y
   registrada en `game/src/hooks/hooks.cpp`.
2. **Llamada no resuelta:** el runtime llama a una dirección que no tiene
   función recompilada ni stub. Se cuenta, se registra una vez y se escribe en
   `hook-report.md`.

El log usa el formato que entiende `tools/triage.py`:

```text
[hooks] llamada no resuelta 0x82A71810
```

Flujo:

```bash
./build/bin/SimpsonsRecomp --game-root game/extracted \
  --trace-out game/re/logs/boot-trace.md 2> game/re/logs/crash.log
python tools/triage.py game/re/logs/crash.log --write
python tools/codegen.py --sdk tools/rexglue-sdk
```

No se registran las direcciones de ejemplo de `game/re/addresses.md`: un stub
con una dirección equivocada oculta el fallo real.

## 5. Activar el runtime real de ReXGlue

El único punto que falta es `CreateRexGlueRuntimeBackend()`. Pasos:

1. Descarga el SDK en `tools/rexglue-sdk/` (ver [BUILDING.md](../BUILDING.md)).
2. Extrae tu ISO en `game/extracted/` y ejecuta el codegen:

   ```bash
   python tools/codegen.py --verify-only
   python tools/codegen.py --sdk tools/rexglue-sdk
   ```

3. Copia la plantilla de integración a `game/src/app/runtime_rexglue.cpp` y
   rellena las llamadas del SDK:
   - inicialización del runtime (memoria, hilos, XAM, sistema de archivos);
   - resolución del entrypoint recompilado;
   - bucle de presentación y entrega de códigos de entrada.
4. Compila con la integración activada:

```bash
cmake --preset windows-full -DSIMPSONS_HAVE_REXGLUE_API=ON
cmake --build --preset windows-full
```

La interfaz que debe implementar la clase está en `game/src/app/runtime.h`.
`Run()` recibe `FrameCallbacks` con `poll_input`, `draw_ui` y `present`; el
backend puede llamarlos desde su propio bucle o ignorarlos si ReXGlue ya posee
el hilo de presentación.

Si la integración falla al crearse, la fábrica vuelve al backend nulo y lo
registra; el port no se queda en un estado silencioso.

## 6. Criterios de salida medibles

La traza (`--trace-out`) permite comprobar la fase sin ambigüedad:

| Criterio | Cómo se comprueba |
| --- | --- |
| El proceso arranca y presenta frames | `Entrypoint`…`Input` alcanzadas; `frames presentados > 0` |
| El runtime real está enlazado | `[runtime] backend = rexglue` y no aparece el aviso de backend nulo |
| El codegen no está vacío | `tools/codegen.py` termina sin error y hay `.cpp` en `game/generated/` |

El backend nulo **nunca** marca `Frontend`; por tanto un build sin XEX no puede
pasar la fase 3 por accidente.

## 7. Pruebas

`tests/test_main.cpp` cubre el núcleo sin SDK:

```bash
cmake --preset windows-dev
cmake --build --preset windows-dev
ctest --test-dir build/windows-dev --output-on-failure
```

- Configuración: secciones, comentarios, cadenas, booleanos, errores.
- VFS: montajes, rutas absolutas y relativas, `..`, mayúsculas.
- Boot trace: orden, omisiones, informe Markdown.
- Hooks: registro, stubs, no resueltos, informe.

## 8. Tareas que siguen dependiendo del volcado

Estas casillas del [ROADMAP.md](../ROADMAP.md) siguen abiertas a propósito:

- [ ] Generar el código con ReXGlue.
- [ ] Enlazar el runtime y llegar a `main` recompilado.
- [ ] Resolver las primeras llamadas no registradas con `tools/triage.py`.
- [ ] Conseguir el primer clear/present real.

Cada una necesita ejecutar el binario del usuario; el repositorio no incluye
`default.xex`, paquetes ni código generado.

## 9. Seguimiento

| Issue | Contenido |
| --- | --- |
| [#2](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/2) | Codegen + primer enlace de ReXGlue |
| [#5](https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp/issues/5) | Activar hooks confirmados desde `addresses.md` |
