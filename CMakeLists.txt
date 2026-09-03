#CMAKELISTS Central para todo el repo
#IMPORTANTE IR ACTUALIZANDO MIENTRAS MAS TRABAJOS SE AGRUEGUEN
cmake_minimum_required(VERSION 3.14)
project(EstructuraDeDatosII CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ejecutable de programa
add_executable(main.cpp)
add_executable(Decoder.cpp)