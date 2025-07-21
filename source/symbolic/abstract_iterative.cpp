#include "abstract_iterative.h"

Eigen::VectorXd abstract_iterative::calibrate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const int counts, const double epsilon) {
	double residual_prev = 0;
	double residual_now = 0;
	for (int32_t i = 0; i < counts; ++i) {
		residual_prev = residual_now;
		residual_now = step_model(domain, range);
		if (std::fabs(residual_now - residual_prev) < epsilon) {
			break;
		}
	}

	Eigen::VectorXd residual(domain.size());
	for (int32_t i = 0; i < domain.size(); ++i) {
		residual(i) = residual_model(domain(i), range(i));
	}

	return residual;
}

Eigen::VectorXd abstract_iterative::estimate(const Eigen::VectorXd &domain) {
	Eigen::VectorXd estimation(domain.size());
	for (int32_t i = 0; i < domain.size(); ++i) {
		estimation(i) = construct_model(domain(i));
	}

	return estimation;
}

double regressor_lasso::construct_model(const double domain) {
}

double regressor_lasso::residual_model(const double domain, const double range) {
}

double regressor_lasso::residual_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
}

double regressor_lasso::step_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
}