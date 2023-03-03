cmake_minimum_required(VERSION 3.0)
project(pitrex_studio)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# Linux -pthread shenanigans
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

#=== LIBRARY: cimgui + Dear ImGui
add_library(cimgui STATIC 
    cimgui/cimgui.cpp 
    cimgui/cimgui.h 
    cimgui/imgui/imgui.cpp
    cimgui/imgui/imgui.h
    cimgui/imgui/imgui_widgets.cpp 
    cimgui/imgui/imgui_draw.cpp 
    cimgui/imgui/imgui_demo.cpp)
target_include_directories(cimgui INTERFACE cimgui)

#=== LIBRARY: sokol
# add headers to the the file list because they are useful to have in IDEs
set(SOKOL_HEADERS
    sokol/sokol_gfx.h
    sokol/sokol_app.h
    sokol/sokol_time.h
    sokol/sokol_imgui.h
    sokol/sokol_glue.h)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_library(sokol STATIC sokol/sokol.m ${SOKOL_HEADERS})
    target_link_libraries(sokol PUBLIC
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework MetalKit"
        "-framework Metal")
else()
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL dl m)
        target_link_libraries(sokol PUBLIC Threads::Threads)
    endif()
endif()
target_link_libraries(sokol PUBLIC cimgui)
target_include_directories(sokol INTERFACE sokol)

#=== EXECUTABLE: pitrex_studio
set(APP_HEADERS
    pitrex_studio_app.hpp
    pitrex_project.hpp)

set(BASIC_HEADERS
    rbbasic/basic/rb_bas_arch.h
    rbbasic/basic/rb_bas_arch.h
    rbbasic/basic/rb_bas_array.h
    rbbasic/basic/rb_bas_callback.h
    rbbasic/basic/rb_bas_dictionary.h
    rbbasic/basic/rb_bas_error.h
    rbbasic/basic/rb_bas_hexdump.h
    rbbasic/basic/rb_bas_internal.h
    rbbasic/basic/rb_bas_interpreter.h
    rbbasic/basic/rb_bas_io.h
    rbbasic/basic/rb_bas_kbhit.h
    rbbasic/basic/rb_bas_lines.h
    rbbasic/basic/rb_bas_math.h
    rbbasic/basic/rb_bas_sound.h
    rbbasic/basic/rb_bas_tokenizer.h
    rbbasic/basic/rb_bas_variables.h
    rbbasic/basic/rb_bas_vgraphic.h
    rbbasic/basic/rb_bas_textbuffer.h
    rbbasic/basic/rb_bas_platform.h)

set(BASIC_SOURCES
    rbbasic/basic/rb_bas_arch.c
    rbbasic/basic/rb_bas_array.c
    rbbasic/basic/rb_bas_dictionary.c
    rbbasic/basic/rb_bas_error.c
    rbbasic/basic/rb_bas_hexdump.c
    rbbasic/basic/rb_bas_interpreter.c
    rbbasic/basic/rb_bas_io.c
    rbbasic/basic/rb_bas_kbhit.c
    rbbasic/basic/rb_bas_lines.c
    rbbasic/basic/rb_bas_math.c
    rbbasic/basic/rb_bas_sound.c
    rbbasic/basic/rb_bas_tokenizer.c
    rbbasic/basic/rb_bas_variables.c
    rbbasic/basic/rb_bas_vgraphic.c
    rbbasic/basic/rb_bas_textbuffer.c
    rbbasic/basic/rb_bas_platform.c)

set(BASE_HEADERS
    rbbase/RBBase.hpp rbbase/base64.h rbbase/http.hh rbbase/socket.hh rbbase/RBColor.hpp rbbase/RBLogger.hpp rbbase/RBMatrix.hpp rbbase/RBTimer.hpp rbbase/RBVector2.hpp rbbase/RBVector3.hpp
    texteditor/TextEditor.h)

set(BASE_SOURCES
    rb_app.c rbbase/base64.cpp rbbase/http.cc rbbase/socket.cc rbbase/RBColor.cpp rbbase/RBLogger.cpp rbbase/RBMatrix.cpp rbbase/RBTimer.cpp rbbase/RBVector2.cpp rbbase/RBVector3.cpp
    texteditor/TextEditor.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(pitrex_studio WIN32  
    pitrex_studio_app.cpp
    pitrex_project.cpp
    ${APP_HEADERS}
    ${BASIC_SOURCES} ${BASIC_HEADERS}
    ${BASE_SOURCES} ${BASE_HEADERS})
else()
    add_executable(pitrex_studio  
    pitrex_studio_app.cpp
    pitrex_project.cpp
    ${APP_HEADERS}
    ${BASIC_SOURCES} ${BASIC_HEADERS}
    ${BASE_SOURCES} ${BASE_HEADERS})
endif()

target_link_libraries(pitrex_studio sokol)

source_group("Base Sources" FILES ${BASE_SOURCES})
source_group("Base Headers" FILES ${BASE_HEADERS})
source_group("BASIC Sources" FILES ${BASIC_SOURCES})
source_group("BASIC Headers" FILES ${BASIC_HEADERS})

# Emscripten-specific linker options
if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    # use our own minimal shell.html
    target_link_options(pitrex_studio PRIVATE --shell-file ../sokol/shell.html)
    # WASM+JS size optimizations
    target_link_options(pitrex_studio PRIVATE -sNO_FILESYSTEM=1 -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1)
endif()

# explicitly strip dead code
if (CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    target_link_options(pitrex_studio PRIVATE LINKER:-dead_strip)
endif()

# this hack removes the xxx-CMakeForceLinker.cxx dummy file
set_target_properties(pitrex_studio PROPERTIES LINKER_LANGUAGE C)
