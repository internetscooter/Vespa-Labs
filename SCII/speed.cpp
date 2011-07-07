#include "speed.h"
#include <QtDebug>
#include <QTextStream>

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
    //mass_kg = 220; // scooter and rider mass - move elsewhere
    //mass_kg = 183.7; // Matt
    mass_kg = 229.5; // Paul
    speed_ms = ms;
    circumference_m = c;
    timer.start();
    currentMeasurementTime_ms = timer.elapsed();

    // Quick and dirty Temp file logging to capture some data
    // no error checking! Comment out when not being used
    logfile = new QFile( "vespa_speed.log" );
    logfile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream log(logfile);
    //log << "ms last, ms this, ms diff, acceleration, last time, this time, time diff, newtons, average speed, power" << "\n";
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

void Speed::set_period_s(double period_seconds) // set time for rotation
{
    if (circumference_m > 0 && period_seconds != 0) // only do this if we have a tyre circumference defined and wheels are turning
    {
        double hertz = 1 / period_seconds; // work out how many times we rotate a second
        //metres_per_second = circumference_metres * hertz;
        set_ms(circumference_m * hertz);
    }
    else
    {
        set_ms(0);
    }
}
void Speed::set_ms(double ms) // set metres per second
{
    // update info and keep last readings
    lastSpeed_ms = speed_ms;
    lastMeasurementTime_ms = currentMeasurementTime_ms;
    currentMeasurementTime_ms = timer.elapsed();
    speed_ms = ms;
    // calculate acceleration
    difference = speed_ms - lastSpeed_ms;
    acceleration_mss = difference / (currentMeasurementTime_ms - lastMeasurementTime_ms) * 1000;
    newtons = mass_kg * acceleration_mss;
    averageSpeed_ms = (speed_ms + lastSpeed_ms)/2;
    power_w = newtons * averageSpeed_ms;

    QTextStream log(logfile);
    log << lastSpeed_ms << ","
        << speed_ms << ","
        << difference << ","
        << lastMeasurementTime_ms << ","    
        << currentMeasurementTime_ms << ","
        << (currentMeasurementTime_ms - lastMeasurementTime_ms) << ","
        << acceleration_mss << ","
        << newtons  << ","
        << averageSpeed_ms << ","
        << power_w << "\n";

    qDebug() << "last time: " << lastMeasurementTime_ms;
    qDebug() << "this time: " << currentMeasurementTime_ms;
    qDebug() << "ms difference: " << difference;
    qDebug() << "time diff: " << (currentMeasurementTime_ms - lastMeasurementTime_ms) / 1000;
}


