#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef enum {
	SENSOR_RTD = 0,
	SENSOR_PD = 1,
	SENSOR_LVDT = 2,
	SENSOR_MMI = 3,
	SENSOR_MZI = 4,
} sensor_simulator_t;

extern int8_t sensor_simulator(sensor_simulator_t sensor, int32_t counts, double *domain, double *range);