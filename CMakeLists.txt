CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(curve_regressor LANGUAGES C CXX)

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)
SET(CIVETWEB_BUILD_TESTING OFF CACHE BOOL "Disable civetweb tests" FORCE)

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/eigen
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tinyexpr
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/sqlite)

FILE(GLOB_RECURSE SOURCES_COMMON
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c)
        
ADD_LIBRARY(tinyexpr-c-library STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tinyexpr/tinyexpr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tinyexpr/tinyexpr.h)

ADD_LIBRARY(sqlite3-c-library STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/sqlite/sqlite3.h)

ADD_SUBDIRECTORY(3rdparty/civetweb)
ADD_SUBDIRECTORY(3rdparty/spdlog)

ADD_EXECUTABLE(start
        module/main_start.c
        ${SOURCES_COMMON})

TARGET_LINK_LIBRARIES(start 
        PRIVATE 
        civetweb-c-library
        tinyexpr-c-library
        sqlite3-c-library
        spdlog::spdlog)

ADD_CUSTOM_TARGET(HTML ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/service-civetweb"
        "${CMAKE_BINARY_DIR}/service-civetweb")

ADD_CUSTOM_TARGET(SQL ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/service-sqlite"
        "${CMAKE_BINARY_DIR}/service-sqlite")