cmake_minimum_required(VERSION 3.26)

project(TGControlPanel VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(STACK_SIZE 16777216)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SRC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
include("${PROJECT_SOURCE_DIR}/build/Release/generators/conan_toolchain.cmake")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBVLC REQUIRED libvlc)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(tgbot CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED)

file(GLOB SOURCE_FILES "${SRC_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(CMAKE_PCH_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pch-cache")
target_precompile_headers(${PROJECT_NAME} PRIVATE "${SRC_INCLUDE_DIR}/pch.h")

if(WIN32)
    message(STATUS "Building for Windows")
    set(SYSTEM_LIB
        wavpack advapi32 gdi32 vorbisfile vorbis ogg mingw32
        pthread crypto setupapi imm32 mpg123 winmm
        version ws2_32 crypt32 user32 kernel32
    )
elseif(UNIX)
    message(STATUS "Building for Linux")
    set(SYSTEM_LIB pthread X11)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}.")
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SDL2::SDL2
        SDL2_mixer::SDL2_mixer
        Boost::boost
        tgbot::tgbot
       ${LIBVLC_LIBRARIES}
       ${SYSTEM_LIB}
)

target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-z,stack-size=${STACK_SIZE}")

target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_INCLUDE_DIR} ${LIBVLC_INCLUDE_DIRS})
if(DEFINED TELEGRAM_API_TOKEN)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TELEGRAM_API_TOKEN="${TELEGRAM_API_TOKEN}")
endif()
