// Trilateration algorithm for n amount of points
// https://gis.stackexchange.com/questions/40660/trilateration-algorithm-for-n-amount-of-points
// Least Squares Trilateration
#include <Arduino.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

#include "debug.h"
#include "access_point.h"
#include "trilateratie_n_points.h"

void VisualTrilateratie(const std::vector<AccessPoint> &aps,
                        const std::vector<double> &distances,
                        float deviceX, float deviceY, float RMSE)
{
    Serial.println("start VisualTrilateratie");

    int n = aps.size();
    for (int i = 0; i < n; i++)
    {
        Serial.println("VisualTrilateratie: apX" + String(aps[i].GetX()));
        Serial.println("VisualTrilateratie: apY" + String(aps[i].GetY()));
        Serial.println("VisualTrilateratie: distance" + String(distances[i]));
    }
    Serial.println("VisualTrilateratie: deviceX" + String(deviceX));
    Serial.println("VisualTrilateratie: deviceY" + String(deviceY));
    Serial.println("VisualTrilateratie: RMSE" + String(RMSE));

    Serial.println("end VisualTrilateratie");
}

// Solve 2x2 linear system:
// [a b][x] = [e]
// [c d][y] = [f]
//& betekent dat de waarde niet gekopieerd wordt maar regstreeks zal worden aangepast
bool solve2x2(double a, double b, double c, double d,
              double e, double f,
              double &outX, double &outY)
{
    double determinant = a * d - b * c;
    if (std::fabs(determinant) < 1e-9) // kijkt of determinant niet te klein is
        return false;

    outX = (e * d - b * f) / determinant; // regel van Cramer
    outY = (a * f - e * c) / determinant;
    return true;
}

// Least Squares Trilateration for N >= 3
trilaterationResult TrilateratieLeastSquares(const std::vector<AccessPoint> &aps,
                                             const std::vector<double> &distances)
{
    int n = aps.size();

    if (n < 3 || distances.size() != aps.size())
    {
        debugln("minimum 3 access points nodig, of aantal afstanden en access points komen niet overeen");
        return {0, 0, 0};
    }

    // Use first AP as reference
    double x1 = (float)aps[0].GetX();
    double y1 = (float)aps[0].GetY();
    double d1 = (float)distances[0];

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

        // vergelijking cirkel
        // Probleem: er zit 𝑥² en 𝑦² in → niet lineair.

        // Row of A
        double A0 = 2.0 * (xi - x1);
        double A1 = 2.0 * (yi - y1);

        // Neem cirkel i min cirkel 1: [ ( 𝑥 − 𝑥𝑖 )² + ( 𝑦 − 𝑦𝑖 )² ] − [ ( 𝑥 − 𝑥1 )² + ( 𝑦 − 𝑦1 )² ] = 𝑑𝑖²  − 𝑑²1
        //  Right-hand side b
        double bi = (d1 * d1 - di * di) - (x1 * x1 - xi * xi) - (y1 * y1 - yi * yi);
        // nu hebben we een lineaire vergelijking

        // w hoeveel moet dit ap meewegen groter w telt meer mee
        double w = 1.0; // standaard 100%
        if (di > 3.0)
            w -= 0.01136 * (di - 3.0);
        if (w < 0.0)
            w = 0.0;
        // Accumulate A^T A
        ATA00 += w * A0 * A0;
        ATA01 += w * A0 * A1;
        ATA11 += w * A1 * A1;

        // Accumulate A^T b
        ATb0 += w * A0 * bi;
        ATb1 += w * A1 * bi;
        // we hebben meer vergelijkingen dan onbekenden
        // matrix
        //[ATA00 ​ATA01​​][x]=[ATb0]
        //[ATA01 ​ATA11​​][y​]=[​ATb1​​]
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
    float RMSE = (float)std::sqrt(errorSum / n);
    VisualTrilateratie(aps, distances, x, y, RMSE);
    return {(float)x, (float)y, RMSE};
}

void test_TrilateratieLeastSquares()
{
    std::vector<AccessPoint> aps = {// mogen in willekeurige volgorde, moet niet met 0,0 starten
                                    {10, 10},
                                    {10.0, 0.0},
                                    {0.0, 10.0},
                                    {5, 15},
                                    {0, 0}};

    std::vector<double> distances = {
        7,
        7,
        7,
        10,
        7};

    AccessPoint device;
    trilaterationResult resultaat = TrilateratieLeastSquares(aps, distances);
    device.SetPos(resultaat.x, resultaat.y);
    device.SetRMSE(resultaat.RMSE);
    debugln("test_TrilateratieLeastSquares");
    debugln("positie = " + String(device.GetX()) + " " + String(device.GetY()));
    debugln("RMSE " + String(device.GetRMSE()));
    return;
}
