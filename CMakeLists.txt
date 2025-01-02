cmake_minimum_required(VERSION 3.11)
project(TGControlPanel VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_CLONE_TIMEOUT 500)  # Увеличьте таймаут до 300 секунд
include(FetchContent)

FetchContent_Declare(
    TgBotCpp
    GIT_REPOSITORY https://github.com/reo7sp/tgbot-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(TgBotCpp)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.26.5
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG release-2.6.3
)
FetchContent_MakeAvailable(SDL2_mixer)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(TGControlPanel src/TGControlPanel.cpp)

target_include_directories(TGControlPanel PUBLIC 
    ${tgbot_INCLUDE_DIRS} 
    ${sdl2_SOURCE_DIR}/include 
    ${sdl2_mixer_SOURCE_DIR}/include
)

target_link_libraries(TGControlPanel PUBLIC 
    TgBot
    Boost::boost 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    SDL2 
    SDL2_mixer
)
