# Pruebas mínimas: no requieren SDK ni datos del juego.
add_test(NAME doctor_help COMMAND SimpsonsDoctor --help)
add_test(NAME doctor_non_strict COMMAND SimpsonsDoctor)

add_executable(SimpsonsCoreTests test_main.cpp)
target_link_libraries(SimpsonsCoreTests PRIVATE SimpsonsCore)
add_test(NAME core COMMAND SimpsonsCoreTests)
