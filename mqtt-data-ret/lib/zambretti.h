#include <stdio.h>
#include <string.h>
#include <math.h>

enum pressure_trend
{
    FALLING,
    STEADY,
    RISING
};

#define ALTITUDE_M 12 // 8 + 4

char *lookUpTable(int z);
float pressureSeaLevel(float t, float p);
int pressureTrend(int new, int old);
int caseCalculation(int c, float p);