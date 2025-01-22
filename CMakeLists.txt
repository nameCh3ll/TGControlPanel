cmake_minimum_required(VERSION 3.26)

project(TGControlPanel VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/sources")
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "Build static libraries instead of shared libraries" FORCE)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  message(STATUS "Building for Windows")

  set(LIBRARY_DIR "${PROJECT_SOURCE_DIR}/libs/x64windows")
  set(LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/libraries/windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  message(STATUS "Building for Linux")

  set(LIBRARY_DIR "${PROJECT_SOURCE_DIR}/libs/x64linux")
  set(LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/libraries/linux")
else()
  message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}.")

endif()

file(GLOB LIBRARIES "${LIBRARY_DIR}/*.a")
include_directories(${LIB_INCLUDE_DIR} ${SRC_INCLUDE_DIR})

file(GLOB SOURCE_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
