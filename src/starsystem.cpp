#include "starsystem.h"
#include "utils.h"

StarSystem::StarSystem(quint32 x, quint32 y, bool bGenerateFullSystem)
{
    // Set seed based on location of star system
    nProcGen = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    // Not all locations contain a system
    starExists = (rndInt(0, 20) == 1);
    if (!starExists) {
        return;
    }

    // Generate Star
    starDiameter = rndDouble(10.0, 20.0);
    starColour = COLOR_STYLE::StarColours[rndInt(0, 8)];

    // When viewing the galaxy map, we only care about the star
    // so abort early
    if (!bGenerateFullSystem) {
        return;
    }

    // If we are viewing the system map, we need to generate the
    // full system

    // Generate Planets
    double dDistanceFromStar = rndDouble(60.0, 200.0);
    int nPlanets = rndInt(0, 10);

    for (int i = 0; i < nPlanets; i++)
    {
        Planet p;
        p.distance = dDistanceFromStar;
        dDistanceFromStar += rndDouble(20.0, 200.0);
        p.diameter = rndDouble(4.0, 20.0);

        // Could make temeprature a function of distance from star
        p.temperature = rndDouble(-200.0, 300.0);

        // Composition of planet
        p.foliage = rndDouble(0.0, 1.0);
        p.minerals = rndDouble(0.0, 1.0);
        p.gases = rndDouble(0.0, 1.0);
        p.water = rndDouble(0.0, 1.0);

        // Normalise to 100%
        double dSum = 1.0 / (p.foliage + p.minerals + p.gases + p.water);
        p.foliage *= dSum;
        p.minerals *= dSum;
        p.gases *= dSum;
        p.water *= dSum;

        // Population could be a function of other habitat encouraging
        // properties, such as temperature and water
        p.population = std::max(rndInt(-5000000, 20000000), 0);

        // 10% of planets have a ring
        p.ring = rndInt(0, 10) == 1;

        // Satellites (Moons)
        int nMoons = std::max(rndInt(-5, 5), 0);
        for (int n = 0; n < nMoons; n++)
        {
            // A moon is just a diameter for now, but it could be
            // whatever you want!
            p.vMoons.push_back(rndDouble(1.0, 5.0));
        }

        // Add planet to vector
        vPlanets.push_back(p);
    }
}

quint32 StarSystem::rnd()
{
    nProcGen += 0xe120fc15;
    quint64 tmp;
    tmp = static_cast<quint64>(nProcGen) * 0x4a39b70d;
    quint32 m1 = (tmp >> 32) ^ tmp;
    tmp = static_cast<quint64>(m1) * 0x12fad5c9;
    quint32 m2 = (tmp >> 32) ^ tmp;
    return m2;
}

double StarSystem::rndDouble(double min, double max)
{
    return ((double)rnd() / (double)(0x7FFFFFFF)) * (max - min) + min;
}

int StarSystem::rndInt(int min, int max)
{
    return (rnd() % (max - min)) + min;
}
