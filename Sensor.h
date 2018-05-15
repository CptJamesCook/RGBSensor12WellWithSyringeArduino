#ifndef SENSOR_H
#define SENSOR_H
#include "Settings.h"

class Sensor{
    public:
        Sensor();
        void setup(char _name, int num);
        void readRGB(BLEMate2 BTModu);
        char getName();
        
    private:
        void toggleLight();
        void toggleMux();
        void readColor(char color);
        void send(char color, BLEMate2 BTModu);
        void setChannels(int num);
        void getADCValue();
        void clearADC();
        
        char name;
        char message[LENGTH];
        
        struct RGB{
            int R;
            int G;
            int B;
        } rgb;

        struct MUX
        {
            int pin;
            bool on;
        } mux;

        struct LIGHT{
            int pin;
            bool on;
        } light;
        
        // used to read adc values
        int32_t rawadcvalue;
        int32_t purgevalue;
        uint32_t processedadcvalue;
};

#endif //SENSOR_H