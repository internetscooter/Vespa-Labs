#include "speed.h"

Speed::Speed()
{
    Speed(0,0);
}

Speed::Speed(double ms)
{
    Speed(ms,0);
}

Speed::Speed(double ms, double c):
    lastMeasurementTime_ms(0),
    lastSpeed_ms(0)
{
    mass_kg = 220; // scooter and rider mass - move elsewhere
    speed_ms = ms;
    circumference_m = c;
    timer.start();
    currentMeasurementTime_ms = timer.elapsed();
}

double Speed::ms(void)
{
    return speed_ms;
}

double Speed::kmph(void)
{
    // seconds converted to hours / metres converted to km
    return speed_ms * 60 * 60 / 1000;
}

double Speed::rpm(void)
{
    if (circumference_m > 0) // only do this if we have a tyre circumference defined
    {
        double hertz = 1 / period_s();
        return hertz * 60; // covert hertz to rpm (seconds to minutes)
    }
    return 0;
}

double Speed::period_s(void)
{
    if (circumference_m > 0) // only do this if we have a tyre circumference defined
    {
        return circumference_m / speed_ms; // time to rotate once
    }
    return 0;
}

void Speed::set_ms(double ms) // set metres per second
{
    lastSpeed_ms = speed_ms;
    lastMeasurementTime_ms = currentMeasurementTime_ms;
    currentMeasurementTime_ms = timer.elapsed();
    speed_ms = ms;
    // calculate acceleration
    double difference = speed_ms - lastSpeed_ms;
    double acceleration_mss = difference / (currentMeasurementTime_ms - lastMeasurementTime_ms) / 1000;
    newtons = mass_kg * acceleration_mss;
    double average_ms = (speed_ms + lastSpeed_ms)/2;
    power_w = lastSpeed_ms * average_ms;
}

void Speed::set_period_s(double period_seconds) // set time for rotation
{
    if (circumference_m > 0) // only do this if we have a tyre circumference defined
    {
        double hertz = 1 / period_seconds; // work out how many times we rotate a second
        //metres_per_second = circumference_metres * hertz;
        set_ms(circumference_m * hertz);
    }
}
