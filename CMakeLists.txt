cmake_minimum_required(VERSION 3.26)

project(TgControlPanel VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TgControlPanel_include_dir "${CMAKE_SOURCE_DIR}/include")

set(SDL2_include_dir "${CMAKE_SOURCE_DIR}/libs/SDL2/include")
set(SDL2_lib_dir "${CMAKE_SOURCE_DIR}/libs/SDL2/lib/libSDL2.a")

set(SDL2mixer_include_dir "${CMAKE_SOURCE_DIR}/libs/SDL2mixer/include")
set(SDL2mixer_lib_dir "${CMAKE_SOURCE_DIR}/libs/SDL2mixer/lib/libSDL2_mixer.a")

set(TgBot_include_dir "${CMAKE_SOURCE_DIR}/libs/TgBot/include")
set(TgBot_lib_dir "${CMAKE_SOURCE_DIR}/libs/TgBot/lib/libTgBot.a")

# Поиск Boost (укажите компоненты, которые вы используете)
find_package(Boost REQUIRED COMPONENTS system thread)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost не найден!")
endif()

# Укажите исходные файлы
set(SOURCE_FILES
    src/TGControlPanel.cpp
    src/button_function.cpp
)

# Укажите директории с заголовочными файлами
include_directories(
    ${SDL2_include_dir}
    ${SDL2mixer_include_dir}
    ${TgBot_include_dir}
    ${Boost_INCLUDE_DIRS}
    ${TgControlPanel_include_dir}
)

# Добавляем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Указываем линковку со статическими библиотеками
target_link_libraries(${PROJECT_NAME}
    ${SDL2_lib_dir}
    ${SDL2mixer_lib_dir}
    ${TgBot_lib_dir}
    Boost::system
    Boost::thread
    ssl # Добавляем OpenSSL
    crypto # Добавляем библиотеку для работы с криптографией
    pthread # Для многопоточности, если требуется
)

# Добавляем путь к библиотекам для статической линковки
target_link_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/libs/SDL2/lib"
    "${CMAKE_SOURCE_DIR}/libs/SDL2mixer/lib"
    "${CMAKE_SOURCE_DIR}/libs/TgBot/lib"
)
