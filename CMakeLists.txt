cmake_minimum_required(VERSION 3.25)

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

file(GLOB_RECURSE APP_ALL_HEADERS "${CMAKE_SOURCE_DIR}/src/*.h")
set(APP_INCLUDE_DIRS "")

foreach(_header ${APP_ALL_HEADERS})
  get_filename_component(_dir "${_header}" DIRECTORY)
  list(APPEND APP_INCLUDE_DIRS "${_dir}")
endforeach()

list(REMOVE_DUPLICATES APP_INCLUDE_DIRS)

target_include_directories(${APP_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/include"
  "${APP_GENERATED_DIR}"
  ${APP_INCLUDE_DIRS}
)


# ---- External libraries (templates, uncomment and adapt) ----
#
# Pick ONE option per library. Every option ends with target_link_libraries(),
# which also adds the library's include dirs and compile flags to the app.
#
# Suggested layout for libraries you drop into the repo:
#   external/<lib>/include/   headers
#   external/<lib>/lib/       .lib files (static libs, or import libs for DLLs)
#   external/<lib>/bin/       .dll files
#
# -- A) Installed package (vcpkg, system install, a lib's own CMake config) --
# find_package(fmt CONFIG REQUIRED)
# target_link_libraries(${APP_NAME} PRIVATE fmt::fmt)
#
# -- B) Download and build from source (CMake-based libs, usually static) --
# include(FetchContent)
# FetchContent_Declare(fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG        11.0.2              # always pin a tag or commit
# )
# FetchContent_MakeAvailable(fmt)
# target_link_libraries(${APP_NAME} PRIVATE fmt::fmt)
#
# -- C) Prebuilt STATIC library (.lib, code gets baked into the .exe) --
# add_library(mylib STATIC IMPORTED)
# set_target_properties(mylib PROPERTIES
#   IMPORTED_LOCATION             "${CMAKE_SOURCE_DIR}/external/mylib/lib/mylib.lib"
#   IMPORTED_LOCATION_DEBUG       "${CMAKE_SOURCE_DIR}/external/mylib/lib/mylibd.lib"  # optional debug build
#   INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/mylib/include"
# )
# target_link_libraries(${APP_NAME} PRIVATE mylib)
#
# -- D) Prebuilt DYNAMIC library (.dll + import .lib, .dll must ship next to the .exe) --
# add_library(mydll SHARED IMPORTED)
# set_target_properties(mydll PROPERTIES
#   IMPORTED_LOCATION             "${CMAKE_SOURCE_DIR}/external/mydll/bin/mydll.dll"  # loaded at runtime
#   IMPORTED_IMPLIB               "${CMAKE_SOURCE_DIR}/external/mydll/lib/mydll.lib"  # linked at build time
#   INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/mydll/include"
# )
# target_link_libraries(${APP_NAME} PRIVATE mydll)
#
# # Copy the DLL next to the exe after each build (so dev/run works)...
# add_custom_command(TARGET ${APP_NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:mydll> $<TARGET_FILE_DIR:${APP_NAME}>
# )
# # ...and ship it with the release
# install(IMPORTED_RUNTIME_ARTIFACTS mydll RUNTIME DESTINATION ${APP_NAME})
#
# -- E) Windows system library (comes with the OS/SDK, nothing to ship) --
# target_link_libraries(${APP_NAME} PRIVATE user32 gdi32)
# -------------------------------------------------------------


# Data folders: symlinked next to the exe in dev, copied on install
file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

foreach(_dir IN ITEMS assets resources)
  string(TOUPPER "${_dir}" _dir_upper)
  set(_src "${CMAKE_SOURCE_DIR}/${_dir}")
  set(_dst "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_dir}")

  if(NOT APP_USE_${_dir_upper})
    continue()
  endif()
  if(NOT IS_DIRECTORY "${_src}")
    message(FATAL_ERROR "APP_USE_${_dir_upper} is ON but ${_src} does not exist")
  endif()

  if(DEV_PRESET)
    # Replace stale copies; keep an existing symlink as is
    if(EXISTS "${_dst}" AND NOT IS_SYMLINK "${_dst}")
      file(REMOVE_RECURSE "${_dst}")
    endif()
    if(NOT IS_SYMLINK "${_dst}")
      file(CREATE_LINK "${_src}" "${_dst}" RESULT _link_result SYMBOLIC)
      if(NOT _link_result EQUAL 0)
        # Symlinks need Developer Mode on Windows; fall back to a copy (refreshed every configure)
        message(STATUS "Could not symlink ${_dir}/ (${_link_result}), copying instead")
        file(COPY "${_src}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
      endif()
    endif()
  endif()

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
