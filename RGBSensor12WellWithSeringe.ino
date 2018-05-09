#include "Sensor.h"
#include "Settings.h"

int32_t rawadcvalue, rawadcvaluepart1;
int32_t purgevalue;
uint32_t processedadcvalue;

char message[LENGTH];

int bluetoothTx = A11;
int bluetoothRx = A12;
bool testrunning;
SoftwareSerial bluetoothSerial(bluetoothTx, bluetoothRx);
BLEMate2 BTModu(&bluetoothSerial);

Sensor sensors[12];

void setup() {
  // We initialize serial communication, setup the SPI pins and lighting/power pins as outputs, and initialize the lights to OFF
  Serial.begin(9600);
  bluetoothSerial.begin(9600);
  
  //Pins that go to the light detector
  initializeLightDetectorPins();

  // Pins that go to the pump and other stuffskis?
  initializeOtherPins();

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  testrunning = false;

  pinMode(Enable, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(Limit1, INPUT);
  pinMode(Limit2, INPUT);

  while(digitalRead(Limit1) == HIGH && millis()<=10000) { 
    digitalWrite(in1, HIGH);
    analogWrite(Enable, PWM);
    Serial.println("setting");
  }
  digitalWrite(in1, LOW);
  Serial.println("set");

  setupBluetooth();
}


void loop() {

  if (testrunning) {
    Serial.println("Starting Test");
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

    SPI.begin();
    LTC2484_read(5, 0x80, &purgevalue);
    SPI.end();

    rawadcvaluepart1 = (rawadcvalue & 0x1fffffe0);

    processedadcvalue = (uint32_t)rawadcvaluepart1;

    processedadcvalue = processedadcvalue >> 5;

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
  }
  // buffer for RCV data
  static uint8_t fullBufferByte[LENGTH];

  // buffer for bluetooth Data
  static uint8_t inputBuffer[LENGTH];

  // mode = 1 impedance over time
  // mode = 2 frequency sweep
  byte mode = 0;

  //RCV buffer index pointer
  static uint8_t iFullBuffer = 0;

  // bluetooth Serail buffer index pointer
  static uint8_t iInputBuffer = 0;

  uint8_t electrode;

  static long lastRXTime = millis();

  // use union to convert byte array to uint32_t
  union fourByte {
    uint32_t dataFrequency;
    uint8_t dataByte[4];
  };
  union fourByte dataVal;

  // if more than 1000
  //  milliseconds has elapsed since last receive
  // parse the command from Iphone apps

  if (lastRXTime + 1000 < millis()) {
    // there is data in the RCV buffer
    if (iFullBuffer != 0) {

      // first byte is the mode
      Serial.print("byte buffer:");
      Serial.println(fullBufferByte[0], DEC);
      mode = fullBufferByte[0];

      iFullBuffer = 0;
    }
  }

  iInputBuffer = 0;
  // read data from bluetooth
  while (bluetoothSerial.available() > 0) {
    inputBuffer[iInputBuffer] = (uint8_t)bluetoothSerial.read();
    iInputBuffer++;
    lastRXTime = millis();
  }

  // check to see if the string
  // and data looks like this:
  // RCV=20 char max msg\n\r

  // if yes copy it to fullBufferByte array

  if (strncmp((char *)&inputBuffer[iInputBuffer - 2], "\n\r", 2) == 0) {

    if (strncmp((char *)&inputBuffer[0], "RCV=", 4) == 0) {
      Serial.println("This is a RCV");

      // copy to RCV buffer
      uint8_t i;
      i = 4;
      iInputBuffer = iInputBuffer - 2;
      while (i < iInputBuffer) {
        fullBufferByte[iFullBuffer] = inputBuffer[i];
        iFullBuffer++;
        i++;
      }
      iInputBuffer = 0;
    }
    else {
      iInputBuffer = 0;
    }
  }

  switch (mode) {
  case 66: //For beginning the stream (ASCII B)
    testrunning = 1;
    mode = 0;
    break;

  case 83: // For stopping the stream (ASCII S)
    testrunning = 0;
    mode = 0;
    break;

  case 77: // For retracting the syringe pump (ASCII M)
    RETRACT();
    break;

  case 79: //For returning the syringe pump (ASCII O)
    RETURN();
    break;

  default:
    break;
  }
  delay(100);
}

void initializeLightDetectorPins() {
  pinMode(CSR, OUTPUT);
  pinMode(CSG, OUTPUT);
  pinMode(CSB, OUTPUT);

  pinMode(CH0, OUTPUT);
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  pinMode(CH3, OUTPUT);
  pinMode(MUX1, OUTPUT);
  pinMode(MUX2, OUTPUT);
  pinMode(MUX3, OUTPUT);

  digitalWrite(CSR, HIGH);
  digitalWrite(CSG, HIGH);
  digitalWrite(CSB, HIGH);

  digitalWrite(CH0, HIGH);
  digitalWrite(CH1, HIGH);
  digitalWrite(CH2, LOW);
  digitalWrite(CH3, HIGH);

  digitalWrite(MUX1, HIGH);
  digitalWrite(MUX2, HIGH);
  digitalWrite(MUX3, HIGH);
}

void initializeOtherPins() {
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);

  digitalWrite(34, LOW);
  digitalWrite(35, LOW);
  digitalWrite(36, LOW);
  digitalWrite(37, LOW);
  digitalWrite(38, LOW);
  digitalWrite(39, LOW);
  digitalWrite(40, LOW);
  digitalWrite(41, LOW);
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(45, LOW);
  digitalWrite(46, LOW);
}

void setupBluetooth() {
  // Regarding function return values: most functions that interact with the
  //  BC118 will return BLEMate2::opResult values. The possible values here
  //  are:
  //  REMOTE_ERROR - No remote devices exist.
  //  INVALID_PARAM - You've called the function with an invalid parameter.
  //  TIMEOUT_ERROR - The BC118 failed to respond to the command in a timely
  //                   manner; timely is redefined for each command.
  //  MODULE_ERROR - The BC118 didn't like the command string it received.
  //                  This will probably only occur when you attempt to send
  //                  commands and parameters outside the built-ins. 
  //  SUCCESS - What it says.
  
  // Reset is a blocking function which gives the BC118 a few seconds to reset.
  //  After a reset, the module will return to whatever settings are in
  //  non-volatile memory. One other *super* important thing it does is issue
  //  the "SCN OFF" command after the reset is completed. Why is this important?
  //  Because if the device is in central mode, it *will* be scanning on reset.
  //  No way to change that. The text traffic generated by the scanning will
  //  interfere with the firmware on the Arduino properly identifying response
  //  strings from the BC118.
  if (BTModu.reset() != BLEMate2::SUCCESS)
  {
    Serial.println("Module reset error!");
    while (1);
  }

  // restore() resets the module to factory defaults; you'll need to perform
  //  a writeConfig() and reset() to make those settings take effect. We don't
  //  do that automatically because there may be things the user wants to
  //  change before committing the settings to non-volatile memory and
  //  resetting.
  if (BTModu.restore() != BLEMate2::SUCCESS)
  {
    Serial.println("Module restore error!");
    while (1);
  }
  // writeConfig() stores the current settings in non-volatile memory, so they
  //  will be in place on the next reboot of the module. Note that some, but
  //  not all, settings changes require a reboot. It's probably in general best
  //  to write/reset when changing anything.
  if (BTModu.writeConfig() != BLEMate2::SUCCESS)
  {
    Serial.println("Module write config error!");
    while (1);
  }
  // One more reset, to make the changes take effect.
  if (BTModu.reset() != BLEMate2::SUCCESS)
  {
    Serial.println("Second module reset error!");
    while (1);
  }

  // The default settings are good enough for the peripheral example; just to
  //  be on the safe side, we'll check the amICentral() function and do a r/w/r
  //  if we're in central mode instead of peripheral mode.

  boolean inCentralMode = false;
  // A word here on amCentral: amCentral's parameter is passed by reference, so
  //  the answer to the question "am I in central mode" is handed back as the
  //  value in the boolean passed to it when it is called. The reason for this
  //  is the allow the user to check the return value and determine if a module
  //  error occurred: should I trust the answer or is there something larger
  //  wrong than merely being in the wrong mode?
  BTModu.amCentral(inCentralMode); 
  if (inCentralMode)
  {
    BTModu.BLEPeripheral();
    BTModu.BLEAdvertise();
  }

  // There are a few more advance settings we'll probably, but not definitely,
  //  want to tweak before we reset the device.

  // The CCON parameter will enable advertising immediately after a disconnect.
  BTModu.stdSetParam("CCON", "ON");
  // The ADVP parameter controls the advertising rate. Can be FAST or SLOW.
  BTModu.stdSetParam("ADVP", "FAST");
  // The ADVT parameter controls the timeout before advertising stops. Can be
  //  0 (for never) to 4260 (71min); integer value, in seconds.
  BTModu.stdSetParam("ADVT", "0");
  // The ADDR parameter controls the devices we'll allow to connect to us.
  //  All zeroes is "anyone".
  BTModu.stdSetParam("ADDR", "000000000000");

  BTModu.writeConfig();
  BTModu.reset();
  
  // We're set up to allow anything to connect to us now.
}

void RETRACT() {//syringe makes a vacuum 
  while(digitalRead(Limit2) == HIGH){
  digitalWrite(in2, HIGH);
  analogWrite(Enable, PWM);
  Serial.println("Retracting");
  }
  digitalWrite(in2, LOW);
  Serial.println("Retracted");
}

void RETURN() {//returns to original position
   while(digitalRead(Limit1)== HIGH){
  digitalWrite(in1, HIGH);
  analogWrite(Enable, PWM);
    Serial.println("Returning");
   }
  digitalWrite(in1, LOW);
  Serial.println("Returned");
}

