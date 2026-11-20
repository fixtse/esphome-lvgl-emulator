cmake_minimum_required(VERSION 3.20)
project(esphome_lvgl_emulator C CXX)
# Run ESP32/ESPHome LVGL UI code on a PC: LVGL on the host, ESPHome and
# ESP-IDF stand-ins, PNG or SDL output. Use as a top-level project (builds the
# example) or from another project via add_subdirectory()/FetchContent, then:
#
#   add_lvgl_emulator_app(my-ui SOURCES app.cpp INCLUDE_DIRS my/stubs path/to/firmware)

set(LVGL_EMU_LVGL_VERSION "v9.6.0" CACHE STRING "LVGL git tag (match your ESPHome build)")
set(LVGL_EMU_LV_CONF "${CMAKE_CURRENT_SOURCE_DIR}/config/lv_conf.h" CACHE FILEPATH "lv_conf.h to build LVGL with")
option(LVGL_EMU_SDL "Build the SDL window (needs SDL2)" ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

FetchContent_Declare(lvgl GIT_REPOSITORY https://github.com/lvgl/lvgl.git GIT_TAG ${LVGL_EMU_LVGL_VERSION} GIT_SHALLOW TRUE)
FetchContent_Declare(arduinojson GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git GIT_TAG v7.4.2 GIT_SHALLOW TRUE)
foreach(dep lvgl arduinojson)
  FetchContent_GetProperties(${dep})
  if(NOT ${dep}_POPULATED)
    FetchContent_Populate(${dep})
  endif()
endforeach()

find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)
if(LVGL_EMU_SDL)
  pkg_check_modules(SDL2 sdl2)
endif()

file(GLOB_RECURSE LVGL_SOURCES ${lvgl_SOURCE_DIR}/src/*.c)
add_library(lvgl_emu_lvgl STATIC ${LVGL_SOURCES})
target_include_directories(lvgl_emu_lvgl PUBLIC ${lvgl_SOURCE_DIR} ${FREETYPE_INCLUDE_DIRS})
target_compile_definitions(lvgl_emu_lvgl PUBLIC "LV_CONF_PATH=\"${LVGL_EMU_LV_CONF}\"" LV_KCONFIG_IGNORE)
target_link_libraries(lvgl_emu_lvgl PUBLIC ${FREETYPE_LIBRARIES})
if(SDL2_FOUND)
  target_compile_definitions(lvgl_emu_lvgl PUBLIC LVGL_EMU_USE_SDL=1)
  target_include_directories(lvgl_emu_lvgl PUBLIC ${SDL2_INCLUDE_DIRS})
  target_link_libraries(lvgl_emu_lvgl PUBLIC ${SDL2_LIBRARIES})
endif()

# The runtime (main, display, fonts, images, PNG) and the stand-in headers.
add_library(lvgl_emu_runtime STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime.cpp)
target_include_directories(lvgl_emu_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/stubs
  ${arduinojson_SOURCE_DIR}/src)
# ESPHome defines these for an ESP32 build with LVGL.
target_compile_definitions(lvgl_emu_runtime PUBLIC USE_ESP32=1 USE_LVGL=1 USE_PSRAM=1 USE_HOST_EMULATOR=1)
target_compile_options(lvgl_emu_runtime PUBLIC -Wno-deprecated-declarations)
target_link_libraries(lvgl_emu_runtime PUBLIC lvgl_emu_lvgl m)

function(add_lvgl_emulator_app name)
  cmake_parse_arguments(APP "" "" "SOURCES;INCLUDE_DIRS;DEFINITIONS" ${ARGN})
  add_executable(${name} ${APP_SOURCES})
  # App include directories come first so a project can override a stand-in.
  target_include_directories(${name} BEFORE PRIVATE ${APP_INCLUDE_DIRS})
  target_compile_definitions(${name} PRIVATE ${APP_DEFINITIONS})
  target_link_libraries(${name} PRIVATE lvgl_emu_runtime)
endfunction()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  add_lvgl_emulator_app(hello-panel SOURCES examples/hello/hello.cpp)
endif()
