# Pruebas mínimas: no requieren SDK ni datos del juego.
add_test(NAME doctor_help COMMAND SimpsonsDoctor --help)
add_test(NAME doctor_non_strict COMMAND SimpsonsDoctor)
