cmake_minimum_required(VERSION 3.23)
project(zadanie4)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(zadanie4
        organism.h
        organism_example.cc)
