#include "Sensor.h"

Sensor::Sensor(char _name, int num){
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

Sensor::readRGB(){
    toggleLight();
    
    readRed();
    send('R');

    readGreen();
    send('G');

    readBlue();
    send('B');

    toggleLight();
}

Sensor::toggleLight(){
    if(light.on) { // if light is on, turn it off
        light.on = false;
        digitalWrite(light.pin, LOW);
    }
    else { // Otherwise, turn it on 
        light.on = true;
        digitalWrite(light.pin, HIGH);
    }
}

Sensor::toggleMux(){
    if(MUX.on){ // if mux is on, turn it off (note: mux is active low)
        MUX.on = false;
        digitalWrite(MUX.pin, HIGH);
    }
    else { // Otherwise, turn it on
        MUX.on = true;
        digitalWrite(MUX.pin, LOW);
    }
}

// ##################################
// Well 1
// ##################################
delay(50);

digitalWrite(24, HIGH);
digitalWrite(25, LOW);
digitalWrite(26, HIGH);
digitalWrite(27, LOW);

digitalWrite(36, HIGH);
digitalWrite(30, LOW);
delay(3);
SPI.begin();
LTC2484_read(5, 0x80, &rawadcvalue);
SPI.end();
delay(50);
digitalWrite(30, HIGH);

// Clear the ADC????
SPI.begin();
LTC2484_read(5, 0x80, &purgevalue);
SPI.end();

processedadcvalue = (uint32_t)(rawadcvalue & 0x1fffffe0);
processedadcvalue = processedadcvalue >> 5; // Might be shifting it to the left one too many bits (>> 4)

Serial.print("RED 1: ");
Serial.print("    ");
Serial.println(processedadcvalue, DEC);

message[0] = 'R';
message[1] = 'a';
message[2] = ',';
dtostrf(processedadcvalue, sizeof(processedadcvalue), 0, &message[3]);
Serial.println(message);
BTModu.sendData(message);
memset(&message[0], 0, sizeof(message));

delay(50);

rawadcvalue = 0;
processedadcvalue = 0;

delay(50);

digitalWrite(24, HIGH);
digitalWrite(25, LOW);
digitalWrite(26, HIGH);
digitalWrite(27, HIGH);

digitalWrite(30, LOW);
delay(3);
SPI.begin();
LTC2484_read(5, 0x80, &rawadcvalue);
SPI.end();
delay(50);
digitalWrite(30, HIGH);

SPI.begin();
LTC2484_read(5, 0x80, &purgevalue);
SPI.end();

rawadcvaluepart1 = (rawadcvalue & 0x1fffffe0);

processedadcvalue = (uint32_t)rawadcvaluepart1;

processedadcvalue = processedadcvalue >> 5;

Serial.print("GREEN 1: ");
Serial.print("    ");
Serial.println(processedadcvalue, DEC);

message[0] = 'G';
message[1] = 'a';
message[2] = ',';
dtostrf(processedadcvalue, sizeof(processedadcvalue), 0, &message[3]);
Serial.println(message);
BTModu.sendData(message);
memset(&message[0], 0, sizeof(message));

delay(50);

rawadcvalue = 0;
processedadcvalue = 0;

delay(50);

digitalWrite(24, LOW);
digitalWrite(25, LOW);
digitalWrite(26, HIGH);
digitalWrite(27, LOW);

digitalWrite(30, LOW);
delay(3);
SPI.begin();
LTC2484_read(5, 0x80, &rawadcvalue);
SPI.end();
delay(50);
digitalWrite(30, HIGH);
digitalWrite(36, LOW);

SPI.begin();
LTC2484_read(5, 0x80, &purgevalue);
SPI.end();

rawadcvaluepart1 = (rawadcvalue & 0x1fffffe0);

processedadcvalue = (uint32_t)rawadcvaluepart1;

processedadcvalue = processedadcvalue >> 5;

Serial.print("BLUE 1: ");
Serial.print("    ");
Serial.println(processedadcvalue, DEC);
Serial.println("    ");

message[0] = 'B';
message[1] = 'a';
message[2] = ',';
dtostrf(processedadcvalue, sizeof(processedadcvalue), 0, &message[3]);
Serial.println(message);
BTModu.sendData(message);
memset(&message[0], 0, sizeof(message));

delay(50);

rawadcvalue = 0;
processedadcvalue = 0;