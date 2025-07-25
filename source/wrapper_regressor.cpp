#include "wrapper_regressor.h"
#include "wrapper_spdlog.h"
#include "predefined.h"
#include "abstract_linear.h"
#include "abstract_iterative.h"

extern double calibrate_classic(regression_model_t model, int32_t counts, double *domain, double *range, void *arguments, double *parameters) {
	abstract_linear *regressor = NULL;
	Eigen::VectorXd domain_vector = Eigen::Map<Eigen::VectorXd>(domain, counts);
	Eigen::VectorXd range_vector = Eigen::Map<Eigen::VectorXd>(range, counts);
	switch (model) {
	case MODEL_LINEAR:
		regressor = new regressor_linear();
		log_debug("regression model: linear");
		break;

	case MODEL_RECIPROCAL:
		regressor = new regressor_reciprocal(*((int8_t *)arguments));
		log_debug("regression model: reciprocal");
		break;

	case MODEL_POLYNOMIAL:
		regressor = new regressor_polynomial(*((int8_t *)arguments));
		log_debug("regression model: polynomial");
		break;

	case MODEL_SINUSOIDAL:
		regressor = new regressor_sinusoidal(*((double *)arguments));
		log_debug("regression model: sinusoidal");
		break;
	}

	if (!regressor) {
		log_critical("invalid regression model: %d", model);
		return -1;
	}

	Eigen::VectorXd residual_vector = regressor->calibrate(domain_vector, range_vector);
	Eigen::VectorXd parameters_vector = regressor->export_parameters();
	std::memcpy(parameters, parameters_vector.data(), parameters_vector.size() * sizeof(double));
	delete regressor;
	double residual = 0;
	for (int32_t i = 0; i < residual_vector.size(); ++i) {
		residual += residual_vector(i);
		log_debug("residual: %+.4E", residual_vector(i));
	}

	residual /= (double)residual_vector.size();
	return residual;
}

extern double calibrate_symbolic(regression_model_t model, int32_t counts, double *domain, double *range, void *arguments, double *parameters) {
	abstract_iterative *regressor = NULL;
	Eigen::VectorXd domain_vector = Eigen::Map<Eigen::VectorXd>(domain, counts);
	Eigen::VectorXd range_vector = Eigen::Map<Eigen::VectorXd>(range, counts);
	switch (model) {
	case MODEL_SYMBOLIC_TAYLOR_LASSO:
		regressor = new regressor_lasso(SYMBOLIC_DEGREE_MAX, *((double *)arguments));
		log_debug("regression model: polynomial L1");
		break;

	case MODEL_SYMBOLIC_TAYLOR_RIDGE:
		regressor = new regressor_ridge(SYMBOLIC_DEGREE_MAX, *((double *)arguments));
		log_debug("regression model: polynomial L2");
		break;
	}

	if (!regressor) {
		log_critical("invalid regression model: %d", model);
		return -1;
	}

	Eigen::VectorXd residual_vector = regressor->calibrate(domain_vector, range_vector, SYMBOLIC_ITERATION_MAX, SYMBOLIC_EPSILON);
	Eigen::VectorXd parameters_vector = regressor->export_parameters();
	std::memcpy(parameters, parameters_vector.data(), parameters_vector.size() * sizeof(double));
	delete regressor;
	double residual = 0;
	for (int32_t i = 0; i < residual_vector.size(); ++i) {
		residual += residual_vector(i);
		log_debug("residual: %+.4E", residual_vector(i));
	}

	residual /= (double)residual_vector.size();
	return residual;
}