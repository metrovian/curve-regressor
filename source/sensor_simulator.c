#include "sensor_simulator.h"

static double sensor_simulator_random(double minimum, double maximum) {
	double random = rand() / (double)RAND_MAX;
	return random * (maximum - minimum) + minimum;
}

extern int8_t sensor_simulator(sensor_simulator_t sensor, int32_t counts, double *domain, double *range) {
	for (int32_t i = 0; i < counts; ++i) {
		switch (sensor) {
		case SENSOR_RTD: {
			const static double CONST_RTD_R0 = 1.0000E+02;
			const static double CONST_RTD_A = 3.9083E-03;
			const static double CONST_RTD_B = -5.7750E-07;
			domain[i] = sensor_simulator_random(0, 650);
			range[i] = CONST_RTD_R0 * (1 + CONST_RTD_A * domain[i] + CONST_RTD_B * domain[i] * domain[i]);
		}

		case SENSOR_PD: {
			const static double CONST_PD_P0 = 1.0000E+01;
			const static double CONST_PD_A = 1.0000E-04;
			domain[i] = sensor_simulator_random(1.0, 5.0);
			range[i] = CONST_PD_P0 * CONST_PD_A / domain[i] / domain[i];
		}

		default: {
			break;
		}
		}

		return 0;
	}