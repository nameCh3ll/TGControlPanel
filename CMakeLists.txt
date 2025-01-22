cmake_minimum_required(VERSION 3.26)

project(TGControlPanel VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/sources")
# set(BUILD_SHARED_LIBS
#     OFF
#     CACHE BOOL "Build static libraries instead of shared libraries" FORCE)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  message(STATUS "Building for Windows")
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")
  set(LIBRARY_DIR "${PROJECT_SOURCE_DIR}/libs/lib")
  set(LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/libraries/include")

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  message(STATUS "Building for Linux")

  set(LIBRARY_DIR "${PROJECT_SOURCE_DIR}/libs/x64linux")
  set(LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/includes/libraries/linux")
else()
  message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}.")

endif()
# set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
# set(LIBRARIES
#     ${LIBRARY_DIR}/libssl.a # version 3.4.0
#     ${LIBRARY_DIR}/libcrypto.a # version 3.4.0
#     ${LIBRARY_DIR}/libSDL2.a # version 2.31.0
#     ${LIBRARY_DIR}/libSDL2_mixer.a # version 2.8.0
#     ${LIBRARY_DIR}/libboost_system.a # version 1.86.0
#     ${LIBRARY_DIR}/libboost_thread.a # version 1.86.0
#     ${LIBRARY_DIR}/libTgBot.a # version 1.8
#     ${LIBRARY_DIR}/libwavpack.a)
# link_directories(/usr/x86_64-w64-mingw32/lib)
file(GLOB LIBRARIES "${LIBRARY_DIR}/*.a")
include_directories(${LIB_INCLUDE_DIR} ${SRC_INCLUDE_DIR})

file(GLOB SOURCE_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
# message(STATUS "Found libraries: ${LIBRARIES}")
