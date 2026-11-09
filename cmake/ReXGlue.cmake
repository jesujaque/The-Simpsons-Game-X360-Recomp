# Integración con el SDK de ReXGlue.
#
# La release del SDK incluye:
#   tools/rexglue-sdk/
#   ├── bin/rexglue[.exe]
#   ├── include/...
#   └── lib/rexglue_runtime[.lib|.a]
#
# Ajusta los nombres si tu release usa otro layout. Este archivo solo crea un
# target INTERFACE; el ejecutable final se declara en game/CMakeLists.txt.

set(REXGLUE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tools/rexglue-sdk"
    CACHE PATH "Ruta al SDK de ReXGlue")

if(NOT EXISTS "${REXGLUE_SDK_DIR}")
    message(FATAL_ERROR
        "No se encontró el SDK de ReXGlue en '${REXGLUE_SDK_DIR}'.\n"
        "Descarga una release de https://github.com/rexglue/rexglue-sdk/releases "
        "y descomprímela ahí, o pasa -DREXGLUE_SDK_DIR=<ruta>.")
endif()

add_library(rexglue_runtime INTERFACE)
add_library(rexglue::runtime ALIAS rexglue_runtime)

target_include_directories(rexglue_runtime INTERFACE
    "${REXGLUE_SDK_DIR}/include")

target_link_directories(rexglue_runtime INTERFACE
    "${REXGLUE_SDK_DIR}/lib")

# TODO: confirmar el nombre exacto de la librería en la release que uses.
target_link_libraries(rexglue_runtime INTERFACE
    $<$<PLATFORM_ID:Windows>:rexglue_runtime>
    $<$<PLATFORM_ID:Linux>:rexglue_runtime>)

if(MSVC)
    target_compile_options(rexglue_runtime INTERFACE /EHsc)
else()
    target_compile_options(rexglue_runtime INTERFACE -fno-exceptions)
endif()

message(STATUS "ReXGlue SDK: ${REXGLUE_SDK_DIR}")
