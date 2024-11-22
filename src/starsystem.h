#ifndef STARSYSTEM_H
#define STARSYSTEM_H
#include <QtTypes>
#include <QColor>
#include "planet.h"

class StarSystem
{
public:
    StarSystem(quint32 x, quint32 y, bool bGenerateFullSystem = false);

    QVector<Planet> vPlanets;
    bool		starExists = false;
    double		starDiameter = 0.0f;
    QColor	starColour = Qt::white;
private:
    quint32 nProcGen = 0;
    quint32 rnd();
    double rndDouble(double min, double max);

    int rndInt(int min, int max);
};

#endif // STARSYSTEM_H
