cmake_minimum_required(VERSION 3.28)
project(HangmanGameFnct C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/SDL2-devel-2.30.2-mingw/SDL2-2.30.2/x86_64-w64-mingw32")


set(SDL2_INCLUDE_DIR "C:/libs/SDL2-2.30.2/include")
set(SDL2_LIB_DIR "C:/libs/SDL2-2.30.2/lib/x64")

set(SDL2_IMAGE_INCLUDE_DIR "C:/libs/SDL2_image-2.8.2/include")
set(SDL2_IMAGE_LIB_DIR "C:/libs/SDL2_image-2.8.2/lib/x64")

set(SDL2_MIXER_INCLUDE_DIR "C:/libs/SDL2_mixer-2.8.0/include")
set(SDL2_MIXER_LIB_DIR "C:/libs/SDL2_mixer-2.8.0/lib/x64")

set(SDL2_TTF_INCLUDE_DIR "C:/libs/SDL2_ttf-2.22.0/include")
set(SDL2_TTF_LIB_DIR "C:/libs/SDL2_ttf-2.22.0/lib/x64")



find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

link_directories(${SDL2_LIB_DIR} ${SDL2_IMAGE_LIB_DIR} ${SDL2_MIXER_LIB_DIR} ${SDL2_TTF_LIB_DIR})

add_executable(HangmanGameFnct main.c
        constants.h
)

target_link_libraries(HangmanGameFnct PRIVATE SDL2main SDL2 SDL2_image SDL2_mixer SDL2_TTF)
