# Releases

## Modelo de ramas

```
feat/*  fix/*  docs/*
     \    |    /
      develop  ── CI ──► PRs (destino por defecto)
          │
          │ merge cuando hay hito publicable
          ▼
        main  ── tag vX.Y.Z ──► workflow Release
```

- **`develop`** es la rama de integración. Todo PR va aquí.
- **`main`** solo contiene versiones publicables. Recibe merges desde `develop`.
- Los tags se crean **siempre desde `main`**, nunca desde `develop` ni desde una feature branch.

## Hitos

| Versión | Criterio de salida |
| --- | --- |
| `v0.1.0` | El ejecutable arranca y llega al menú principal. |
| `v0.2.0` | Primer episodio jugable de principio a fin. |
| `v1.0.0` | Campaña completa, guardado estable y cierre limpio. |

Una release solo se etiqueta cuando el criterio se cumple en `develop` y se ha probado en Windows
y Linux (o al menos en la plataforma principal).

## Preparar una release

1. Actualiza la versión en `CMakeLists.txt` (`project(... VERSION x.y.z)`).
2. Mueve las entradas de `CHANGELOG.md` de `[No publicado]` a `[x.y.z] - AAAA-MM-DD`.
3. Abre un PR `develop` → `main` con el título `release: vX.Y.Z`.
4. Espera CI en verde y haz merge (sin squash, para conservar el commit de merge).
5. Crea y sube el tag:

```bash
git checkout main
git pull origin main
git tag -a v0.1.0 -m "The Simpsons Game Recompiled v0.1.0"
git push origin v0.1.0
```

6. El workflow `Release` valida que el tag coincide con `CMakeLists.txt`, que el commit está en
   `main`, empaqueta las herramientas y crea la release (borrador si se lanza manualmente).
7. Adjunta el ejecutable del juego si corresponde (ver abajo) y publica la release.

## Ejecutable del juego

El código recompilado (`game/generated/`) **no se versiona** y no se puede generar en CI alojada
porque requiere el `default.xex` del usuario. Por eso el workflow solo publica herramientas y
documentación. El binario completo se empaqueta en una máquina que tenga:

- el SDK de ReXGlue,
- los datos extraídos,
- el codegen ejecutado.

```bash
cmake --preset windows-full
cmake --build --preset windows-full
python tools/package_release.py --version 0.2.0 \
  --exe build/windows-full/bin/SimpsonsRecomp.exe
gh release upload v0.2.0 dist/SimpsonsRecomp-0.2.0-full-windows-x64.zip
```

> Comprueba antes de adjuntarlo que el paquete no contiene assets ni datos extraídos. El script
> solo incluye el `.exe` y documentación.

## Contenido de cada paquete

| Archivo | Incluido |
| --- | --- |
| Ejecutable (`SimpsonsRecomp` o `SimpsonsDoctor`) | Sí |
| `README.md`, `LICENSE`, `CHANGELOG.md` | Sí |
| `config.example.toml` | Sí |
| `docs/BUILDING.md`, `docs/LEGAL.md`, `docs/RELEASING.md` | Sí |
| `default.xex`, `.str`, `.pkg`, audio, vídeo, texturas | **No** |
| `game/generated/` | **No** |

## Checklist antes de publicar

- [ ] `CMakeLists.txt` y el tag coinciden.
- [ ] CI en verde en `main`.
- [ ] `CHANGELOG.md` actualizado.
- [ ] Probado en Windows y Linux.
- [ ] El paquete no contiene datos del juego.
- [ ] Notas de release revisadas (no dejar una release en borrador para siempre).
- [ ] `main` y `develop` sincronizadas después del merge.

## Hotfix

Si una release publicada tiene un fallo crítico:

1. Crea `fix/*` desde `main`.
2. PR a `main`, merge y tag `vX.Y.Z+1`.
3. Merge de `main` de vuelta a `develop` para no perder el arreglo.
