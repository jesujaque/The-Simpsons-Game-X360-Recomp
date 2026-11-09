# Seguridad

## Alcance

Este proyecto recompila código de un juego de 2007 y lo ejecuta de forma nativa. Los riesgos
principales no vienen del juego en sí, sino de:

- Ejecutar binarios o scripts descargados de terceros.
- Instalar "parches" o mods de fuentes no confiables.
- Usar ISOs de dudosa procedencia.

## Recomendaciones

1. Descarga releases solo desde la página oficial del repositorio.
2. Verifica los hashes publicados en cada release.
3. No ejecutes `cmd.txt`, scripts Lua ni paquetes de contenido de desconocidos.
4. Mantén actualizados los drivers gráficos.

## Reportar una vulnerabilidad

No abras un issue público. Usa *Security → Report a vulnerability* en GitHub con:

- Versión afectada y sistema operativo.
- Pasos para reproducir.
- Impacto estimado.
- Prueba de concepto, si es posible.

Responderemos en un plazo orientativo de 72 horas.
