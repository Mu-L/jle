
# This cmake file is intended to be included in the game CMakeLists.txt
# See the template project's CMakeLists.txt.

target_include_directories(${JLE_GAME_BUILD} SYSTEM PUBLIC)

option(BUILD_EDITOR "Build the game in the editor" ON)
option(BUILD_EMSCRIPTEN "Build with Emscripten targeting WebAssembly" OFF)
option(BUILD_OPENGLES30 "Use OpenGL ES 3.0 instead of desktop core 3.3" ON)

# Mac OS doesn't support GLES 3.0
if (APPLE)
    set(BUILD_OPENGLES30 OFF)
endif ()

if (BUILD_EMSCRIPTEN)
    set(BUILD_EDITOR OFF)
    set(BUILD_OPENGLES30 ON)
endif ()

add_compile_definitions(_JLE_ENGINE_PATH="${JLE_ENGINE_PATH}/")
add_compile_definitions(_GAME_RESOURCES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/GameResources")

if (BUILD_EDITOR)
    add_definitions(-DBUILD_EDITOR)
endif ()

if (BUILD_OPENGLES30)
    add_definitions(-DBUILD_OPENGLES30)
endif ()

if (BUILD_EMSCRIPTEN)
    # TODO: Remove having to enable exceptions flag '-fexceptions' since not all browser engines supports exceptions?
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -o index.html -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --shell-file ${JLE_ENGINE_PATH}/shell_fullscreen.html -s EXPORTED_RUNTIME_METHODS=cwrap,allocate -s EXPORTED_FUNCTIONS=_main,_resize_canvas_js --preload-file EditorResources --preload-file GameResources --preload-file EngineResources")
endif ()


add_subdirectory("${JLE_ENGINE_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/${JLE_GAME_BUILD}")
target_link_libraries(${JLE_GAME_BUILD} LINK_PUBLIC engine)

if (BUILD_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(${JLE_GAME_BUILD} PROPERTIES OUTPUT_NAME "index")
endif ()


if (BUILD_EMSCRIPTEN)
    # When building with Emscripten, it needs to find the pre-loaded assets
    # at the same location as the 'binary'
    FILE(COPY GameResources DESTINATION ${PROJECT_BINARY_DIR})
else ()
    FILE(COPY GameResources DESTINATION ${JLE_GAME_BUILD})
endif ()