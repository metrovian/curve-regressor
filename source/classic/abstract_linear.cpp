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