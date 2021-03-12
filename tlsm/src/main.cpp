#include <Arduino.h>
#include <math.h>
#include <Wire.h>

const int n_samples = 200;

#define lsm 0x6B
#define LED_PIN 13
#define BOD_RATE 9600
#define CTRL_REG1_G 0x10

#define axl 0x28
#define axh 0x29
#define ayl 0x2A
#define ayh 0x2B
#define azl 0x2C
#define azh 0x2D

#define gxl 0x18
#define gxh 0x19
#define gyl 0x1A
#define gyh 0x1B
#define gzl 0x1C
#define gzh 0x1D

#define whoami 0xF0

void toggleBlink();

void setup() {
Serial.begin(9600);
Wire.begin();
Wire.beginTransmission(lsm);
Wire.write(CTRL_REG1_G);
Wire.write(0b10100010);
Wire.endTransmission();
// for (int i = 0; i < n_samples; i++)
// {
//   if (i % 15 == 0) {
//     toggleBlink();
  
//   Wire.beginTransmission(lsm);
//   Wire.write(gyl);
//   Wire.endTransmission();
//   Wire.requestFrom(lsm, 2)
//   const int lower_g = Wire.read();
//   const int higher_g = Wire.read();
//   const int calibration = lower_g | (higher_g << 8);
//   y_g_calibration += calibration;
//   delayMicroseconds(3000);
//   }
//   y_g_calibration /= n_samples;
}

void loop() {
Wire.beginTransmission(lsm);
Wire.write(axl);
Wire.endTransmission();
Wire.requestFrom(lsm, 2);
const int raxl = Wire.read();
const int raxh = Wire.read();
int16_t ax_value = (raxh << 8) | raxl;

Serial.println(ax_value);
delay(500);
}