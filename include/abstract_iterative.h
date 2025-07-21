#pragma once
#include "Eigen/Dense"

class abstract_iterative {
protected:
	Eigen::VectorXd parameters;
	virtual double construct_model(const double domain) = 0;
	virtual double residual_model(const double domain, const double range) = 0;
	virtual double residual_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) = 0;
	virtual double step_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) = 0;

public:
	Eigen::VectorXd calibrate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const int counts, const double epsilon);
	Eigen::VectorXd estimate(const Eigen::VectorXd &domain);
	virtual ~abstract_iterative() = default;
};

class regressor_lasso : public abstract_iterative {
protected:
	int parameters_degree;
	double parameters_lambda;
	virtual double construct_model(const double domain) final;
	virtual double residual_model(const double domain, const double range) final;
	virtual double residual_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) final;
	virtual double step_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) final;

public:
	regressor_lasso(const int degree, const double lambda);
};

class regressor_ridge : public abstract_iterative {
protected:
	int parameters_degree;
	double parameters_lambda;
	virtual double construct_model(const double domain) final;
	virtual double residual_model(const double domain, const double range) final;
	virtual double residual_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) final;
	virtual double step_model(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) final;

public:
	regressor_ridge(const int degree, const double lambda);
};