#ifndef SENSOR_H
#define SENSOR_H
#include "Settings.h"

class Sensor{
    public:
        Sensor(char _name, int num);
        void readRGB();
        
    private:
        void toggleLight();
        void toggleMux();
        void readRed();
        void readGreen();
        void readBlue();
        void send(char color);
        void setChannels(int num);

        char name;
        char message[LENGTH];
        
        struct RGB{
            int R;
            int G;
            int B;
        } RBG;

        struct MUX
        {
            int pin;
            bool on;
        } MUX;

        struct LIGHT{
            int pin;
            bool on;
        } light;
};

#endif //SENSOR_H