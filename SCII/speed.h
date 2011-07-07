#ifndef SPEED_H
#define SPEED_H
// This class covers speed and related calculations
#include <QElapsedTimer> // including keeping a track of it's own time
#include <QFile>
#include <QTextStream>

class Speed
{
public:
    Speed();
    Speed(double metres_per_second);
    Speed(double metres_per_second, double circumference_metres);

    // getters
    double ms(void);                    // speed in metres per second
    double kmph(void);                  // speed in km per hour
    double rpm(void);                   // speed in rotations per minute - requires circumference
    double period_s(void);              // time for rotation in seconds
    double mss(void)    {return acceleration_mss;}
    double kwatt(void)  {return power_w/1000;}
    double hp(void)     {return power_w/1000*1.34102209;}

    // setters
    void set_ms(double ms);                     // set metres per second
    void set_period_s(double period_seconds);     // set time for rotation

private:
    double speed_ms;                    // straight line speed
    double circumference_m;             // for rotational calcuations = zero means unset
    QElapsedTimer timer;                // this is needed to calculate time based parameters
    double lastMeasurementTime_ms;      // such as last measurement time ms
    double currentMeasurementTime_ms;   // ms
    double lastSpeed_ms;                // last speed
    double acceleration_mss;            // difference between two speeds
    double mass_kg;                     // now this is a bit silly - mass of the scooter and rider - needs to be moved elsewhere
    double newtons;                     // yes even more silly
    double power_w;                     // watt!?
    QFile *logfile;                     // for debugging
};

#endif // SPEED_H
