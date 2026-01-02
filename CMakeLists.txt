cmake_minimum_required(VERSION 3.16)
project(Progetto_CLion)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)


add_executable(Progetto_CLion
        src/main.cpp
        src/Preventivo.cpp
        src/ListinoPrezzi.cpp
        src/VoceTinteggiatura.cpp
        src/VoceCartongesso.cpp
        src/RegolaTinteggiatura.cpp
        src/RegolaCartongesso.cpp
        src/CalcolatorePreventivo.cpp
        src/VoceCartongessoBuilder.cpp
        src/Utils.cpp
        src/GestioneInputUI.cpp
        src/CatalogoCicli.cpp
        src/ListinoDefault.cpp
        src/IdPreventivoGenerator.cpp
        src/SalvataggioPreventivo.cpp
        include/VoceCartongesso.h


)