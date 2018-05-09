#ifndef SENSOR_H
#define SENSOR_H
#include "Settings.h"

class Sensor{
    public:
        Sensor(char _name);
        void readRGB();
        void writeRGB();
    private:
        char name;
};

#endif //SENSOR_H