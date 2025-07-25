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

Eigen::VectorXd abstract_iterative::export_parameters() {
	return parameters;
}

Eigen::VectorXd abstract_iterative::import_parameters(const Eigen::VectorXd &import) {
	parameters = import;
	return parameters;
}

double regressor_lasso::construct_model(const double domain) {
	double model = 0;
	for (int32_t i = 0; i < parameters_degree + 1; ++i) {
		model += parameters(parameters_degree - i) * std::pow(domain, i);
	}

	return model;
}

double regressor_lasso::residual_model(const double domain, const double range) {
	return std::pow(construct_model(domain) - range, 2.0);
}

double regressor_lasso::residual_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	double residual = 0;
	for (int32_t i = 0; i < domain.size(); ++i) {
		residual += residual_model(domain(i), range(i));
	}

	residual /= static_cast<double>(domain.size() * 2);
	for (int32_t i = 0; i < parameters_degree + 1; ++i) {
		residual += parameters_lambda * std::fabs(parameters(i));
	}

	return residual;
}

double regressor_lasso::step_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	for (int32_t i = 0; i < parameters_degree + 1; ++i) {
		double numerator = 0;
		double denominator = 0;
		for (int32_t j = 0; j < domain.size(); ++j) {
			double power = std::pow(domain(j), i);
			double predict = std::pow(range(j), 1);
			for (int32_t k = 0; k < parameters_degree + 1; ++k) {
				if (k != i) {
					predict -= parameters(parameters_degree - k) * std::pow(domain(j), k);
				}
			}

			numerator += predict * power;
			denominator += power * power;
		}

		if (denominator) {
			if (numerator > parameters_lambda) {
				parameters(parameters_degree - i) = (numerator - parameters_lambda) / denominator;
			} else if (numerator < -parameters_lambda) {
				parameters(parameters_degree - i) = (numerator + parameters_lambda) / denominator;
			} else {
				parameters(parameters_degree - i) = 0;
			}
		}
	}

	return residual_model(domain, range);
}

regressor_lasso::regressor_lasso(const int degree, const double lambda) : parameters_degree(degree), parameters_lambda(lambda) {
	parameters = Eigen::VectorXd(degree + 1);
}

double regressor_ridge::construct_model(const double domain) {
	double model = 0;
	for (int32_t i = 0; i < parameters_degree + 1; ++i) {
		model += parameters(parameters_degree - i) * std::pow(domain, i);
	}

	return model;
}

double regressor_ridge::residual_model(const double domain, const double range) {
	return std::pow(construct_model(domain) - range, 2.0);
}

double regressor_ridge::residual_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	double residual = 0;
	for (int32_t i = 0; i < domain.size(); ++i) {
		residual += residual_model(domain(i), range(i));
	}

	residual /= static_cast<double>(domain.size() * 2);
	for (int32_t i = 0; i < parameters_degree + 1; ++i) {
		residual += parameters_lambda * std::pow(parameters(i), 2.0);
	}

	return residual;
}

double regressor_ridge::step_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	for (int32_t i = 0; i < parameters_degree + 1; ++i) {
		double numerator = 0;
		double denominator = 0;

		for (int32_t j = 0; j < domain.size(); ++j) {
			double power = std::pow(domain(j), i);
			double predict = std::pow(range(j), 1);
			for (int32_t k = 0; k < parameters_degree + 1; ++k) {
				if (k != i) {
					predict -= parameters(parameters_degree - k) * std::pow(domain(j), k);
				}
			}

			numerator += predict * power;
			denominator += power * power;
		}

		denominator += parameters_lambda * 2;
		if (denominator) {
			parameters(parameters_degree - i) = numerator / denominator;
		}
	}

	return residual_model(domain, range);
}

regressor_ridge::regressor_ridge(const int degree, const double lambda) : parameters_degree(degree), parameters_lambda(lambda) {
	parameters = Eigen::VectorXd(degree + 1);
}