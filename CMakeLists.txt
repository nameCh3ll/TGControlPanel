cmake_minimum_required(VERSION 3.26)

project(TGControlPanel VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SRC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/sources")

set(LIB_DIR "${PROJECT_SOURCE_DIR}/libs/x64${CMAKE_SYSTEM_NAME}")
set(LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/libraries/${CMAKE_SYSTEM_NAME}")

option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)

if(WIN32)
  message(STATUS "Building for Windows")

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")
  set(SYSTEM_LIB
      wavpack advapi32 gdi32 vorbisfile vorbis ogg mingw32
      pthread crypto setupapi imm32 mpg123 winmm
      version ws2_32 crypt32 user32 kernel32
  )
elseif(UNIX)
  message(STATUS "Building for Linux")

  set(SYSTEM_LIB pthread)
else()
  message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}.")

endif()

file(GLOB LIB_FILES "${LIB_DIR}/*.a")
include_directories(${LIB_INCLUDE_DIR} ${SRC_INCLUDE_DIR})

file(GLOB SOURCE_FILES "${SRC_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_FILES} ${SYSTEM_LIB})
target_precompile_headers(${PROJECT_NAME} PRIVATE "${SRC_INCLUDE_DIR}/pch.h")
