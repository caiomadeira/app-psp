cmake_minimum_required(VERSION 3.5)

project(APPPSP)
set(CMAKE_CXX_STANDARD 17)

# --- Configuração do Llama.cpp ---
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(GGML_DYNAMIC_BACKENDS OFF CACHE BOOL "Disable dynamic backend loading")
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "Build tests")
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
set(LLAMA_NATIVE OFF CACHE BOOL "Disable native optimizations for cross-compiling") # Adicionado para segurança

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/llama.cpp")

# --- Definição do nosso executável ---
add_executable(${PROJECT_NAME}
    main.cpp
    include/graphic.cpp
    include/player.cpp
    include/audio.cpp
    include/LLMInference.cpp
    # Compilamos o common.cpp do llama diretamente, evitando os arquivos problemáticos
    externals/llama.cpp/common/common.cpp
)

# --- Copiando Assets ---
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
add_custom_target(copy_assets ALL COMMENT "Copying assets to build directory...")
add_custom_command(TARGET copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}/audio" "${CMAKE_BINARY_DIR}/audio")
add_custom_command(TARGET copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}/font" "${CMAKE_BINARY_DIR}/font")
add_custom_command(TARGET copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}/img" "${CMAKE_BINARY_DIR}/img")
add_dependencies(${PROJECT_NAME} copy_assets)

# --- Includes e Bibliotecas ---
include(FindPkgConfig)
pkg_search_module(SDL3 REQUIRED sdl3)

# Adiciona os diretórios de include necessários
target_include_directories(${PROJECT_NAME} PRIVATE
    # CRÍTICO: Caminhos para os includes do llama.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/externals/llama.cpp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/externals/llama.cpp/common"
    # Seus outros includes
    "${CMAKE_SOURCE_DIR}/include" # Adicionado para encontrar seus próprios .h
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_image_INCLUDE_DIRS}
    ${SDL3_ttf_INCLUDE_DIRS}
    $ENV{PSPDEV}/psp/include
)

# Linka as bibliotecas necessárias
target_link_libraries(${PROJECT_NAME} PRIVATE
    llama ggml # Bibliotecas do llama.cpp que compilamos
    # Suas outras bibliotecas
    ${SDL3_LIBRARIES} ${SDL3_image_LIBRARIES} ${SDL3_ttf_LIBRARIES}
    SDL3 SDL3_image SDL3_ttf
    pspaudio pspaudiolib
    vorbis ogg modplug
    harfbuzz freetype
    png jpeg z bz2
    pspctrl pspgum pspgu pspdisplay pspdebug pspge pspvram m
)

if(PSP)
    create_pbp_file(TARGET ${PROJECT_NAME} ICON_PATH NULL BACKGROUND_PATH NULL PREVIEW_PATH NULL TITLE ${PROJECT_NAME})
endif()