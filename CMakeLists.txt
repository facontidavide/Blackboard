cmake_minimum_required(VERSION 2.8)

project(Blackboard)

add_definitions(-std=c++11)

INCLUDE_DIRECTORIES( ./include )

add_executable(${PROJECT_NAME} main.cpp  )

