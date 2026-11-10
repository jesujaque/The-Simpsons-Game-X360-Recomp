---
name: sdd-constitution
description: >
  Constitución técnica y de proceso del proyecto The-Simpsons-Game-X360-Recomp para el flujo
  SDD. Fija las reglas no negociables de C++20, build con CMake + Ninja, tipos de ancho fijo y
  aislamiento de stubs del kernel de Xbox 360. Aplica antes de redactar spec, plan, tareas o código.
---

# SDD · Constitución

Este skill fija las **reglas no negociables** del proyecto *The-Simpsons-Game-X360-Recomp* para
cualquier cambio del flujo SDD. Debe aplicarse en cada etapa (`sdd-spec`, `sdd-plan`, `sdd-task`,
`sdd-implements`) y antes de proponer o ejecutar cualquier cambio técnico.

## Fuentes de verdad

1. `docs/ROADMAP.md` — fuente de alcance y fases (0–6).
2. `docs/ARCHITECTURE.md` — capas, pipeline de recompilación y decisiones de diseño.
3. `CONTRIBUTING.md` — reglas de oro, ramas, PRs, commits y estilo.
4. `docs/BUILDING.md` — entorno, presets, doctor, extracción y codegen.
5. Runbooks de fase: `docs/001-analisis-y-linea-base/PHASE-1-ANALYSIS.md`,
   `docs/002-primer-enlace/PHASE-2-IMPLEMENTATION.md`, `docs/003-menu-principal/PHASE-3-IMPLEMENTATION.md`.
6. Investigación: `game/re/addresses.md`, `game/re/boot-sequence.md`, `game/re/formats.md`,
   `game/functions_patch.toml`.

## Reglas

### R1 — Lenguaje y estilo C++20

- El proyecto se rige por **C++20**: `CMAKE_CXX_STANDARD 20`, `CMAKE_CXX_STANDARD_REQUIRED ON`,
  `CMAKE_CXX_EXTENSIONS OFF` (`CMakeLists.txt`); `.clang-format` con `Standard: c++20` (LLVM base,
  `IndentWidth: 4`, `ColumnLimit: 110`, `PointerAlignment: Left`, `BreakBeforeBraces: Attach`,
  `AccessModifierOffset: -4`, `SortIncludes: CaseSensitive`, `IncludeBlocks: Regroup`,
  `SpaceAfterCStyleCast: false`).
- Compilación: MSVC `/W4 /permissive- /utf-8` (+`/WX` si `SIMPSONS_WARNINGS_AS_ERRORS`); Clang
  `-Wall -Wextra -Wpedantic` (+`-Werror` si `SIMPSONS_WARNINGS_AS_ERRORS`).
- Sin dependencias nuevas sin discusión previa (`CONTRIBUTING.md`).
- Comentarios y documentación en español; identificadores y constantes en inglés.

### R2 — Build: CMake + Ninja

- `cmake_minimum_required(VERSION 3.24)` y presets (v6) con generator **Ninja**.
- Presets: `windows-dev` / `linux-dev` (doctor, `SIMPSONS_FULL_BUILD=OFF`, `Debug`);
  `windows-full` / `linux-full` (full, `SIMPSONS_FULL_BUILD=ON`, `RelWithDebInfo`,
  `REXGLUE_SDK_DIR=${sourceDir}/tools/rexglue-sdk`).
- Comandos: `cmake --preset <p> && cmake --build --preset <p>`; tests con
  `ctest --test-dir build/<preset> --output-on-failure`.
- El **build doctor** (sin SDK ni codegen) debe seguir funcionando:
  `cmake --preset linux-dev && cmake --build --preset linux-dev` y ejecutar el doctor.
- El full build requiere SDK ReXGlue y `game/generated/` regenerado.

### R3 — Tipos de ancho fijo

- Direcciones, retornos, tamaños y contadores de juego → tipos fijos de `<cstdint>`:
  - Direcciones: `std::uint32_t`, con formato `0x` + hex mayúsculo de 8 dígitos (p. ej. `0x82A71810`).
  - Retornos: `std::uint32_t` (`HookContext::return_value`, stubs).
  - Contadores/frames: `std::uint64_t` (p. ej. `FrameStats::frame_count`).
  - Tamaños/bytes: `std::uintmax_t` (p. ej. `AssetCatalog::total_bytes`), arrays `std::array<std::uint8_t, N>`.
- Prohibido usar `int`, `unsigned int`, `long` o `short` para direcciones, tamaños, retornos o contadores.
- Los headers que modelan direcciones incluyen `<cstdint>`.

### R4 — Aislamiento de stubs del kernel de Xbox 360

- El puerto se comunica con el runtime mediante el contrato `RuntimeBackend`
  (`game/src/app/runtime.h`); el backend nulo (`NullRuntimeBackend`) **no finge hitos**.
- La integración real se activa con `SIMPSONS_HAVE_REXGLUE_API` y la fábrica
  `CreateRexGlueRuntimeBackend()` (`game/src/app/runtime_rexglue.cpp`); si falla, se usa el backend
  nulo y se registra.
- Hooks y stubs en `HookRegistry`:
  - Hook: `Register(address, name, HookFn, note)`.
  - Stub: `RegisterStub(address, name, return_value, note)`: devuelve `return_value` y marca
    `context.handled = true`.
- Llamadas no resueltas: `Dispatch` las acumula y `python tools/triage.py <log> --write` las
  traslada a `game/functions_patch.toml`.
- Solo se registran direcciones **confirmadas** en `game/re/addresses.md`; nunca direcciones de ejemplo.
- Naming: hooks `Hook_<FuncionOriginal>` con comentario de por qué; stubs con nombre descriptivo;
  prefijos `boot_`, `fs_`, `xam_`, `rw_`, `gpu_`, `audio_`, `input_`, `lua_`, `game_`.

### R5 — Proceso, documentación y calidad

- Ramas de trabajo desde `develop`; PR a `develop`; squash merge; Conventional Commits.
- Documentación por fase en `docs/NNN-titulo-en-minusculas/` con `README.md` índice.
- Definition of Done general: PR con CI verde, docs actualizadas, sin assets/binarios del juego,
  build doctor funcional.

## Comandos de verificación

```bash
python -m compileall tools
python tools/check_docs.py
python tools/codegen.py --verify-only

cmake --preset linux-dev && cmake --build --preset linux-dev
ctest --test-dir build/linux-dev --output-on-failure
./build/linux-dev/bin/SimpsonsDoctor --help
```

## Definition of Done de la constitución

- [ ] C++20 y estilo aplicados.
- [ ] Build doctor pasa en Linux y Windows.
- [ ] Tipos de ancho fijo usados para direcciones, tamaños y contadores.
- [ ] Stubs aislados, con triage y solo direcciones confirmadas.
