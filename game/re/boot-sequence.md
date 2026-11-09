# Secuencia de arranque

Documento vivo. Recoge el orden real de inicialización observado en Xenia y confirmado en Ghidra.
Cada fila debe tener evidencia; si no la hay, marca la confianza como **hipótesis**.

## Línea base

| Campo | Valor |
| --- | --- |
| Fecha | |
| Región / Title ID | USA / `45410809` |
| Xenia | Canary, build |
| Backend | Vulkan / D3D12 |
| Resolución interna | 1x |
| Tiempo hasta primer frame | |
| Tiempo hasta menú | |
| Tiempo hasta primer nivel | |
| Save usado | ninguno / completo / parcial |
| Notas | |

## Etapas

| # | Etapa | Función / dirección | Evidencia | Confianza | Estado |
| --- | --- | --- | --- | --- | --- |
| 1 | Entrypoint XEX | | loader | | ⬜ |
| 2 | CRT / constructores | | | | ⬜ |
| 3 | Heap y memoria | | | | ⬜ |
| 4 | Sistema de archivos | | | | ⬜ |
| 5 | Init RenderWare | | | | ⬜ |
| 6 | Init audio | | | | ⬜ |
| 7 | Init input | | | | ⬜ |
| 8 | Frontend / menú | | | | ⬜ |
| 9 | Gameflow / Lua | | | | ⬜ |
| 10 | Carga de nivel | | | | ⬜ |

## 1. Entrypoint

- **Dirección:**
- **Firma:**
- **Qué hace:**
- **Evidencia:**
- **Riesgo para el port:**

## 2. CRT y constructores

- **Tabla de inicialización:**
- **Funciones llamadas:**
- **Orden observado:**
- **Notas:**

## 3. Memoria

- **Heap inicial:**
- **Llamadas a `NtAllocateVirtualMemory`:**
- **Alineación / tamaño:**
- **Notas:**

## 4. Sistema de archivos

- **Rutas montadas:**
- **Funciones de apertura:**
- **Paquetes que se abren al arrancar:**
- **Comportamiento si falta un archivo:**

## 5. Render

- **Init de RenderWare:**
- **Creación de device:**
- **Primer clear/present:**
- **Shaders o estados conocidos:**
- **Notas:**

## 6. Audio

- **Init del mixer:**
- **Primer sonido:**
- **Problemas en Xenia:**

## 7. Input

- **Init:**
- **Primera lectura:**
- **Mando/teclado:**

## 8. Frontend

- **Carga de `Frontend.str`:**
- **Estado inicial de UI:**
- **Transiciones:**
- **Notas:**

## 9. Gameflow y Lua

- **Script de entrada:**
- **Funciones registradas:**
- **Episodio por defecto:**
- **Notas:**

## 10. Primer nivel

- **Episodio:**
- **Assets que carga:**
- **Punto de entrada del gameplay:**
- **Problemas observados:**

## Diferencias con el port

| Etapa | Xenia | Port | Estado |
| --- | --- | --- | --- |
| | | | |

## Pendiente

- [ ] Completar etapas 1–4.
- [ ] Confirmar init de render.
- [ ] Añadir direcciones a `addresses.md`.
- [ ] Convertir candidatos en entradas de `functions_patch.toml`.
