add_executable(SimpsonsDoctor
    src/doctor_main.cpp
    src/app/doctor.cpp
    src/platform/paths.cpp
)

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
        src/platform/paths.cpp
        src/hooks/hooks.cpp
        src/patches/patches.cpp
        src/renderer/renderer.cpp
        src/audio/audio.cpp
        src/input/input.cpp
        src/ui/overlay.cpp
        ${SIMPSONS_GENERATED_SOURCES}
    )

    target_include_directories(SimpsonsRecomp PRIVATE src generated)
    target_link_libraries(SimpsonsRecomp PRIVATE rexglue::runtime)
    set_target_properties(SimpsonsRecomp PROPERTIES
        OUTPUT_NAME "SimpsonsRecomp"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    if(WIN32)
        target_compile_definitions(SimpsonsRecomp PRIVATE SIMPSONS_PLATFORM_WINDOWS=1)
    else()
        target_compile_definitions(SimpsonsRecomp PRIVATE SIMPSONS_PLATFORM_LINUX=1)
    endif()
endif()
