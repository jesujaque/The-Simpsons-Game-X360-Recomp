# Compilación

## 1. Preparar el entorno

### Windows

```powershell
winget install --id Kitware.CMake -e
winget install --id Ninja-build.Ninja -e
winget install --id Python.Python.3.12 -e
winget install --id Git.Git -e
# Visual Studio 2022 con la carga de trabajo "Desarrollo para el escritorio con C++"
```

### Linux (Ubuntu 24.04)

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build python3 git clang lld
```

## 2. Obtener el SDK de ReXGlue

Descarga la release más reciente y colócala en `tools/rexglue-sdk/`:

```bash
gh release download --repo rexglue/rexglue-sdk --pattern "*windows*" --dir /tmp/rexglue
# descomprime en tools/rexglue-sdk
```

El SDK debe contener el ejecutable `rexglue` y las librerías del runtime. Comprueba:

```bash
./tools/rexglue-sdk/rexglue.exe --version
```

## 3. Extraer el juego

Necesitas una ISO de tu copia de The Simpsons Game (Xbox 360).

```bash
python tools/extract_iso.py "D:/dumps/simpsons.iso" --out game/extracted
```

Resultado esperado:

```
game/extracted/
├── default.xex
├── Frontend.str
├── *.str / *.pkg / *.lvl ...
└── ...
```

Nada de esto se sube al repositorio.

### Herramientas de análisis (fase 1)

Además del entorno de compilación necesitarás:

- **Ghidra** + **XEXLoaderWV** (fork SaveEditors) + JDK 21.
- **XenonAnalyse**, incluido en las releases de XenonRecomp.
- **Xenia Canary** para la línea base.
- `python tools/verify_dump.py --root game/extracted --md game/re/dump-report.md`.

El proceso completo está en [PHASE-1-ANALYSIS.md](001-analisis-y-linea-base/PHASE-1-ANALYSIS.md).

## 4. Generar el código

```bash
# Comprueba el manifiesto y que exista game/extracted/default.xex (no requiere SDK).
python tools/codegen.py --verify-only

python tools/codegen.py --sdk tools/rexglue-sdk
```

El script ejecuta `rexglue codegen` con `game/simpsons_manifest.toml` y escribe en
`game/generated/default/`. La primera pasada puede tardar entre 10 y 40 minutos.
Si el SDK resuelve las rutas de entrada de otra forma, añade
`--skip-input-check`; si la ejecución no produce ningún `.cpp`, el script
termina con error para evitar dar por bueno un codegen vacío.

Si aparecen funciones no resueltas, añádelas a `game/functions_patch.toml` y repite.

## 5. Compilar

### Modo doctor (sin SDK ni codegen)

Sirve para validar la cadena de herramientas y comprobar rutas:

```bash
cmake --preset windows-dev
cmake --build --preset windows-dev
./build/dev/SimpsonsDoctor.exe
```

### Modo completo

```bash
cmake --preset windows-full
cmake --build --preset windows-full
```

Linux:

```bash
cmake --preset linux-full
cmake --build --preset linux-full
```

## 6. Ejecutar

```bash
./build/full/SimpsonsRecomp.exe --game-root game/extracted --config config.toml
```

Opciones útiles:

| Flag | Descripción |
| --- | --- |
| `--game-root <dir>` | Directorio con los datos instalados |
| `--config <file>` | Configuración TOML |
| `--log-level <n>` | 0 silencioso, 3 trazas completas |
| `--trace-out <file>` | Informe Markdown de la traza de arranque |
| `--headless` | Sin ventana; valida la cadena de arranque |
| `--windowed` | Fuerza ventana |
| `--vulkan` / `--d3d12` | Forzar backend gráfico |
| `--fps <n>` | Límite de FPS de presentación |

El arranque con backend nulo y la activación de ReXGlue se detallan en
[PHASE-2-IMPLEMENTATION.md](002-primer-enlace/PHASE-2-IMPLEMENTATION.md) y
[PHASE-3-IMPLEMENTATION.md](003-menu-principal/PHASE-3-IMPLEMENTATION.md).

## Problemas comunes

| Síntoma | Causa probable | Solución |
| --- | --- | --- |
| `Unresolved call 0x...` | Función solo alcanzable por puntero | Añadir a `functions_patch.toml` |
| Crash al cargar nivel | Asset sin extraer o ruta incorrecta | Revisar `game/extracted` y logs |
| Pantalla negra con audio | Backend gráfico | Probar `--d3d12` / `--vulkan`, actualizar drivers |
| `XMA decode failed` | Falta soporte de códec | Verificar build del SDK y versión |
| Build lento | Codegen sin optimizar | Usar Ninja, `--release`, ccache/sccache |
