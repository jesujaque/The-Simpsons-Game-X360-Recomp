# Arquitectura

## Visión general

```
┌──────────────────────────────────────────────────────────────────┐
│                      SimpsonsRecomp.exe                          │
│                                                                  │
│  ┌──────────────┐   ┌────────────────┐   ┌───────────────────┐  │
│  │  Launcher /  │   │  Código del    │   │  Runtime ReXGlue  │  │
│  │  Overlay UI  │◄──┤  juego (AOT)   │──►│  kernel / XAM     │  │
│  └──────────────┘   │  generated/    │   │  memoria / hilos  │  │
│                     └───────┬────────┘   └─────────┬─────────┘  │
│                             │                      │            │
│                     ┌───────▼────────┐   ┌─────────▼─────────┐  │
│                     │ Hooks/parches  │   │ GPU  │ Audio │ Input│
│                     │ src/hooks      │   │ Vulkan/D3D12 │ SDL │
│                     └────────────────┘   └───────────────────┘  │
└──────────────────────────────────────────────────────────────────┘
              ▲                                        ▲
       assets extraídos                       configuración TOML
       (no versionados)                       (config.toml)
```

## Pipeline de recompilación

1. **Extracción** — `tools/extract_iso.py` obtiene `default.xex` y los paquetes de datos del ISO.
   Los datos van a `game/extracted/` (ignorado por git).
2. **Análisis** — ReXGlue analiza el XEX: secciones, tablas de saltos, vtables RTTI y llamadas
   directas. Las funciones alcanzadas solo por punteros se declaran a mano en
   `game/functions_patch.toml`.
3. **Codegen** — ReXGlue convierte cada instrucción PowerPC en C++ y genera un registro de funciones
   (`generated/`). Este árbol **no se versiona**: se regenera a partir del XEX del usuario.
4. **Enlace** — CMake compila el código generado y lo enlaza con el runtime ReXGlue y con nuestro
   código de `game/src/`.
5. **Ejecución** — El runtime inicializa memoria, hilos, sistema de archivos y XAM; llama al
   entrypoint recompilado y desde ahí todo es código nativo.

## Capas

| Capa | Ubicación | Responsabilidad |
| --- | --- | --- |
| Plataforma | `game/src/platform` | Rutas, detección de SO, directorios de datos/guardado |
| VFS | `game/src/platform/vfs.*` | Traducción de rutas Xbox (`d:\`, `save:\`) al host |
| Configuración | `game/src/app/config.*` | Lectura y validación de `config.toml` |
| Arranque | `game/src/app/application.*`, `boot_trace.*` | Ciclo de vida, etapas y criterios de salida |
| Runtime | `game/src/app/runtime.*` | Contrato con ReXGlue y backend nulo de desarrollo |
| Assets | `game/src/assets` | Inventario, `Frontend.str` y sondeo de contenedores |
| Hooks | `game/src/hooks` | Sustituciones, stubs y llamadas no resueltas |
| Parches | `game/src/patches` | Cambios de comportamiento (FPS, aspect ratio, fixes) |
| Render | `game/src/renderer` | Ajustes sobre la capa gráfica de ReXGlue |
| Audio | `game/src/audio` | Mezcla, volúmenes, fixes de streaming |
| Input | `game/src/input` | Mapeo mando/teclado, deadzones, rebinding |
| UI | `game/src/ui` | Overlay, menús propios, launcher |
| Investigación | `game/re` | Direcciones, firmas, formatos, notas de Ghidra |

## Módulos del juego relevantes

The Simpsons Game (360) usa **RenderWare** como motor gráfico y un ejecutable `simpsons_f.exe`
(Title ID `45410809`, región USA). Puntos de interés para la investigación:

- **Gameflow en Lua**: `simpsons_gameflow.lua`, `simpsons_gameflow_helpers.lua` y
  `simpsons_scores.lua` definen episodios, costumes y puntuaciones.
- **`cmd.txt`** en la raíz del disco: flags de arranque separados por espacios.
- **Sistema de contenido**: handlers de `.lvl`, `.costume` y `.lua`; DLC cancelado pero con código
  residual.
- **Guardado**: comprobación de integridad y paquetes de contenido dependientes.

Estas notas se amplían en `game/re/` a medida que se avanza.

## Decisiones de diseño

- **ReXGlue frente a XenonRecomp puro.** ReXGlue aporta kernel, XAM, audio, input y backend gráfico;
  XenonRecomp solo genera código y obliga a escribir el runtime. Para llegar antes a un build
  jugable, ReXGlue es la ruta corta.
- **`generated/` fuera de git.** Evita distribuir código derivado del binario propietario y mantiene
  el repositorio pequeño y legalmente limpio.
- **Sin assets en el repo.** Todo se instala desde la copia del usuario.
- **Hooks antes que reescritura.** Primero se hace funcionar el juego tal cual; las mejoras de port
  se aplican después, una a una y con comparación visual.

## Flujo de trabajo recomendado

```
XEX ──► codegen ──► build ──► run ──► crash / función no registrada
  ▲                                        │
  └──── functions_patch.toml ◄── triaje ◄──┘
```

Antes del codegen, la fase 1 produce el mapa del binario:
[PHASE-1-ANALYSIS.md](001-analisis-y-linea-base/PHASE-1-ANALYSIS.md).

La integración de las fases 2–3 se apoya en tres contratos:

1. `RuntimeBackend` (`game/src/app/runtime.h`): inicializa el runtime y ejecuta
   el entrypoint. Sin SDK se usa un backend nulo que no finge hitos.
2. `VirtualFileSystem` (`game/src/platform/vfs.h`): convierte rutas del juego a
   rutas del host y corrige mayúsculas en Linux.
3. `BootTrace` (`game/src/app/boot_trace.h`): registra hasta dónde llega el
   arranque y genera `game/re/logs/boot-trace.md`.

Detalle operativo en [PHASE-2-IMPLEMENTATION.md](002-primer-enlace/PHASE-2-IMPLEMENTATION.md) y
[PHASE-3-IMPLEMENTATION.md](003-menu-principal/PHASE-3-IMPLEMENTATION.md).

Cada vez que el runtime se detenga por una llamada no resuelta:

1. Anota la dirección en `game/re/addresses.md`.
2. Añádela a `game/functions_patch.toml`.
3. Regenera y vuelve a compilar.
4. Si el fallo es de comportamiento (no de código), implementa un hook en `game/src/hooks`.
