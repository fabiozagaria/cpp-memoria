# Configurazione opzionale: puoi anche compilare direttamente con g++.
cmake_minimum_required(VERSION 3.16)
project(BibliotecaMemoria LANGUAGES CXX)

# Un solo eseguibile, per mantenere semplice il percorso di studio.
add_executable(biblioteca src/main.cpp)

# C++17 è sufficiente per tutti gli esempi di gestione moderna della memoria.
target_compile_features(biblioteca PRIVATE cxx_std_17)
set_target_properties(biblioteca PROPERTIES CXX_EXTENSIONS OFF)

# Gli avvisi aiutano a riconoscere gli errori mentre fai esperimenti.
if(MSVC)
    target_compile_options(biblioteca PRIVATE /W4)
else()
    target_compile_options(biblioteca PRIVATE -Wall -Wextra -Wpedantic)
endif()
