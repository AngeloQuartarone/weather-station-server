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
#define SUMMER 0
#define WINTER 1


char *lookUpTable(int z);
float pressureSeaLevel(float t, float p);
int pressureTrend(float new, float old);
int caseCalculation(int c, float p);
int adjustForHumidity(int forecast, float humidity);
int caseCalculationWithHumidity(int trend, float pressure, float humidity);
int adjustForSeason(int forecast, int season);
int determineSeason(int month);
int caseCalculationWithSeason(int trend, float pressure, float humidity, int month);