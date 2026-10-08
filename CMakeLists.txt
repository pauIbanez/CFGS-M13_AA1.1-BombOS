cmake_minimum_required(VERSION 3.25)
project(BombOS VERSION 0.1.0 LANGUAGES CXX)

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


add_executable(BombOS ${SOURCES}  ./release/icon.rc)

set_target_properties(BombOS PROPERTIES
  WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
  LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup"
  LINK_FLAGS_RELWITHDEBINFO "/ENTRY:mainCRTStartup"
)

file(GLOB_RECURSE BombOS_ALL_HEADERS "${CMAKE_SOURCE_DIR}/src/*.h")
set(BombOS_INCLUDE_DIRS "")

foreach(_header ${BombOS_ALL_HEADERS})
  get_filename_component(_dir "${_header}" DIRECTORY)
  list(APPEND BombOS_INCLUDE_DIRS "${_dir}")
endforeach()

list(REMOVE_DUPLICATES BombOS_INCLUDE_DIRS)

target_include_directories(BombOS PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  ${BombOS_INCLUDE_DIRS}
)


install(TARGETS BombOS
  RUNTIME DESTINATION .
)

install(DIRECTORY "${CMAKE_BINARY_DIR}/"
  DESTINATION .
  FILES_MATCHING
  PATTERN "*.dll"
  PATTERN "CMakeFiles" EXCLUDE
  PATTERN "default" EXCLUDE
)

install(DIRECTORY "${CMAKE_BINARY_DIR}/resources"
  DESTINATION .
)
