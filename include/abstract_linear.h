#pragma once
#include "Eigen/Dense"

class abstract_linear {
protected:
	Eigen::VectorXd parameters;

protected:
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
	virtual Eigen::MatrixXd construct_model(const Eigen::VectorXd &domain) const override;
};