#ifndef NON_LINEAR_LEAST_SQUARES
#define NON_LINEAR_LEAST_SQUARES

#include <vector>
struct Punt
{
    double x, y;
};
int test_non_linear_least_squares(const std::vector<AccessPoint> &aps1,
                                  const std::vector<double> &distances1);
int test_non_linear_least_squares();
#endif