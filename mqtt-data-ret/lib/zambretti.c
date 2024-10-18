/**
 * @author: Angelo Quartarone
 * @date: 22/06/2023
 * @version: 1.0
 * Language: C
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "zambretti.h"



/**
 * Calculates the pressure trend over a fixed period.
 * The trend can be rising, falling or steady.
 * @param new newest pressure data
 * @param old oldest pressure data
 */
int pressureTrend(int new, int old)
{
    int act_press_trnd = -1;

    if (new - old < -1.6)
    {
        act_press_trnd = FALLING;
    }
    else if (new - old > 1.6)
    {
        act_press_trnd = RISING;
    }
    else
    {
        act_press_trnd = STEADY;
    }

    return act_press_trnd;
}

/**
 * Calculates pressure at sea level.
 * @param t temperature
 * @param p pressure
 */
float pressureSeaLevel(float t, float p)
{
    float x = 0;
    // float y = 0;
    x = (0.0065 * ALTITUDE_M) / (t + (0.0065 * ALTITUDE_M) + 273.15);
    x = powf((1 - x), (-5.257));
    return (x * p);
}

/**
 * Apply the right formula in base of the pressure trend.
 * @param c actual pressure trend
 * @param p pressure at sea level
 */
int caseCalculation(int c, float p)
{
    float x = 0;
    switch (c)
    {
    case FALLING:
        x = 127 - (0.12 * p);
        break;
    case STEADY:
        x = 144 - (0.13 * p);
        break;
    case RISING:
        x = 185 - (0.16 * p);
        break;
    default:
        return -1;
    }

    return (int)roundf(x);
}

/**
 * Return the string that describes weather forecast.
 * @param z result of "caseCalculation" function.
 */
char *lookUpTable(int z)
{
    switch (z)
    {
    case 1:
        return "Settled Fine";
        break;
    case 2:
        return "Fine Weather";
        break;
    case 3:
        return "Fine, Becoming Less Settled";
        break;
    case 4:
        return "Fairly Fine, Showery Later";
        break;
    case 5:
        return "Showery, Becoming More Unsettled";
        break;
    case 6:
        return "Unsettled, Rain Later";
        break;
    case 7:
        return "	Rain at Times, Worse Later";
        break;
    case 8:
        return "Rain at Times, Becoming Very Unsettled";
        break;
    case 9:
        return "Very Unsettled, Rain";
        break;
    case 10:
        return "Settled Fine";
        break;
    case 11:
        return "Fine Weather";
        break;
    case 12:
        return "Fine, Possibly Showers";
        break;
    case 13:
        return "Fairly Fine, Showers Likely";
        break;
    case 14:
        return "Showery, Bright Intervals";
        break;
    case 15:
        return "Changeable, Some Rain";
        break;
    case 16:
        return "Unsettled, Rain at Times";
        break;
    case 17:
        return "Rain at Frequent Intervals";
        break;
    case 18:
        return "Very Unsettled, Rain";
        break;
    case 19:
        return "Stormy, Much Rain";
        break;
    case 20:
        return "Settled Fine";
        break;
    case 21:
        return "Fine Weather";
        break;
    case 22:
        return "Becoming Fine";
        break;
    case 23:
        return "Fairly Fine, Improving";
        break;
    case 24:
        return "Fairly Fine, Possibly Showers Early";
        break;
    case 25:
        return "Showery Early, Improving";
        break;
    case 26:
        return "Changeable, Mending";
        break;
    case 27:
        return "Rather Unsettled, Clearing Later";
        break;
    case 28:
        return "Unsettled, Probably Improving";
        break;
    case 29:
        return "Unsettled, Short Fine Intervals";
        break;
    case 30:
        return "Very Unsettled, Finer at Times";
        break;
    case 31:
        return "Stormy, Possibly Improving";
        break;
    case 32:
        return "Stormy, Much Rain";
        break;
    default:
        return "Error in look up table";
    }
}