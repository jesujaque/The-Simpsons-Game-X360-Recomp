#pragma once

namespace simpsons {

/// Diagnóstico de entorno y de la instalación del juego.
/// No requiere el SDK ni el código generado: sirve para validar la cadena de
/// herramientas antes de empezar el trabajo pesado.
int RunDoctor(int argc, char** argv);

} // namespace simpsons
