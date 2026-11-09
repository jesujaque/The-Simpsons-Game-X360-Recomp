# Documentación

Índice de la documentación del proyecto y convención para organizarla.

## Documentos generales

| Documento | Contenido |
| --- | --- |
| [ROADMAP.md](ROADMAP.md) | Fases, tareas, entregables y criterios de salida |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Capas, pipeline y decisiones de diseño |
| [BUILDING.md](BUILDING.md) | Entorno, extracción, codegen y compilación |
| [RESEARCH.md](RESEARCH.md) | Índice de investigación y hallazgos |
| [RELEASING.md](RELEASING.md) | Ramas, tags y publicación |
| [LEGAL.md](LEGAL.md) | Alcance legal y uso de datos del juego |

## Documentación por fase

Cada fase del roadmap tiene su propia carpeta numerada. El nombre sigue el
formato `NNN-titulo-en-minusculas`:

| Fase | Carpeta | Documentos |
| --- | --- | --- |
| 0 | [000-andamiaje](000-andamiaje/README.md) | Andamiaje del repositorio |
| 1 | [001-analisis-y-linea-base](001-analisis-y-linea-base/README.md) | Runbook de análisis y línea base |
| 2 | [002-primer-enlace](002-primer-enlace/README.md) | Codegen, runtime y primer enlace |
| 3 | [003-menu-principal](003-menu-principal/README.md) | Assets, frontend, entrada y audio |
| 4 | [004-primer-nivel-jugable](004-primer-nivel-jugable/README.md) | Primer nivel jugable |
| 5 | [005-estabilidad](005-estabilidad/README.md) | Estabilidad y campaña completa |
| 6 | [006-mejoras-de-port](006-mejoras-de-port/README.md) | Mejoras de port a largo plazo |

## Convención para nuevas fases

- Los documentos operativos de una fase **deben** vivir en
  `docs/NNN-titulo-fase/`, no en la raíz de `docs/`.
- Usa tres dígitos (`000`–`999`) y un título corto en minúsculas separado por
  guiones. La numeración coincide con la fase del [ROADMAP.md](ROADMAP.md).
- Cada carpeta debe tener un `README.md` que sirva de índice: estado, alcance,
  runbook(s), tareas abiertas y criterio de salida.
- Los documentos transversales (`ARCHITECTURE`, `BUILDING`, `LEGAL`,
  `RELEASING`, `RESEARCH`, `ROADMAP`) se quedan en `docs/`.
- Al mover o renombrar un documento, actualiza todos los enlaces relativos y
  ejecuta `python tools/check_docs.py`.
- `docs/ROADMAP.md` sigue siendo la fuente de verdad del alcance; la carpeta de
  fase es el detalle operativo.
