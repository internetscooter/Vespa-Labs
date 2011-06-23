#include "speed.h"

Speed::Speed():
    metres_per_second(0),
    circumference_metres(0)
{
}

Speed::Speed(double ms):
    circumference_metres(0)
{
    metres_per_second = ms;
}

Speed::Speed(double ms, double c)
{
    metres_per_second = ms;
    circumference_metres = c;
}

double Speed::ms(void)
{
    return metres_per_second;
}

double Speed::kmph(void)
{
    // seconds converted to hours / metres converted to km
    return metres_per_second * 60 * 60 / 1000;
}

double Speed::rpm(void)
{
    if (circumference_metres > 0) // only do this if we have a tyre circumference defined
    {
        double hertz = 1 / periodsec();
        return hertz * 60; // covert hertz to rpm (seconds to minutes)
    }
    return 0;
}

double Speed::periodsec(void)
{
    if (circumference_metres > 0) // only do this if we have a tyre circumference defined
    {
        return circumference_metres / metres_per_second; // time to rotate once
    }
    return 0;
}

void Speed::set_ms(double ms) // set metres per second
{
    metres_per_second = ms;
}

void Speed::set_period(double period_seconds) // set time for rotation
{
    if (circumference_metres > 0) // only do this if we have a tyre circumference defined
    {
        double hertz = 1 / period_seconds; // work out how many times we rotate a second
        metres_per_second = circumference_metres * hertz;
    }
}
