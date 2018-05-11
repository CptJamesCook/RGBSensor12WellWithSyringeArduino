#include "Sensor.h"

Sensor::Sensor(char _name, int num) {
    name = _name;

    // Setup the MUX value. This value determines the MUX used to access the sensor.
    this.MUX.on = false;
    if((num == 0) || (num == 1) || (num == 2) || (num == 3)) {
        this.MUX.pin = MUX3;
    }

    else if((num == 4) || (num == 5) || (num == 6) || (num == 7)) {
        this.MUX.pin = MUX2;
    }
    
    else if ((num == 8) || (num == 9) || (num == 10) || (num == 11)) {
        this.MUX.pin = MUX1;
    }

    // Setup the RGB values. These values map how to set up the channel to access the proper RGB sensor.
    if((num == 0) || (num==4) || (num==8)) {
        this.RBG.R = 10;
        this.RBG.G = 11;
        this.RBG.B = 2;
    }

    else if ((num == 1) || (num == 5) || (num == 9)) {
        this.RBG.R = 1;
        this.RBG.G = 0;
        this.RBG.B = 5;
    }

    else if ((num == 2) || (num == 6) || (num == 10)) {
        this.RBG.R = 4;
        this.RBG.G = 3;
        this.RBG.B = 8;
    }
    
    else if ((num == 3) || (num == 7) || (num == 11)) {
        this.RBG.R = 7;
        this.RBG.G = 6;
        this.RBG.B = 9;
    }

    else {// do nothing
    }

    // Setup the light variable
    light.on = false;
    switch(num){
        case 0:
            ligth.pin = LED0;
            break;
        case 1:
            ligth.pin = LED1;
            break;
        case 2:
            ligth.pin = LED2;
            break;
        case 3:
            ligth.pin = LED3;
            break;
        case 4:
            ligth.pin = LED4;
            break;
        case 5:
            ligth.pin = LED5;
            break;
        case 6:
            ligth.pin = LED6;
            break;
        case 7:
            ligth.pin = LED7;
            break;
        case 8:
            ligth.pin = LED8;
            break;
        case 9:
            ligth.pin = LED9;
            break;
        case 10:
            ligth.pin = LED10;
            break;
        case 11:
            ligth.pin = LED11;
            break;
        default:
            break;
    }
}

Sensor::readRGB() {
    toggleLight();
    
    delay(3);
    readColor('R');
    send('R');
    delay(3);
    
    delay(3);
    readColor('G');
    send('G');
    delay(3);
    
    delay(3);
    readColor('B');
    send('B');
    delay(3);

    toggleLight();
    
    rawadcvalue = 0;
    processedadcvalue = 0;
}

Sensor::toggleLight() {
    if(light.on) { // if light is on, turn it off
        light.on = false;
        digitalWrite(light.pin, LOW);
    }
    else { // Otherwise, turn it on 
        light.on = true;
        digitalWrite(light.pin, HIGH);
    }
}

Sensor::toggleMux() {
    if(MUX.on){ // if mux is on, turn it off (note: mux is active low)
        MUX.on = false;
        digitalWrite(MUX.pin, HIGH);
    }
    else { // Otherwise, turn it on
        MUX.on = true;
        digitalWrite(MUX.pin, LOW);
    }
}

Sensor::readColor(char color) {
    switch(color){
        case 'R':
            setChannels(this.RGB.R);
            break;
        case 'G':
            setChannels(this.RGB.G);
            break;
        case 'B':
            setChannels(this.RGB.B);
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

Sensor::send(char color) {
    message[0] = color;
    message[1] = name;
    message[2] = ',';
    dtostrf(processedadcvalue, sizeof(processedadcvalue), 0, &message[3]);
    Serial.println(message);
    BTModu.sendData(message);
    memset(&message[0], 0, sizeof(message)); // deletes message
}

Sensor::setChannels(int channels) { // There's probably a smarter way to do this...
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

Sensor::getADCValue() {
    SPI.begin();
    LTC2484_read(5, 0x80, &rawadcvalue);
    SPI.end();
    processedadcvalue = (uint32_t)(rawadcvalue & 0x1fffffe0);
    processedadcvalue = processedadcvalue >> 5; // Might be shifting it to the left one too many bits (>> 4)
}

Sensor::clearADC() {
    SPI.begin();
    LTC2484_read(5, 0x80, &purgevalue);
    SPI.end();
}
