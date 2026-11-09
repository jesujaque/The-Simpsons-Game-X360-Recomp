# The Simpsons Game — Xbox 360 Recompiled

Recompilación estática de **The Simpsons Game** (Xbox 360, 2007) para ejecutarlo de forma nativa en PC
(Windows y Linux), sin emulación de CPU. El código PowerPC del ejecutable original se traduce una sola
vez a C++ y se compila para x86-64 con [ReXGlue](https://github.com/rexglue/rexglue-sdk), el SDK de
recompilación derivado de Xenia.

> **Estado: andamiaje inicial (scaffold).** El repositorio contiene la estructura del proyecto, las
> herramientas de preparación y el manifiesto del recompilador. El código generado y la integración
> con el runtime todavía no están incluidos.

> **Aviso legal:** este proyecto **no incluye ni distribuirá** ningún contenido del juego (texturas,
> modelos, audio, vídeo, scripts, niveles ni el ejecutable original). Para usarlo necesitas tu propia
> copia legal del juego para Xbox 360. No está afiliado ni respaldado por Electronic Arts, Disney,
> Fox o Microsoft. Consulta [docs/LEGAL.md](docs/LEGAL.md).

---

## ¿Qué es una recompilación estática?

La Xbox 360 usa una CPU PowerPC (Xenon). Un emulador como Xenia traduce las instrucciones en tiempo
de ejecución (JIT). Una recompilación estática hace esa traducción **una sola vez, antes de ejecutar**:
cada función del ejecutable se convierte en código C++ nativo que se compila con Clang/MSVC.

El resultado es un ejecutable de PC normal:

- Sin JIT ni intérprete en el hilo de juego.
- Depurable con las herramientas estándar (Visual Studio, RenderDoc, PIX).
- Base ideal para mejoras de port nativo: 60 FPS, ultrawide, mods, accesibilidad, etc.

La GPU (Xenos) se sigue gestionando mediante la capa gráfica del runtime (Vulkan / D3D12), que
traduce los comandos y shaders del juego. Sustituirla por un renderer nativo es trabajo posterior.

## Características objetivo

| Área | Objetivo |
| --- | --- |
| CPU | Recompilación AOT completa del `default.xex` |
| Render | Vulkan (Linux/Windows) y D3D12 (Windows) vía ReXGlue; escalado de resolución |
| Audio | XMA/decodificación nativa y mezcla del runtime |
| Entrada | Mando XInput/GameSir + teclado y ratón remapeables |
| Presentación | 16:9 y ultrawide, 60/120/unlimited FPS con lógica a velocidad original |
| Calidad de vida | Launcher, guardado compatible, skipping de logos, selector de episodios |
| Plataformas | Windows 10/11 x86-64, Linux x86-64 (Steam Deck) |

## Requisitos

### Para jugar (cuando exista una release)

- Tu propia copia de **The Simpsons Game** para Xbox 360 (ISO o volcado GOD/XEX).
  La versión USA (Title ID `45410809`) es el objetivo principal.
- Unos **5 GB** libres para los datos extraídos.
- CPU x86-64 con AVX2 (Haswell / Excavator / Zen o posterior).
- GPU con Vulkan 1.2+ o D3D12.
- Mando recomendado.

### Para desarrollar

- Windows 10/11 o Linux (probado en Ubuntu 24.04).
- Visual Studio 2022 (C++ Desktop) o Clang 18+.
- CMake ≥ 3.24 y Ninja.
- Python ≥ 3.10.
- [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk/releases) (v0.8.0 o superior).
- Ghidra + [XenonAnalyse](https://github.com/hedge-dev/XenonAnalyse) para análisis del binario.
- Xenia Canary como referencia de comportamiento.
- Git con LFS **no** necesario (no se versionan assets).

## Inicio rápido

```bash
# 1. Clona el repositorio
git clone https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp.git
cd The-Simpsons-Game-X360-Recomp

# 2. Comprueba tu entorno y tu copia del juego
python tools/bootstrap.py --check

# 3. Extrae la ISO (requiere extract-xiso en el PATH)
python tools/extract_iso.py "D:/dumps/The Simpsons Game.iso" --out game/extracted

# 4. Genera el proyecto y el código recompilado
python tools/codegen.py --sdk tools/rexglue-sdk

# 5. Compila en modo completo (requiere el SDK y el código generado)
cmake --preset windows-full
cmake --build --preset windows-full
```

Mientras el código generado no exista, se puede compilar el modo *doctor* (sin dependencias):

```bash
cmake --preset windows-dev
cmake --build --preset windows-dev
./build/dev/SimpsonsDoctor.exe
```

Consulta [docs/BUILDING.md](docs/BUILDING.md) para el detalle paso a paso y
[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) para entender cómo encajan las piezas.

## Estructura del repositorio

```
.
├── .github/                 CI, plantillas de issues y PR
├── assets/                  Recursos propios del port (no assets del juego)
├── cmake/                   Integración con ReXGlue
├── docs/                    Arquitectura, build, investigación, roadmap y legal
├── game/                    Proyecto del juego
│   ├── simpsons_manifest.toml   Manifiesto de entrada para ReXGlue
│   ├── functions_patch.toml     Fronteras de funciones detectadas a mano
│   ├── generated/               C++ generado por el recompilador (ignorado por git)
│   ├── re/                      Notas de ingeniería inversa (direcciones, formatos)
│   └── src/                     Código escrito a mano: hooks, parches, render, audio, UI
├── tools/                   Scripts de extracción, triaje y generación
├── CMakeLists.txt           Build raíz (modo doctor y modo completo)
├── CMakePresets.json        Presets de desarrollo y release
├── config.example.toml      Configuración de ejecución de ejemplo
└── README.md
```

## Hoja de ruta

| Fase | Hito | Estado |
| --- | --- | --- |
| 0 | Scaffold, CI, manifiesto y herramientas | ✅ |
| 1 | Extracción de la ISO y análisis del `default.xex` | ⬜ |
| 2 | Codegen completo y enlazado con ReXGlue | ⬜ |
| 3 | Arranque hasta el logo / menú principal | ⬜ |
| 4 | Primer nivel jugable (Homer, "The Simpsons Game") | ⬜ |
| 5 | Audio, guardado y configuración estables | ⬜ |
| 6 | 60 FPS, ultrawide y mejoras de PC | ⬜ |

Detalle y criterios de aceptación en [docs/ROADMAP.md](docs/ROADMAP.md).

## Prior art y créditos

- [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk) — runtime de recompilación (kernel, audio,
  input, gráficos) construido sobre el trabajo de Xenia.
- [XenonRecomp](https://github.com/hedge-dev/XenonRecomp) y
  [XenosRecomp](https://github.com/hedge-dev/XenosRecomp) — recompiladores pioneros.
- [Xenia](https://github.com/xenia-project/xenia) — emulador de referencia e infraestructura.
- [Unleashed Recompiled](https://github.com/hedge-dev/UnleashedRecomp) — referencia de calidad de port.
- [TheSimpsonsGameRecomp](https://github.com/YesterMester/TheSimpsonsGameRecomp) — recompilación
  previa del mismo juego; referencia técnica y de investigación.

Este proyecto es independiente y no reutiliza assets ni código propietario de esos proyectos más
allá de las licencias que correspondan.

## Contribuir

Lee [CONTRIBUTING.md](CONTRIBUTING.md). Las contribuciones más útiles al principio son:

1. Nombres y firmas de funciones en `game/re/`.
2. Entradas en `game/functions_patch.toml` para llamadas indirectas no detectadas.
3. Pruebas de arranque con distintos volcados (USA/EU) y registro de errores.
4. Mejoras de accesibilidad y de launcher.

## Licencia

El código de este repositorio se publica bajo **MIT** (ver [LICENSE](LICENSE)). Las marcas,
personajes y contenido del juego pertenecen a sus respectivos titulares.
