CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(data_regressor LANGUAGES C CXX)

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/eigen
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tinyexpr
)

FILE(GLOB_RECURSE SOURCES_COMMON
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c
)

ADD_EXECUTABLE(start
        module/main_start.cpp
        3rdparty/tinyexpr/tinyexpr.c
        ${SOURCES_COMMON}
)