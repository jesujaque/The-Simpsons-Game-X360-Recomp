## Resumen

<!-- Qué cambia y por qué. Enlaza el issue si existe. -->

## Tipo de cambio

- [ ] Bug fix
- [ ] Nueva funcionalidad
- [ ] Documentación
- [ ] Herramientas / build / CI
- [ ] Investigación (direcciones, formatos)

## Fase del roadmap

Fase: <!-- 0, 1, 2, 3, 4, 5 o 6 --> — enlace a la tarea de [ROADMAP](../docs/ROADMAP.md) y al
`README.md` de `docs/NNN-titulo-fase/` si aplica.

## Cómo se ha probado

- Sistema operativo:
- Región del volcado:
- Comando de build:
- Resultado:

## Checklist

- [ ] No incluyo assets, binarios ni código generado del juego.
- [ ] El PR apunta a `develop` (salvo hotfix documentado a `main`).
- [ ] `cmake --preset linux-dev && cmake --build --preset linux-dev` sigue funcionando.
- [ ] Si hay investigación nueva, `game/re/` está actualizado con evidencia.
- [ ] Si cambia el flujo o el build, `docs/` está actualizado.
- [ ] La documentación de fase vive en `docs/NNN-titulo-fase/` y `python tools/check_docs.py` pasa.
