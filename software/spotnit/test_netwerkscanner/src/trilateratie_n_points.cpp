// Trilateration algorithm for n amount of points
// https://gis.stackexchange.com/questions/40660/trilateration-algorithm-for-n-amount-of-points
// Least Squares Trilateration
#include <Arduino.h>

// input een lijst van alle access points
//  haalt de locatie van de access points
//  vul in de afstand van device naar access point
//  geef een geschatte locatie en radius waarbinnen het device kan liggen.

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

#include "debug.h"
#include "access_point.h"

struct trilaterationResult
{
    float x;
    float y;
    float RMSE;
};

// Solve 2x2 linear system:
// [a b][x] = [e]
// [c d][y]   [f]
bool solve2x2(double a, double b, double c, double d,
              double e, double f,
              double &outX, double &outY)
{
    double det = a * d - b * c;
    if (std::fabs(det) < 1e-9)
        return false;

    outX = (e * d - b * f) / det;
    outY = (a * f - e * c) / det;
    return true;
}

// Least Squares Trilateration for N >= 3
trilaterationResult TrilateratieLeastSquares(const std::vector<AccessPoint> &aps,
                                             const std::vector<double> &distances)
{
    int n = aps.size();

    if (n < 3 || distances.size() != aps.size())
    {
        debugln("minimum 3 access points nodig, ");
        return {0, 0, 0};
    }

    // Use first AP as reference
    double x1 = (float) aps[0].GetX();
    double y1 = (float) aps[0].GetY();
    double d1 = (float) distances[0];

    // We build system: A * p = b
    // where p = [x, y]
    // each row is derived from subtracting circle equations

    // We'll solve using normal equations:
    // (A^T A) p = (A^T b)

    double ATA00 = 0, ATA01 = 0, ATA11 = 0;
    double ATb0 = 0, ATb1 = 0;

    for (int i = 1; i < n; i++)
    {
        double xi = aps[i].GetX();
        double yi = aps[i].GetY();
        double di = distances[i];

        // Row of A
        double A0 = 2.0 * (xi - x1);
        double A1 = 2.0 * (yi - y1);

        // Right-hand side b
        double bi = (d1 * d1 - di * di) - (x1 * x1 - xi * xi) - (y1 * y1 - yi * yi);

        // Accumulate A^T A
        ATA00 += A0 * A0;
        ATA01 += A0 * A1;
        ATA11 += A1 * A1;

        // Accumulate A^T b
        ATb0 += A0 * bi;
        ATb1 += A1 * bi;
    }

    double x, y;
    bool ok = solve2x2(ATA00, ATA01,
                       ATA01, ATA11,
                       ATb0, ATb1,
                       x, y);

    if (!ok)
    {
        debugln("ERROR: Singular matrix (bad AP geometry). Access points op 1 lijn?");
        return {0, 0, 0};
    }

    // Calculate RMSE (root mean squared error)
    double errorSum = 0.0;
    for (int i = 0; i < n; i++)
    {
        double dx = x - aps[i].GetX();
        double dy = y - aps[i].GetY();
        double predicted = std::sqrt(dx * dx + dy * dy);

        double err = predicted - distances[i];
        errorSum += err * err;
    }

    return {(float)x, (float)y, (float)std::sqrt(errorSum / n)};
}

void test_TrilateratieLeastSquares()
{
    std::vector<AccessPoint> aps = {
        {0.0, 0.0},
        {10.0, 0.0},
        {0.0, 10.0},
        {10.0, 10.0}};

    // Example: device is around (5,5) but distances are noisy
    std::vector<double> distances = {
        5, // real should be 7.071
        5,
        5,
        5};

    AccessPoint device;
    trilaterationResult resultaat = TrilateratieLeastSquares(aps, distances);
    device.SetPos(resultaat.x, resultaat.y);
    device.SetRMSE(resultaat.RMSE);
    debugln("test_TrilateratieLeastSquares");
    debugln("positie = " + String(device.GetX()) + " " + String(device.GetY()));
    debugln("RMSE " + String(device.GetRMSE()));
    return;
}
