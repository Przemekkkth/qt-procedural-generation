#ifndef PLANET_H
#define PLANET_H
#include <QVector>

struct Planet {
    double distance = 0.0;
    double diameter = 0.0;
    double foliage = 0.0;
    double minerals = 0.0;
    double water = 0.0;
    double gases = 0.0;
    double temperature = 0.0;
    double population = 0.0;
    bool ring = false;
    QVector<double> vMoons;
};

#endif // PLANET_H
