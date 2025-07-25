#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
	MODEL_LINEAR = 0,
	MODEL_RECIPROCAL = 1,
	MODEL_POLYNOMIAL = 2,
	MODEL_SINUSOIDAL = 3,
} regression_model_t;

extern double calibrate(regression_model_t model, int32_t counts, double *domain, double *range, void *arguments, double *parameters);
#ifdef __cplusplus
}
#endif