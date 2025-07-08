#include <Eigen/Core>

extern "C" {
#include "tinyexpr.h"
}

int main() {
	printf(
	    "Eigen version: %d.%d.%d\n",
	    EIGEN_WORLD_VERSION,
	    EIGEN_MAJOR_VERSION,
	    EIGEN_MINOR_VERSION);

	const char *expr_str = "sqrt(2)";
	te_expr *expr = te_compile(expr_str, NULL, 0, NULL);
	if (expr) {
		double result = te_eval(expr);
		printf("Result: %f\n", result);
		te_free(expr);
	} else {
		printf("Failed to compile expression.\n");
	}

	return 0;
}