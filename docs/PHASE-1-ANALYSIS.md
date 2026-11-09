# Fase 1 — Análisis y línea base

> **Objetivo:** entender el `default.xex`, mapear la cadena de arranque y dejar una base de
> conocimiento suficiente para que la fase 2 (codegen + enlace) no sea a ciegas.
>
> **Duración orientativa:** 3–5 días de trabajo efectivo.
> **Requisito de entrada:** copia legal del juego y entorno de [BUILDING.md](BUILDING.md).

---

## 1. Criterio de salida

La fase 1 termina cuando se cumplen **todos** estos puntos:

- [ ] `game/re/dump-report.md` generado y revisado.
- [ ] `game/re/boot-sequence.md` describe el arranque desde el entrypoint hasta el menú principal.
- [ ] `game/re/addresses.md` contiene al menos **25 funciones** con evidencia y nivel de confianza.
- [ ] La lista de imports del XEX está documentada (XAM, kernel, XInput, XAudio, etc.).
- [ ] Los candidatos a `game/functions_patch.toml` están identificados y justificados.
- [ ] Existe un log de referencia de Xenia con tiempos, errores y comportamiento esperado.
- [ ] Un segundo investigador puede repetir la extracción y el análisis con esta guía.

No hace falta haber identificado *todo* el juego. Hace falta que el camino crítico de arranque esté
mapeado y que el resto tenga un plan.

## 2. Entregables

| Artefacto | Ubicación | ¿Se versiona? |
| --- | --- | --- |
| Informe del volcado | `game/re/dump-report.md` | Sí (sin rutas personales) |
| Secuencia de arranque | `game/re/boot-sequence.md` | Sí |
| Direcciones y firmas | `game/re/addresses.md` | Sí |
| Formato de paquetes | `game/re/formats.md` | Sí |
| Candidatos de funciones | `game/functions_patch.toml` | Sí |
| Log de Xenia | `game/re/logs/` | **No** (ignorado) |
| Proyecto de Ghidra | `game/re/ghidra/` | **No** (ignorado) |
| ISO / XEX / datos extraídos | `game/extracted/` | **No** (ignorado) |

Regla: se documentan **direcciones, firmas y conclusiones**, nunca volcados de código ni assets.

## 3. Herramientas

| Herramienta | Versión recomendada | Para qué |
| --- | --- | --- |
| Python | 3.10+ | `tools/*.py` |
| 7-Zip | reciente | Descomprimir releases y dumps |
| `extract-xiso` | última | Extraer la ISO |
| Ghidra | 11.x / 12.x | Desensamblado y decompilado |
| JDK | 21 (el que pida Ghidra) | Requisito de Ghidra y extensiones |
| XEXLoaderWV | fork SaveEditors si usas Ghidra 12.0.4+ | Cargar XEX en Ghidra |
| `XenonAnalyse` | build de XenonRecomp | Fronteras de funciones y jump tables |
| Xenia Canary | última | Línea base de ejecución y logs |
| ReXGlue SDK | 0.8.x | Fase 2 (déjalo ya descargado) |
| Ghidra / VS Code | — | Editar notas y TOML |

### Instalación en Windows

```powershell
winget install --id Python.Python.3.12 -e
winget install --id 7zip.7zip -e
winget install --id EclipseAdoptium.Temurin.21.JDK -e
winget install --id Ghidra.Ghidra -e
```

`extract-xiso` no suele estar en winget: descarga el binario de
<https://github.com/XboxDev/extract-xiso/releases> y añade su carpeta al `PATH`.

`XenonAnalyse` se distribuye con las releases de
[XenonRecomp](https://github.com/hedge-dev/XenonRecomp). Copia el ejecutable a `tools/XenonRecomp/`
(ignorado por git).

### Instalación en Linux

```bash
sudo apt install -y openjdk-21-jdk python3 p7zip-full
# Ghidra: descarga el zip oficial y descomprímelo en ~/tools/ghidra
# extract-xiso y XenonAnalyse: compila o descarga releases y añade al PATH
```

### XEXLoaderWV

1. Descarga el zip de <https://github.com/SaveEditors/XEXLoaderWV/releases>.
2. En Ghidra: **File → Install Extensions → +** y selecciona el zip.
3. Reinicia Ghidra.
4. Comprueba que al importar un `.xex` aparece el loader **XEX Loader**.

Si usas una versión antigua de Ghidra, el fork original es
<https://github.com/zeroKilo/XEXLoaderWV>.

## 4. Verificar el volcado

### 4.1 Identificación mínima

| Campo | Valor esperado |
| --- | --- |
| Juego | The Simpsons Game |
| Plataforma | Xbox 360 |
| Región objetivo | USA |
| Title ID | `45410809` |
| Ejecutable | `default.xex` (`simpsons_f.exe`) |
| Motor | RenderWare |
| Lanzamiento | 30/10/2007 |

La versión PAL/EU puede arrancar, pero **no es el objetivo de la fase 1** y sus direcciones pueden
diferir. Si solo tienes PAL, documenta el Title ID real y trabaja con él de forma consistente.

### 4.2 Extraer

```bash
python tools/extract_iso.py "D:/dumps/The Simpsons Game.iso" --out game/extracted
```

El script llama a `extract-xiso -x -d game/extracted <iso>` y falla si no aparece `default.xex`.
No subas la ISO ni la carpeta extraída.

### 4.3 Generar el informe

```bash
python tools/verify_dump.py --root game/extracted --md game/re/dump-report.md
```

El informe incluye:

- tamaño y hashes (MD5/SHA-256) de `default.xex`;
- número de archivos y tamaño total;
- inventario por extensión;
- presencia de `Frontend.str`, scripts Lua y paquetes;
- archivos sospechosos (ISOs dentro de la extracción, duplicados evidentes).

Guarda el hash en un sitio privado si quieres comparar volcados; **no** publiques rutas personales.

## 5. Línea base en Xenia

Antes de tocar Ghidra, graba cómo se comporta el juego original.

1. Abre el ISO o `default.xex` en **Xenia Canary**.
2. Usa backend Vulkan (o D3D12 si tu GPU lo prefiere) y resolución interna 1x.
3. Activa el log a archivo y nivel informativo.
4. Cronometra: arranque → logo EA → intro → menú → carga del primer nivel.
5. Anota sonido, resolución, errores visuales y cuelgues.
6. Guarda el log en `game/re/logs/` (ignorado por git) y resume lo relevante en
   `game/re/boot-sequence.md`.

Preguntas que debes responder:

- ¿Cuánto tarda en aparecer el primer frame?
- ¿Qué pantallas se suceden antes del menú?
- ¿Hay cinemáticas obligatorias? ¿Se pueden saltar?
- ¿Qué pasa si no hay save? ¿Qué pasa con un save existente?
- ¿Qué errores de Xenia aparecen (shader, XMA, contenido)?

## 6. Ghidra

### 6.1 Crear el proyecto

Crea el proyecto **fuera** del repositorio, por ejemplo `~/simpsons-ghidra`, para no versionar
archivos `.gpr`/`.rep` ni el binario importado.

### 6.2 Importar

1. **File → Import File** y selecciona `default.xex`.
2. Formato: **XEX Loader**.
3. Deja la imagen base que proponga el loader; confírmala en el listado de memoria.
4. Acepta el análisis automático.

Comprobaciones:

- El listado debe contener direcciones en torno a `0x82xxxxxx`.
- En `Window → Memory Map` verifica base y permisos (RX para código, RW para datos).
- En el árbol de imports deben aparecer `xam.xex`, `xinput1_3.dll`, `xapilib`/`xaudio` y funciones
  `Nt*`/`Xam*`.

### 6.3 Convenciones de nombres

| Prefijo | Subsistema |
| --- | --- |
| `boot_` | Entrypoint, CRT, constructores |
| `fs_` | Sistema de archivos y paquetes |
| `xam_` | XAM (usuario, contenido, guardado) |
| `rw_` | RenderWare |
| `gpu_` | Xenos / comandos |
| `audio_` | XMA, mixer |
| `input_` | XInput |
| `lua_` | Scripting |
| `game_` | Lógica del juego |

Marca las funciones con **plate comment**:

```
Nombre:
Propósito:
Evidencia: string | import | xref | log | Xenia
Confianza: confirmada | probable | hipótesis
Notas:
```

### 6.4 Búsquedas que ahorran horas

| Busca | Por qué |
| --- | --- |
| `simpsons_gameflow.lua` | Punto de entrada del flujo de episodios |
| `cmd.txt` | Flags de arranque |
| `Frontend.str` | Carga del frontend |
| `.str`, `.pkg`, `.lvl` | Cargadores de paquetes |
| `XamContentCreateEnumerator` | Sistema de contenido/DLC |
| `XamUserGetSigninState` | Perfil y guardado |
| `RwEngineInit`, `RwWorld`, `RwCamera` | RenderWare |
| `XInputGetState` | Input |
| `XMACreateDecoder` / `XMA*` | Audio |
| `D3DDevice` / `Xenos` | Capa gráfica |

Usa **Search → For Strings** y luego `Ctrl+Shift+F` para xrefs. Cada string interesante debe acabar
en `game/re/addresses.md` con la dirección de la función que la referencia.

### 6.5 Imports

Documenta la tabla de imports en una sección de `addresses.md` o en un anexo:

| Módulo | Función | Dirección del stub | Uso esperado |
| --- | --- | --- | --- |
| `xam.xex` | `XamUserGetSigninState` | | Perfil |
| `xam.xex` | `XamContentCreateEnumerator` | | Contenido |
| `xinput1_3.dll` | `XInputGetState` | | Mando |

Esto alimenta directamente los stubs de la fase 2.

## 7. XenonAnalyse

`XenonAnalyse` recorre el XEX y detecta funciones, jump tables y vtables. Es la entrada natural del
codegen.

```bash
# Ajusta el nombre del ejecutable y las flags a tu build:
tools/XenonRecomp/XenonAnalyse.exe game/extracted/default.xex game/re/analysis.toml
```

Notas:

- Ejecuta `XenonAnalyse.exe --help` si tu build usa otros argumentos.
- Salida esperada: TOML/JSON con direcciones de funciones y tablas.
- **No** versiones la salida si contiene datos derivados del binario; resume las direcciones
  relevantes en `addresses.md` y en `functions_patch.toml`.
- Cruza las funciones detectadas con Ghidra: si XenonAnalyse encuentra una función que Ghidra no
  marcó, créala y nómbrala.

Criterio práctico: si una función solo se alcanza por puntero, XenonAnalyse puede verla y Ghidra no;
si XenonAnalyse no la ve, se declara a mano en `functions_patch.toml`.

## 8. Mapa de subsistemas

Rellena esta tabla en `boot-sequence.md` a medida que avances. No hace falta completarla entera en
la fase 1, pero sí el camino hasta el menú.

| # | Etapa | Evidencia esperada | Prioridad |
| --- | --- | --- | --- |
| 1 | Entrypoint del XEX | Import/entry del loader | Alta |
| 2 | CRT / constructores globales | Tabla de init, llamadas a `xstart` | Alta |
| 3 | Inicialización de memoria y heap | `NtAllocateVirtualMemory`, tamaño de heap | Alta |
| 4 | Sistema de archivos y montaje | Rutas `d:\`, `device:\`, `CreateFile` | Alta |
| 5 | Init de RenderWare | `RwEngineInit`, device, cámara | Alta |
| 6 | Init de audio | XMA, mixer, canales | Media |
| 7 | Init de input | `XInputGetState` | Media |
| 8 | Frontend / menú | Carga de `Frontend.str`, estado de UI | Alta |
| 9 | Gameflow / Lua | `simpsons_gameflow.lua`, tabla de episodios | Media |
| 10 | Carga del primer nivel | `.lvl`, `.str`, streaming | Media |

Para cada etapa documenta:

- dirección inicial y funciones llamadas;
- strings/imports que la confirman;
- qué espera el runtime de ReXGlue (memoria, FS, XAM);
- qué puede fallar en el port.

## 9. Cómo documentar un hallazgo

### 9.1 Entrada de dirección

```markdown
| `0x82A71810` | `boot_init_callback` | boot | Confirma arranque de constructores | Xref desde tabla en `.data` + log | probable |
```

### 9.2 Nota de función

```markdown
#### `0x82A71810` — `boot_init_callback`

- **Firma hipotética:** `void boot_init_callback()`
- **Llamadores:** tabla de constructores en `0x82...`
- **Llamadas:** `0x82...`, `0x82...`
- **Evidencia:** xref en `.data`, coincide con el crash de arranque de Xenia.
- **Confianza:** probable
- **Notas:** candidata a `functions_patch.toml`.
```

### 9.3 Niveles de confianza

| Nivel | Significado |
| --- | --- |
| **Confirmada** | Verificada en ejecución o por dos fuentes independientes |
| **Probable** | Evidencia fuerte (string + xref + patrón) sin prueba runtime |
| **Hipótesis** | Solo patrón o nombre; pendiente de confirmar |

No mezcles niveles: es mejor una hipótesis honesta que una confirmación falsa.

## 10. Plan de 5 días

| Día | Trabajo | Salida |
| --- | --- | --- |
| 1 | Instalar herramientas, extraer ISO, `verify_dump.py`, Xenia baseline | `dump-report.md`, primeras notas de `boot-sequence.md` |
| 2 | Importar en Ghidra, imports, strings, entrypoint y CRT | Sección de boot + imports |
| 3 | XenonAnalyse, funciones del boot, FS y XAM | `addresses.md` con 10–15 funciones |
| 4 | RenderWare, audio, input y frontend | 25 funciones, mapa de subsistemas |
| 5 | Revisar candidatos, actualizar `functions_patch.toml`, escribir conclusiones | Fase 1 cerrada, PR |

Si un día se atasca, prioriza **entrypoint → FS → render init → frontend**; es el camino que
desbloquea la fase 2.

## 11. Errores comunes

| Problema | Causa | Solución |
| --- | --- | --- |
| Ghidra no reconoce el `.xex` | Falta XEXLoaderWV | Instala la extensión y reinicia |
| Direcciones no cuadran con las notas | Región distinta o base distinta | Fija región y base; anótalas |
| El XEX no carga | Volcado incompleto/cifrado | Reextrae la ISO completa |
| XenonAnalyse no genera salida | Flags incorrectas | Ejecuta `--help` y revisa la versión |
| Xenia va bien pero el port crashea | El runtime no cubre una API | Documenta el import y prepara un stub |
| Pérdida de notas | Proyecto dentro del repo o sin commit | Proyecto fuera; notas en `game/re/` |

## 12. Checklist final

- [ ] Hash y tamaño del `default.xex` registrados.
- [ ] Title ID y región confirmados.
- [ ] Inventario de archivos y extensiones.
- [ ] `Frontend.str` y scripts Lua localizados.
- [ ] Log de Xenia resumido.
- [ ] Entrypoint, CRT y heap identificados.
- [ ] Sistema de archivos y rutas de montaje.
- [ ] Init de RenderWare localizado.
- [ ] Imports de XAM/kernel/input/audio documentados.
- [ ] 25 funciones con evidencia y confianza.
- [ ] Candidatos en `functions_patch.toml`.
- [ ] PR a `develop` con la documentación y CI en verde.

## 13. Siguiente fase

Cuando esta checklist esté cerrada, pasa a la [fase 2 del roadmap](ROADMAP.md): generar el código,
enlazar ReXGlue y resolver las primeras llamadas no registradas.
