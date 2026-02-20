#include <iostream>
#include <vector>
#include <cmath>
#include <utility>

#include "access_point.h"

struct Punt
{
    double x, y;
};

// Kostenfunctie
double kostenFunctie(const Punt &schatting, const std::vector<Punt> &aps, const std::vector<double> &afstanden)
{
    double kosten = 0.0;
    for (size_t i = 0; i < aps.size(); ++i)
    {
        double dx = schatting.x - aps[i].x;
        double dy = schatting.y - aps[i].y;
        double r = std::sqrt(dx * dx + dy * dy);
        double w = 1.0 / (afstanden[i] * afstanden[i]);
        kosten += w * (r - afstanden[i]) * (r - afstanden[i]);
    }
    return kosten;
}

// Gradiëntafdaling (Gradient Descent)
Punt gradientAfdaling(const std::vector<Punt> &aps, const std::vector<double> &afstanden, Punt initieel, double lr = 0.01, int maxIter = 10000, double tol = 1e-6)
{
    Punt schatting = initieel;
    for (int iter = 0; iter < maxIter; ++iter)
    {
        double gradX = 0.0, gradY = 0.0;
        for (size_t i = 0; i < aps.size(); ++i)
        {
            double dx = schatting.x - aps[i].x;
            double dy = schatting.y - aps[i].y;
            double r = std::sqrt(dx * dx + dy * dy);
            if (r < 1e-8)
                r = 1e-8;
            double w = 1.0 / (afstanden[i] * afstanden[i]);
            double diff = r - afstanden[i];
            gradX += 2 * w * diff * dx / r;
            gradY += 2 * w * diff * dy / r;
        }
        schatting.x -= lr * gradX;
        schatting.y -= lr * gradY;
        if (std::sqrt(gradX * gradX + gradY * gradY) < tol)
            break;
    }
    return schatting;
}

// Eenvoudige 2x2 covariantiematrix berekenen
std::pair<std::pair<double, double>, double> berekenCovariantie(const Punt &schatting, const std::vector<Punt> &aps, const std::vector<double> &afstanden)
{
    double J[2][2] = {0}; // Informatie matrix

    for (size_t i = 0; i < aps.size(); ++i)
    {
        // 1. Bepaal de onzekerheid van deze specifieke meting
        double sigma_i = 0.1 + (afstanden[i] * 0.1); // 10cm basis + 10% van afstand
        double w = 1.0 / (sigma_i * sigma_i);        // Het gewicht (inverse variantie)

        double dx = schatting.x - aps[i].x;
        double dy = schatting.y - aps[i].y;
        double r = std::sqrt(dx * dx + dy * dy);
        if (r < 1e-8) r = 1e-8;

        // 2. Bereken de afgeleiden (Jacobian)
        double rx = dx / r;
        double ry = dy / r;

        // 3. Vul de matrix J^T * W * J
        // We vermenigvuldigen hier direct met het gewicht 'w'
        J[0][0] += w * rx * rx;
        J[0][1] += w * rx * ry;
        J[1][0] += w * rx * ry;
        J[1][1] += w * ry * ry;
    }

    // 4. Inverteer de matrix om de covariantiematrix te krijgen
    double det = J[0][0] * J[1][1] - J[0][1] * J[1][0];
    if (std::abs(det) < 1e-12) det = 1e-12; // Voorkom delen door nul

    // De covariantie is nu direct de inverse van J (omdat w al in J zit)
    double cov00 =  J[1][1] / det;
    double cov11 =  J[0][0] / det;
    double cov01 = -J[0][1] / det;

    return {{cov00, cov11}, cov01};
}

int test_non_linear_least_squares(const std::vector<AccessPoint> &aps1,
                                  const std::vector<double> &distances1)
{
    // Bekende locaties van Access Points (aps) en gemeten afstanden

    std::vector<Punt> aps;
    for (const auto &ap : aps1)
    {
        aps.push_back({ap.GetX(), ap.GetY()});
    }
    std::vector<double> afstanden = distances1;

    Punt initieel = {5, 5};
    Punt schatting = gradientAfdaling(aps, afstanden, initieel);

    std::cout << "Geschatte locatie: (" << schatting.x << ", " << schatting.y << ")\n";

    // Covariantie ophalen: {{cov00, cov11}, cov01}
    auto cov = berekenCovariantie(schatting, aps, afstanden);
    double c00 = cov.first.first;
    double c11 = cov.first.second;
    double c01 = cov.second;

    // Eigenwaarden berekenen voor de assen van de ellips
    double term = std::sqrt(std::pow(c00 - c11, 2) + 4 * std::pow(c01, 2));
    double lambda1 = 0.5 * (c00 + c11 + term);
    double lambda2 = 0.5 * (c00 + c11 - term);

    double chi2_95 = 5.991; // 95% betrouwbaarheid voor 2 vrijheidsgraden
    double hoofdas = std::sqrt(chi2_95 * lambda1);
    double nevenas = std::sqrt(chi2_95 * lambda2);

    // Hoek berekenen (in radialen)
    double hoek = 0.5 * std::atan2(2 * c01, c00 - c11);

    Serial.println("--- 95% Betrouwbaarheidsovaal ---");

    Serial.print("Hoofdas (lengte): ");
    Serial.println(hoofdas);

    Serial.print("Nevenas (breedte): ");
    Serial.println(nevenas);

    Serial.print("Hoek (radialen):  ");
    Serial.print(hoek);
    Serial.println(" rad");

    Serial.print("Hoek (graden):    ");
    Serial.print(hoek * 180.0 / PI); // In Arduino is PI vaak al gedefinieerd als hoofdletter
    Serial.println("°");

    for (int i = 0; i < aps.size(); ++i)
    {
        Serial.print("Punt: ");
        Serial.print(aps[i].x);
        Serial.print(",");
        Serial.print(aps[i].y);
        Serial.print(" -> Afstand: ");
        Serial.println(afstanden[i]);
    }

    return 0;
}