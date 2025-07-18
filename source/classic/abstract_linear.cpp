#include "abstract_linear.h"

Eigen::VectorXd abstract_linear::pre_process(const Eigen::VectorXd &range) const {
	return range;
}

Eigen::VectorXd abstract_linear::post_process(const Eigen::VectorXd &range) const {
	return range;
}

Eigen::VectorXd abstract_linear::calibrate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	Eigen::MatrixXd model = construct_model(domain);
	Eigen::VectorXd model_response = pre_process(range);
	parameters = (model.transpose() * model).ldlt().solve(model.transpose() * model_response);
	return estimate(domain) - range;
}

Eigen::VectorXd abstract_linear::estimate(const Eigen::VectorXd &domain) {
	Eigen::MatrixXd model = construct_model(domain);
	Eigen::VectorXd model_estimate = model * parameters;
	return post_process(model_estimate);
}

Eigen::MatrixXd regressor_linear::construct_model(const Eigen::VectorXd &domain) const {
	Eigen::MatrixXd model(domain.size(), 2);
	model.col(0) = domain;
	model.col(1) = Eigen::VectorXd::Ones(domain.size());
	return model;
}

Eigen::MatrixXd regressor_polynomial::construct_model(const Eigen::VectorXd &domain) const {
	Eigen::MatrixXd model(domain.size(), parameters_degree + 1);
	for (int8_t i = 0; i < parameters_degree + 1; ++i) {
		model.col(parameters_degree - i) = domain.array().pow(i).matrix();
	}

	return model;
}

Eigen::MatrixXd regressor_sinusoidal::construct_model(const Eigen::VectorXd &domain) const {
	Eigen::MatrixXd model(domain.size(), 3);
	for (int32_t i = 0; i < domain.size(); ++i) {
		model(i, 0) = std::sin(M_2_PI * domain(i) / parameters_period);
		model(i, 1) = std::cos(M_2_PI * domain(i) / parameters_period);
		model(i, 2) = 1.0;
	}

	return model;
}

Eigen::MatrixXd regressor_exponential::construct_model(const Eigen::VectorXd &domain) const {
	Eigen::MatrixXd model(domain.size(), 2);
	model.col(0) = domain;
	model.col(1) = Eigen::VectorXd::Ones(domain.size());
	return model;
}

Eigen::VectorXd regressor_exponential::pre_process(const Eigen::VectorXd &range) const {
	return range.array().log().matrix();
}

Eigen::VectorXd regressor_exponential::post_process(const Eigen::VectorXd &range) const {
	return range.array().exp().matrix();
}