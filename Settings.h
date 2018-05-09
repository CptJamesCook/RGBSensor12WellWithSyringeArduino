#ifndef SETTINGS_H

#include <SPI.h>
#include <Arduino.h>
#include "LTC2484.h"
#include "LTC24XX_general.h"
#include <SoftwareSerial.h>
#include <SparkFunBLEMate2.h>

#define REPEATS 1
#define LENGTH 25
#define ELENO 8
#define TIME_LIMIT 120000

#define Enable A0
#define in1 A1
#define in2 A2
#define Limit1 A3
#define Limit2 A4
#define PWM 160

#define MUX1 28
#define MUX2 29
#define MUX3 30
#define CH0 27
#define CH1 26
#define CH2 24
#define CH3 25
#define CSR 7
#define CSG 6
#define CSB 5

#endif // SETTINGS_H