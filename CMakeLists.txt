cmake_minimum_required(VERSION 3.24)
project(TheRunner)

set(CMAKE_REQUIRED_LIBRARIES "-L${atomic_lib_dir}" "-latomic")

find_package(PkgConfig REQUIRED)

# set(CMAKE_TOOLCHAIN_FILE /home/henry/emsdk/)
if(EMSCRIPTEN)
    MESSAGE("BUILDING FOR THE WEB")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "public/")
else()
    find_package(X11 REQUIRED)
    find_package(ALSA REQUIRED)

    if(NOT X11_FOUND)
        message("ERROR: x11 not found")
    endif(NOT X11_FOUND)
endif()

find_package(OpenGL REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)
# pkg_search_module(GL REQUIRED gl)

set(IMGUI_PATH ${CMAKE_CURRENT_LIST_DIR}/thirdparty/imgui)
file(GLOB IMGUI_GLOB
        ${IMGUI_PATH}/imgui.h
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imconfig.h
        ${IMGUI_PATH}/imgui_demo.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_internal.h
        ${IMGUI_PATH}/imstb_rectpack.h
        ${IMGUI_PATH}/imstb_textedit.h
        ${IMGUI_PATH}/imstb_truetype.h
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp

        # specific bindings...
        ${IMGUI_PATH}/backends/imgui_impl_glfw.h
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.cpp)

add_library("imgui" STATIC ${IMGUI_GLOB})
target_include_directories("imgui" PUBLIC ${IMGUI_PATH})
target_link_libraries("imgui" PRIVATE glfw)

include_directories(${X11_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
MESSAGE("${CMAKE_CURRENT_LIST_DIR}")
find_package(HaGame2 REQUIRED)

add_executable(TheRunner main.cpp src/constants.h src/game.h src/game.cpp src/scenes/mainMenu.h src/scenes/mainMenu.cpp src/common/actionMap.h src/common/gameState.cpp src/common/persistentSettings.cpp src/common/ui.cpp src/scenes/runtime.h src/scenes/runtime.cpp src/systems/renderer.cpp src/components/resource.h src/components/resource.h  src/common/resources.h src/components/player.h src/components/body.h src/systems/movement.h src/systems/movement.cpp src/common/tiles.h src/components/spriteSheet.h src/systems/ysorter.cpp src/systems/ysorter.h src/common/mapBuilder.h src/common/mapBuilder.cpp src/components/tile.h src/components/tree.h src/components/ysort.h src/components/inventory.h src/components/inventory.cpp)
target_include_directories(TheRunner PUBLIC ${HAGAME2_INCLUDE_DIR})
target_include_directories(TheRunner PUBLIC ${FIND_HAGAME2_PATHS}/thirdparty/asio/asio/include/)
target_link_libraries(TheRunner ${HAGAME2_LIBRARY})
target_link_libraries(TheRunner ${GLFW_LIBRARIES})
target_link_libraries(TheRunner ${OPENGL_LIBRARIES})
target_include_directories(TheRunner PUBLIC ${IMGUI_PATH})
target_link_libraries(TheRunner imgui)
target_link_libraries(TheRunner ${X11_LIBRARIES})
target_link_libraries(TheRunner ${ALSA_LIBRARIES})

# set(CMAKE_TOOLCHAIN_FILE /home/henry/emsdk/)
if(EMSCRIPTEN)
    set_target_properties(TheRunner PROPERTIES LINK_FLAGS "-DCMAKE_EXE_LINKER_FLAGS=-s -sMAX_WEBGL_VERSION=2 -sUSE_GLFW=3 -sTOTAL_MEMORY=512MB --preload-file ./../assets -sSTACK_SIZE=1000000")
endif()