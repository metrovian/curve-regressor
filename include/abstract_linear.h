#pragma once
#include "Eigen/Dense"

class abstract_linear {
protected:
	Eigen::VectorXd parameters;
	virtual Eigen::MatrixXd construct_model(const Eigen::VectorXd &domain) const = 0;
	virtual Eigen::VectorXd pre_process(const Eigen::VectorXd &range) const;
	virtual Eigen::VectorXd post_process(const Eigen::VectorXd &range) const;

public:
	Eigen::VectorXd calibrate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range);
	Eigen::VectorXd estimate(const Eigen::VectorXd &domain);
	virtual ~abstract_linear() = default;
};

class regressor_linear : public abstract_linear {
protected:
	virtual Eigen::MatrixXd construct_model(const Eigen::VectorXd &domain) const final;
};

class regressor_reciprocal : public abstract_linear {
protected:
	int8_t parameters_degree;
	virtual Eigen::MatrixXd construct_model(const Eigen::VectorXd &domain) const final;

public:
	regressor_reciprocal(int8_t degree) : parameters_degree(degree) {}
};

class regressor_polynomial : public abstract_linear {
protected:
	int8_t parameters_degree;
	virtual Eigen::MatrixXd construct_model(const Eigen::VectorXd &domain) const final;

public:
	regressor_polynomial(int8_t degree) : parameters_degree(degree) {}
};

class regressor_sinusoidal : public abstract_linear {
protected:
	double parameters_period;
	virtual Eigen::MatrixXd construct_model(const Eigen::VectorXd &domain) const final;

public:
	regressor_sinusoidal(double period) : parameters_period(period) {}
};

class regressor_exponential : public abstract_linear {
protected:
	double parameters_bias;
	virtual Eigen::MatrixXd construct_model(const Eigen::VectorXd &domain) const final;
	virtual Eigen::VectorXd pre_process(const Eigen::VectorXd &range) const final;
	virtual Eigen::VectorXd post_process(const Eigen::VectorXd &range) const final;

public:
	regressor_exponential(double bias) : parameters_bias(bias) {}
};