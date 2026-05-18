cmake_minimum_required(VERSION 4.2)
project(ProjectC C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjectC main.c
        event.h
        event.c
        network.h
        server.h
        network.c
        simulation.c
        simulation.h
        )
