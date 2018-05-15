#include "Sensor.h"

Sensor::Sensor() {
  // do nothing
}

void Sensor::setup(char _name, int num) {
    name = _name;

    // Setup the MUX value. This value determines the MUX used to access the sensor.
    mux.on = false;
    if((num == 0) || (num == 1) || (num == 2) || (num == 3)) {
        mux.pin = MUX3;
    }

    else if((num == 4) || (num == 5) || (num == 6) || (num == 7)) {
        mux.pin = MUX2;
    }
    
    else if ((num == 8) || (num == 9) || (num == 10) || (num == 11)) {
        mux.pin = MUX1;
    }

    // Setup the RGB values. These values map how to set up the channel to access the proper RGB sensor.
    if((num == 0) || (num==4) || (num==8)) {
        rgb.R = 10;
        rgb.G = 11;
        rgb.B = 2;
    }

    else if ((num == 1) || (num == 5) || (num == 9)) {
        rgb.R = 1;
        rgb.G = 0;
        rgb.B = 5;
    }

    else if ((num == 2) || (num == 6) || (num == 10)) {
        rgb.R = 4;
        rgb.G = 3;
        rgb.B = 8;
    }
    
    else if ((num == 3) || (num == 7) || (num == 11)) {
        rgb.R = 7;
        rgb.G = 6;
        rgb.B = 9;
    }

    else {// do nothing
    }

    // Setup the light variable
    light.on = false;
    switch(num){
        case 0:
            light.pin = LED0;
            break;
        case 1:
            light.pin = LED1;
            break;
        case 2:
            light.pin = LED2;
            break;
        case 3:
            light.pin = LED3;
            break;
        case 4:
            light.pin = LED4;
            break;
        case 5:
            light.pin = LED5;
            break;
        case 6:
            light.pin = LED6;
            break;
        case 7:
            light.pin = LED7;
            break;
        case 8:
            light.pin = LED8;
            break;
        case 9:
            light.pin = LED9;
            break;
        case 10:
            light.pin = LED10;
            break;
        case 11:
            light.pin = LED11;
            break;
        default:
            break;
    }
}

void Sensor::readRGB(BLEMate2 BTModu) {
    toggleLight();
    
    delay(3);
    readColor('R');
    send('R', BTModu);
    delay(3);
    
    delay(3);
    readColor('G');
    send('G', BTModu);
    delay(3);
    
    delay(3);
    readColor('B');
    send('B', BTModu);
    delay(3);

    toggleLight();
    
    rawadcvalue = 0;
    processedadcvalue = 0;
}

char Sensor::getName() {
  return name;
}

void Sensor::toggleLight() {
    if(light.on) { // if light is on, turn it off
        light.on = false;
        digitalWrite(light.pin, LOW);
    }
    else { // Otherwise, turn it on 
        light.on = true;
        digitalWrite(light.pin, HIGH);
    }
}

void Sensor::toggleMux() {
    if(mux.on){ // if mux is on, turn it off (note: mux is active low)
        mux.on = false;
        digitalWrite(mux.pin, HIGH);
    }
    else { // Otherwise, turn it on
        mux.on = true;
        digitalWrite(mux.pin, LOW);
    }
}

void Sensor::readColor(char color) {
    switch(color){
        case 'R':
            setChannels(rgb.R);
            break;
        case 'G':
            setChannels(rgb.G);
            break;
        case 'B':
            setChannels(rgb.B);
            break;
        default:
            break;
    }
    toggleMux();
    delay(3);
    getADCValue();
    delay(3);
    clearADC();
    toggleMux();
}

void Sensor::send(char color, BLEMate2 BTModu) {
    message[0] = color;
    message[1] = name;
    message[2] = ',';
    dtostrf(processedadcvalue, sizeof(processedadcvalue), 0, &message[3]);
    Serial.println(message);
    BTModu.sendData(message);
    memset(&message[0], 0, sizeof(message)); // deletes message
}

void Sensor::setChannels(int channels) { // There's probably a smarter way to do this...
    switch(channels) {
        case 0:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, LOW);
            digitalWrite(CH0, LOW);
            break;
        case 1:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, LOW);
            digitalWrite(CH0, HIGH);
            break;
        case 2:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, HIGH);
            digitalWrite(CH0, LOW);
            break;
        case 3:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, HIGH);
            digitalWrite(CH0, HIGH);
            break;
        case 4:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, HIGH);
            digitalWrite(CH1, LOW);
            digitalWrite(CH0, LOW);
            break;
        case 5:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, HIGH);
            digitalWrite(CH1, LOW);
            digitalWrite(CH0, HIGH);
            break;
        case 6:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, HIGH);
            digitalWrite(CH1, HIGH);
            digitalWrite(CH0, LOW);
            break;
        case 7:
            digitalWrite(CH2, LOW);
            digitalWrite(CH3, HIGH);
            digitalWrite(CH1, HIGH);
            digitalWrite(CH0, HIGH);
            break;
        case 8:
            digitalWrite(CH2, HIGH);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, LOW);
            digitalWrite(CH0, LOW);
            break;
        case 9:
            digitalWrite(CH2, HIGH);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, LOW);
            digitalWrite(CH0, HIGH);
            break;
        case 10:
            digitalWrite(CH2, HIGH);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, HIGH);
            digitalWrite(CH0, LOW);
            break;
        case 11:
            digitalWrite(CH2, HIGH);
            digitalWrite(CH3, LOW);
            digitalWrite(CH1, HIGH);
            digitalWrite(CH0, HIGH);
            break;
        default:
            break;        
    }
}

void Sensor::getADCValue() {
    SPI.begin();
    LTC2484_read(5, 0x80, &rawadcvalue);
    SPI.end();
    processedadcvalue = (uint32_t)(rawadcvalue & 0x1fffffe0);
    processedadcvalue = processedadcvalue >> 5; // Might be shifting it to the left one too many bits (>> 4)
}

void Sensor::clearADC() {
    SPI.begin();
    LTC2484_read(5, 0x80, &purgevalue);
    SPI.end();
}
