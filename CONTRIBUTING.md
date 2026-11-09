# Contribuir

Gracias por querer ayudar. Este proyecto es técnico y experimental; antes de abrir un PR, lee esto.

## Antes de empezar

- Comprueba que tu copia del juego es legal y que no vas a subir ningún asset ni binario.
- Busca issues existentes y el [roadmap](docs/ROADMAP.md).
- Para cambios grandes, abre un issue de propuesta primero.

## Reglas de oro

1. **Nunca hagas commit de datos del juego** (`default.xex`, `.str`, `.pkg`, `.lvl`, audio, vídeo,
   texturas). El `.gitignore` cubre la mayoría, pero revísalo antes de subir.
2. **No subas `game/generated/`**: se regenera con el XEX de cada usuario.
3. **Un PR = un cambio lógico.** Nada de reformateos masivos mezclados con fixes.
4. **Documenta las direcciones** que descubras en `game/re/addresses.md`.
5. **No rompas el build doctor**: `cmake --preset linux-dev && cmake --build --preset linux-dev`
   debe seguir funcionando sin SDK ni codegen.

## Cómo contribuir en cada área

### Investigación (muy útil al principio)

- Añade entradas a `game/functions_patch.toml` con un comentario de cómo la encontraste.
- Documenta firmas y structs en `game/re/`.
- Adjunta capturas de Ghidra o logs de Xenia en el issue, no en el repo.

### Código

- C++20, estilo `.clang-format`, sin dependencias nuevas sin discusión.
- Los hooks deben ser pequeños, con nombre `Hook_<FuncionOriginal>` y un comentario de por qué.
- Los parches de gameplay deben poder desactivarse por configuración.

### Herramientas

- Python 3.10+, solo biblioteca estándar salvo justificación.
- Cada script debe tener `--help`, `--dry-run` cuando aplique y mensajes claros.

## Flujo

```bash
git checkout -b feat/mi-cambio
# programa y prueba
python -m compileall tools
cmake --preset linux-dev && cmake --build --preset linux-dev
git commit -m "feat(render): add widescreen hook"
git push -u origin feat/mi-cambio
gh pr create --fill
```

## Commits

Usamos [Conventional Commits](https://www.conventionalcommits.org/):

- `feat:`, `fix:`, `docs:`, `build:`, `ci:`, `refactor:`, `test:`, `chore:`
- Ámbito opcional: `feat(audio): ...`

## Código de conducta

Sé respetuoso. No se toleran ataques personales, acoso ni discusión sobre piratería. Ver
[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
