cmake_minimum_required(VERSION 3.16)
project(Progetto_CLion)

# Uso C++11 per restare compatibile con Dev-C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Per includere glie header nella cartella include
include_directories(include)

# Per il compilatore
add_executable(Progetto_CLion
        src/main.cpp
        src/Preventivo.cpp
        src/VoceTinteggiatura.cpp
        src/ListinoPrezzi.cpp
        include/ListinoPrezzi.h
        include/GradoDifficolta.h
)
