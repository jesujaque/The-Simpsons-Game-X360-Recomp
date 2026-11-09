# Núcleo compartido: no depende de ReXGlue ni del XEX del usuario. Lo usan el
# doctor, el ejecutable completo y las pruebas.
add_library(SimpsonsCore STATIC
    src/app/boot_trace.cpp
    src/app/config.cpp
    src/assets/asset_catalog.cpp
    src/hooks/hook_registry.cpp
    src/input/keymap.cpp
    src/platform/paths.cpp
    src/platform/vfs.cpp
)

target_include_directories(SimpsonsCore PUBLIC src)
target_compile_features(SimpsonsCore PUBLIC cxx_std_20)

find_package(Threads REQUIRED)
target_link_libraries(SimpsonsCore PUBLIC Threads::Threads)

add_executable(SimpsonsDoctor
    src/doctor_main.cpp
    src/app/doctor.cpp
)
target_link_libraries(SimpsonsDoctor PRIVATE SimpsonsCore)

target_include_directories(SimpsonsDoctor PRIVATE src)
set_target_properties(SimpsonsDoctor PROPERTIES OUTPUT_NAME "SimpsonsDoctor")

# El ejecutable completo solo se construye cuando existen el SDK y el código
# generado a partir del XEX del usuario.
if(SIMPSONS_FULL_BUILD)
    file(GLOB_RECURSE SIMPSONS_GENERATED_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/generated/*.cpp")

    if(NOT SIMPSONS_GENERATED_SOURCES)
        message(FATAL_ERROR
            "No hay código generado en game/generated/.\n"
            "Ejecuta: python tools/codegen.py --sdk tools/rexglue-sdk")
    endif()

    add_executable(SimpsonsRecomp
        src/main.cpp
        src/app/application.cpp
        src/app/runtime.cpp
        src/hooks/hooks.cpp
        src/patches/patches.cpp
        src/renderer/renderer.cpp
        src/audio/audio.cpp
        src/input/input.cpp
        src/ui/overlay.cpp
        ${SIMPSONS_GENERATED_SOURCES}
    )

    target_include_directories(SimpsonsRecomp PRIVATE src generated)
    target_link_libraries(SimpsonsRecomp PRIVATE SimpsonsCore rexglue::runtime)
    set_target_properties(SimpsonsRecomp PROPERTIES
        OUTPUT_NAME "SimpsonsRecomp"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    if(WIN32)
        target_compile_definitions(SimpsonsRecomp PRIVATE SIMPSONS_PLATFORM_WINDOWS=1)
    else()
        target_compile_definitions(SimpsonsRecomp PRIVATE SIMPSONS_PLATFORM_LINUX=1)
    endif()

    # Integración real opcional. Cuando exista runtime_rexglue.cpp, activa:
    #   -DSIMPSONS_HAVE_REXGLUE_API=ON
    if(SIMPSONS_HAVE_REXGLUE_API)
        target_compile_definitions(SimpsonsRecomp PRIVATE SIMPSONS_HAVE_REXGLUE_API=1)
        target_sources(SimpsonsRecomp PRIVATE src/app/runtime_rexglue.cpp)
    endif()
endif()
