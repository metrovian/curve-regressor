#include "sensor_simulator.h"

static double sensor_simulator_random(double minimum, double maximum) {
	double random = rand() / (double)RAND_MAX;
	return random * (maximum - minimum) + minimum;
}

static double sensor_simulator_rtd(double domain) {
	const static double CONST_RTD_R0 = 1.0000E+02;
	const static double CONST_RTD_A = 3.9083E-03;
	const static double CONST_RTD_B = -5.7750E-07;
	const static double CONST_RTD_X = 1.0000E+00;
	return CONST_RTD_R0 * (CONST_RTD_X + CONST_RTD_A * domain + CONST_RTD_B * domain * domain);
}

static double sensor_simulator_pd(double domain) {
	const static double CONST_PD_P0 = 1.0000E+01;
	const static double CONST_PD_A = 1.0000E-03;
	return CONST_PD_P0 / domain / domain + CONST_PD_A;
}

static double sensor_simulator_lvdt(double domain) {
	const static double CONST_LVDT_V0 = 2.0000E-02;
	const static double CONST_LVDT_V1 = 1.0000E-04;
	return CONST_LVDT_V0 * domain + CONST_LVDT_V1 * domain * domain * domain;
}

static double sensor_simulator_mmi(double domain) {
	const static double CONST_MMI_V0 = 1.0000E-03;
	const static double CONST_MMI_A = 1.5500E-03;
	const static double CONST_MMI_B = M_PI_4;
	return CONST_MMI_V0 * cos(M_2_PI / CONST_MMI_A * domain + CONST_MMI_B);
}

static double sensor_simulator_mzi(double domain) {
	const static double CONST_MZI_V0 = 1.0000E-03;
	const static double CONST_MZI_A = 1.5500E-01;
	const static double CONST_MZI_B = M_PI_4;
	return CONST_MZI_V0 * cos(M_2_PI / CONST_MZI_A * domain + CONST_MZI_B);
}

extern int8_t sensor_simulator(sensor_simulator_t sensor, int32_t counts, double *domain, double *range) {
	for (int32_t i = 0; i < counts; ++i) {
		switch (sensor) {
		case SENSOR_RTD: {
			domain[i] = sensor_simulator_random(0.0000E+00, 6.5000E+02);
			range[i] = sensor_simulator_rtd(domain[i]);
			break;
		}

		case SENSOR_PD: {
			domain[i] = sensor_simulator_random(1.0000E+00, 5.0000E+00);
			range[i] = sensor_simulator_pd(domain[i]);
			break;
		}

		case SENSOR_LVDT: {
			domain[i] = sensor_simulator_random(-1.5000E+01, 1.5000E+01);
			range[i] = sensor_simulator_lvdt(domain[i]);
			break;
		}

		case SENSOR_MMI: {
			domain[i] = sensor_simulator_random(-1.5000E-03, 1.5000E-03);
			range[i] = sensor_simulator_mmi(domain[i]);
			break;
		}

		case SENSOR_MZI: {
			domain[i] = sensor_simulator_random(1.0000E+00, 1.5000E+00);
			range[i] = sensor_simulator_mzi(domain[i]);
			break;
		}

		default: {
			break;
		}
		}

		return 0;
	}
}