# Contribuir

Gracias por querer ayudar. Este proyecto es técnico y experimental; antes de abrir un PR, lee esto.

## Antes de empezar

- Comprueba que tu copia del juego es legal y que no vas a subir ningún asset ni binario.
- Busca issues existentes y el [roadmap](docs/ROADMAP.md).
- Para cambios grandes, abre un issue de propuesta primero.

## Reglas de oro

1. **Ningún cambio directo a `develop` o `main`.** Cada cambio, por pequeño que sea (código, docs,
   CI, configuración), va en una rama y entra por pull request.
2. **Nunca hagas commit de datos del juego** (`default.xex`, `.str`, `.pkg`, `.lvl`, audio, vídeo,
   texturas). El `.gitignore` cubre la mayoría, pero revísalo antes de subir.
3. **No subas `game/generated/`**: se regenera con el XEX de cada usuario.
4. **Un PR = un cambio lógico.** Nada de reformateos masivos mezclados con fixes. Si el diff supera
   ~400 líneas, plantéate partirlo.
5. **Documenta las direcciones** que descubras en `game/re/addresses.md` con evidencia y confianza.
6. **No rompas el build doctor**: `cmake --preset linux-dev && cmake --build --preset linux-dev`
   debe seguir funcionando sin SDK ni codegen.

## Documentación por fase

La documentación operativa se organiza por fase del roadmap, no toda suelta en
`docs/`:

- Cada fase vive en `docs/NNN-titulo-fase/`, con tres dígitos y un título corto
  en minúsculas separado por guiones. La numeración coincide con
  [docs/ROADMAP.md](docs/ROADMAP.md): `000-andamiaje`, `001-analisis-y-linea-base`,
  `002-primer-enlace`, etc.
- Cada carpeta debe tener un `README.md` como índice: estado, alcance,
  runbook(s), tareas abiertas, criterio de salida y enlaces.
- Los documentos transversales (`ARCHITECTURE`, `BUILDING`, `LEGAL`,
  `RELEASING`, `RESEARCH`, `ROADMAP`) siguen en la raíz de `docs/`.
- Al crear una fase nueva, crea su carpeta y su `README.md` en el mismo PR; no
  dejes documentos de fase en la raíz de `docs/`.
- Al mover o renombrar documentos, actualiza los enlaces relativos y ejecuta
  `python tools/check_docs.py`.
- El índice y la convención completa están en [docs/README.md](docs/README.md).

## Nombres de rama

| Prefijo | Uso | Ejemplo |
| --- | --- | --- |
| `feat/` | Funcionalidad | `feat/widescreen-hook` |
| `fix/` | Corrección | `fix/audio-xma-crash` |
| `docs/` | Documentación | `docs/phase-1-runbook` |
| `ci/` | CI y build | `ci/release-workflow` |
| `refactor/` | Reestructuración sin cambio funcional | `refactor/hooks-registry` |
| `research/` | Notas de ingeniería inversa | `research/boot-sequence` |

Usa minúsculas, guiones y un nombre corto. Una rama por PR; bórrala al mergear.

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

La rama de integración es **`develop`**. Todos los PR van ahí; `main` solo recibe merges de release.

```bash
# 1. Sincroniza develop
git checkout develop
git pull origin develop

# 2. Crea la rama del cambio
git checkout -b feat/mi-cambio

# 3. Trabaja y prueba
python -m compileall tools
cmake --preset linux-dev && cmake --build --preset linux-dev

# 4. Commit y push
git add -A
git commit -m "feat(render): add widescreen hook"
git push -u origin feat/mi-cambio

# 5. Abre el PR contra develop
gh pr create --base develop --fill
```

### Qué debe incluir el PR

- Título con Conventional Commits.
- Descripción: qué cambia, por qué y cómo se ha probado.
- Fase del roadmap afectada (`Fase 0–6`) y enlace al `README.md` de su carpeta
  en `docs/NNN-titulo-fase/` si el cambio toca esa fase.
- Enlaces a issues relacionados.
- Checklist del template completada.

### Revisión y merge

1. CI en verde (los 3 checks obligatorios).
2. Revisión del diff: sin datos del juego, sin secretos, sin archivos generados.
3. **Squash merge** a `develop` y borrar la rama.
4. Si el cambio cierra una tarea del roadmap, marca la casilla en el PR y actualiza `docs/ROADMAP.md`
   en el mismo PR o en uno inmediatamente posterior.

Los hotfix urgentes también parten de `develop`; solo se abre un PR directo a `main` si hay que
parchear una release ya publicada, y después se hace merge de vuelta a `develop`.

Al mergear una feature en `develop`, usa **squash**. Al mergear `develop` en `main` para una
release, usa **merge commit** (sin squash) para conservar el historial del hito. El proceso de
release está en [docs/RELEASING.md](docs/RELEASING.md).

### Protección de ramas

`develop` y `main` requieren pull request y CI en verde (`Python tools`, `Build doctor (ubuntu-latest)`,
`Build doctor (windows-latest)`). No se permite force-push ni borrar las ramas. El bypass de
administrador existe solo para emergencias; úsalo de forma excepcional y documentándolo en el commit.

## Commits

Usamos [Conventional Commits](https://www.conventionalcommits.org/):

- `feat:`, `fix:`, `docs:`, `build:`, `ci:`, `refactor:`, `test:`, `chore:`
- Ámbito opcional: `feat(audio): ...`

## Código de conducta

Sé respetuoso. No se toleran ataques personales, acoso ni discusión sobre piratería. Ver
[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
