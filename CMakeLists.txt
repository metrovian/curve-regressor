CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(data_regressor LANGUAGES C CXX)

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/eigen
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tinyexpr
)

FILE(GLOB_RECURSE SOURCES_CLASSIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source/classic/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/classic/*.c
)

FILE(GLOB_RECURSE SOURCES_SYMBOLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source/symbolic/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/symbolic/*.c
)

ADD_EXECUTABLE(classic
        module/main_classic.cpp
        3rdparty/tinyexpr/tinyexpr.c
        ${SOURCES_CLASSIC}
)

ADD_EXECUTABLE(symbolic
        module/main_symbolic.cpp
        ${SOURCES_SYMBOLIC}
)