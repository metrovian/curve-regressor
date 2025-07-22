#include "sensor_simulator.h"

static double sensor_simulator_random(double minimum, double maximum) {
	double random = rand() / (double)RAND_MAX;
	return random * (maximum - minimum) + minimum;
}

static double sensor_simulator_rtd(double domain) {
	const static double CONST_RTD_R0 = 1.0000E+02;
	const static double CONST_RTD_A = 3.9083E-03;
	const static double CONST_RTD_B = -5.7750E-07;
	return CONST_RTD_R0 * (1.0000E+00 + CONST_RTD_A * domain + CONST_RTD_B * domain * domain);
}

static double sensor_simulator_pd(double domain) {
	const static double CONST_PD_P0 = 1.0000E+01;
	const static double CONST_PD_A = 1.0000E-03;
	return CONST_PD_P0 / domain / domain + CONST_PD_A;
}

extern int8_t sensor_simulator(sensor_simulator_t sensor, int32_t counts, double *domain, double *range) {
	for (int32_t i = 0; i < counts; ++i) {
		switch (sensor) {
		case SENSOR_RTD: {
			domain[i] = sensor_simulator_random(0, 650);
			range[i] = sensor_simulator_rtd(domain[i]);
		}

		case SENSOR_PD: {
			domain[i] = sensor_simulator_random(1.0, 5.0);
			range[i] = sensor_simulator_pd(domain[i]);
		}

		default: {
			break;
		}
		}

		return 0;
	}
}