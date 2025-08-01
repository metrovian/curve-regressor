// clang-format off
#pragma once
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define SYSTEM_LOGGER_SYSTEM    "system"
#define SYSTEM_LOGGER_FORMAT    "log"
#define SYSTEM_LOGGER_SIZE      4096
#define SYSTEM_LOGGER_LOGLEVEL  2

#define NETWORK_CIVETWEB_ROOT   "service-civetweb"
#define NETWORK_CIVETWEB_PORT   "8080"
#define NETWORK_SQLITE_DB       "service-sqlite/measurements.db"
#define NETWORK_SQLITE_SCHEMA   "service-sqlite/schema.sql"

#define SENSOR_DATA_COUNTS      100
#define SENSOR_DATA_MAX         1000
#define SYMBOLIC_DEGREE_MAX     15
#define SYMBOLIC_ITERATION_MAX  100
#define SYMBOLIC_EPSILON        1.0000E-15
// clang-format on