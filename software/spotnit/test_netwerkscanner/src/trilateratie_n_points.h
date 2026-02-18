#ifndef TRILATERATIE_N_POINTS
#define TRILATERATIE_N_POINTS

#include "access_point.h"

struct trilaterationResult
{
    float x;
    float y;
    float RMSE;
};

void test_TrilateratieLeastSquares();
trilaterationResult TrilateratieLeastSquares(const std::vector<AccessPoint> &aps,
                                             const std::vector<double> &distances);

#endif