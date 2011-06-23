#ifndef SPEED_H
#define SPEED_H
// This class covers speed and related calculations
class Speed
{
public:
    Speed();
    Speed(double metres_per_second);
    Speed(double metres_per_second, double circumference_metres);
    // getters
    double ms(void);            // return metres per second
    double kmph(void);          // return km per hour
    double rpm(void);           // return rotations per minute - requires circumference
    double periodsec(void);     // time for rotation in seconds
    // setters
    void set_ms(double ms);                     // set metres per second
    void set_period(double period_seconds);     // set time for rotation
private:
    double metres_per_second;       // straight line speed
    double circumference_metres;    // for rotational calcuations = zero means unset
};

#endif // SPEED_H
