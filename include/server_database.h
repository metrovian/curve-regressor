#pragma once
#include "sensor_simulator.h"

extern int8_t server_database_open();
extern int8_t server_database_close();
extern int8_t server_database_status();
extern int8_t server_database_insert_sensor_measurements(sensor_simulator_t sensor, int32_t counts, double *domain, double *range);