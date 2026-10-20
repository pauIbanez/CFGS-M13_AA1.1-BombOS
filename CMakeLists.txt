cmake_minimum_required(VERSION 3.31)

# ---- App config (single source of truth) ----
set(APP_NAME    "BombOS")
set(APP_VERSION "0.0.1")
set(APP_ICON    "${CMAKE_SOURCE_DIR}/icon.ico")  # "" = no icon
set(APP_WINDOW_MODE "CONSOLE")  # CONSOLE | WINDOW | AUTO (console in Debug, window otherwise)
set(APP_USE_ASSETS    ON)  # ship assets/ next to the exe
set(APP_USE_RESOURCES OFF)  # ship resources/ next to the exe
# ---------------------------------------------

project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


file(GLOB_RECURSE SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_SOURCE_DIR}/src/*.h"
  "${CMAKE_SOURCE_DIR}/src/*.hpp"
)



add_executable(${APP_NAME} ${SOURCES})

# Console vs window app (AUTO = console in Debug, window otherwise)
if(APP_WINDOW_MODE STREQUAL "WINDOW")
  set(APP_WINDOWED ON)
elseif(APP_WINDOW_MODE STREQUAL "CONSOLE")
  set(APP_WINDOWED OFF)
elseif(APP_WINDOW_MODE STREQUAL "AUTO")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(APP_WINDOWED OFF)
  else()
    set(APP_WINDOWED ON)
  endif()
else()
  message(FATAL_ERROR "APP_WINDOW_MODE must be CONSOLE, WINDOW or AUTO (got '${APP_WINDOW_MODE}')")
endif()

if(WIN32 AND APP_WINDOWED)
  set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE ON)
  # Keep int main() as the entry point instead of requiring WinMain
  if(MSVC)
    target_link_options(${APP_NAME} PRIVATE "/ENTRY:mainCRTStartup")
  endif()
endif()
message(STATUS "${APP_NAME}: ${CMAKE_BUILD_TYPE} build, windowed = ${APP_WINDOWED}")

# Generated files: AppConfig.h (name/version for C++) and app.rc (icon + version info)
set(APP_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
configure_file("${CMAKE_SOURCE_DIR}/cmake/AppConfig.h.in" "${APP_GENERATED_DIR}/AppConfig.h" @ONLY)

if(WIN32)
  if(APP_ICON)
    if(NOT EXISTS "${APP_ICON}")
      message(FATAL_ERROR "APP_ICON is set but ${APP_ICON} does not exist")
    endif()
    set(APP_ICON_LINE "IDI_ICON1 ICON \"${APP_ICON}\"")
  else()
    set(APP_ICON_LINE "")
  endif()
  configure_file("${CMAKE_SOURCE_DIR}/cmake/app.rc.in" "${APP_GENERATED_DIR}/app.rc" @ONLY)
  target_sources(${APP_NAME} PRIVATE "${APP_GENERATED_DIR}/app.rc")
endif()

target_include_directories(${APP_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/include"
  "${APP_GENERATED_DIR}"
)


# External libraries: see cmake/LIBRARIES.md for copy-paste templates


# Data folders: mirrored next to the exe on every build, copied on install
foreach(_dir IN ITEMS assets resources)
  string(TOUPPER "${_dir}" _dir_upper)
  set(_src "${CMAKE_SOURCE_DIR}/${_dir}")

  if(NOT APP_USE_${_dir_upper})
    continue()
  endif()
  if(NOT IS_DIRECTORY "${_src}")
    message(FATAL_ERROR "APP_USE_${_dir_upper} is ON but ${_src} does not exist")
  endif()

  # Custom targets always run, so edits (and deletions) always reach the build
  add_custom_target(copy_${_dir}
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_dir}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${_src}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_dir}"
  )
  add_dependencies(${APP_NAME} copy_${_dir})

  install(DIRECTORY "${_src}" DESTINATION ${APP_NAME})
endforeach()


install(TARGETS ${APP_NAME}
  RUNTIME DESTINATION ${APP_NAME}
)


# `cmake --build --preset <preset> --target run` builds and runs the app
add_custom_target(run
  COMMAND $<TARGET_FILE:${APP_NAME}>
  DEPENDS ${APP_NAME}
  WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  USES_TERMINAL
)
