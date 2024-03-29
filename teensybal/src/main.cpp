#include <Arduino.h>

#include <math.h>
#include <Wire.h>
#include <regsNpins.h>

IntervalTimer myTimer;
void myISR();


// Number of samples taken to calibrate the gyroscope
const int n_samples = 1000;

// If the angle is whithin this window, then start balancing
const double balance_window = 0.5;

// microseconds before next pid tick
const int loop_time_length = 2000;

// pid params
constexpr double pgain = 25;//15;
constexpr double igain = 0;//.5;//1.5; 
constexpr double dgain = -45;//45;//10;

//moving average filter setup
const int points = 1;
int readArray[points];
int sum = 0, movingAvg = 0, readIndex = 0;

const int serialSpacer = 100;
int serialCounter = 0; 

// UTIL
constexpr double to_deg = 180.0 / M_PI;

// GLOBALS

double gangle, adjuster, pid_i, pidde, y_g_calibration, output;
byte balance;
int stepr = 0,
    pulsecountr = 0,
    pulsememr = 0;
int stepl = 0,
    pulsecountl = 0,
    pulsememl = 0;

unsigned long loop_time;

void toggleBlink();
double calc_mot(double output);

void setup(){  ////////////////////////////////////////////////////////////////////////
  Serial.begin(9600);
  Serial1.begin(9600);
  Wire.begin();
  myTimer.begin(myISR,20);
  // set i2c clock frequency
  //TWBR = 12;

  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(13, OUTPUT);
  PORTD &= ~(1<<6);
  // configure device
  Wire.beginTransmission(lsm);
  Wire.write(CTRL_REG1_G);
  Wire.write(0b10100010);
  Wire.endTransmission();

  for (int i = 0; i < n_samples; i++)
  {
    if (i % 15 == 0) {
      toggleBlink();
    }

    Wire.beginTransmission(lsm);
    Wire.write(gyl);
    Wire.endTransmission();
    Wire.requestFrom(lsm, 2);

    const int lower_g = Wire.read();
    const int higher_g = Wire.read();
    const int calibration = lower_g | (higher_g << 8);
    y_g_calibration += calibration;
    delayMicroseconds(3000);
  }
  y_g_calibration /= n_samples;
  loop_time = micros() + loop_time_length;

  for (int i=0; i < points; i++){
    readArray[i] =0;
  }
}

void loop(){ //////////////////////////////////////////////////////////////////////
  if(serialCounter >= serialSpacer){
    serialCounter = 0;
  }
  serialCounter++;

  PORTD &= ~(1<<7);
  if (loop_time > micros())
    return;
  loop_time += loop_time_length;
  PORTD |= (1<<7);
  // read from accelerometer
  Wire.beginTransmission(lsm);
  Wire.write(axl);
  Wire.endTransmission();
  Wire.requestFrom(lsm, 2);
  const int raxl = Wire.read();
  const int raxh = Wire.read();
  int16_t ax_value = (raxh << 8) | raxl;
  //Serial.println(ax_value);
  double ax = -ax_value/8200.0;
  ax = ax + 0.01;
  if (ax > 1.0) {
    ax = 1.0;
  }
  else if (ax < -1.0)
  {
    ax = -1.0;
  }
  Serial1.println(ax);
  Serial1.print(serialCounter); 

  // read from gyroscope
  Wire.beginTransmission(lsm);
  Wire.write(gyl);
  Wire.endTransmission();
  Wire.requestFrom(lsm, 2);
  const int rgyl = Wire.read();
  const int rgyh = Wire.read();
  int16_t gy_value = (rgyh << 8) | rgyl;

  double gy = gy_value - y_g_calibration;

  /*for(i=0; i<9; i++)){
    
  }
  /*
  Serial.println(ax);
  Serial.println(gy);
  Serial.println();
  delay(20);
  //*/

  double aangle = asin(ax) * to_deg;

  if (balance == 0 && aangle > -balance_window && aangle < balance_window)
  {
    gangle = aangle;
    balance = 1;
  }

  //enable motors only when balancing to save battery
  if(balance == 1){
    PORTD &= ~(1<<6);
  }
  else PORTD |= (1<<6);

  gangle += gy * 0.000031;
  gangle = gangle * 0.9996 + aangle * 0.0004;

  //implement moving average
  sum = sum - readArray[readIndex];
  readArray[readIndex] = gangle;
  sum = sum + readArray[readIndex];
  readIndex = readIndex + 1;
  if(readIndex >= points){
    readIndex = 0;
  }
  movingAvg = sum/points;


  double usedAngle = gangle;
//  double usedAngle = movingAvg;

  //pid
  double err = usedAngle - adjuster;
  pid_i += igain * err;
  const double pid_p = pgain * err;
  double pid_d = dgain * (err - pidde);
  
  double output = pid_p + pid_i + pid_d;

  // if (output > 1800)
  //   output = 1800;
  // else if (output < -1800)
  //   output = -1800;

  pidde = err;

  // if (output < 3 && output > -3)
  // {
  //   output = 0;
  // }

  if (gangle < -30 || gangle > 30 || balance == 0)
  {
    output = 0;
    pid_i = 0;
    balance = 0;
    adjuster = 0;
  }

  /*
  Serial.println(output);
  Serial.println();
  delay(20);
  //*/

  // if (output < 0)
  //   adjuster += 0.015;
  // if (output > 0)
  //   adjuster -= 0.015;

  double motl = calc_mot(output);
  double motr = motl; // calc_mot(output);

  noInterrupts();
  stepl = motl;
  stepr = motr;
  interrupts();

}

inline double calc_mot(double output) {
  double mot;

  if (output > 0)
    output = 203 - 2750 / (output + 9);
  else if (output < 0)
    output = -203 - 2750 / (output - 9);

  if (output > 0)
    mot = 200 - output;
  else if (output < 0)
    mot = -200 - output;
  else
    mot = 0;
  return mot;
}

inline void toggleBlink() {
  const auto &currentValue = digitalRead(LED_PIN);
  digitalWrite(LED_PIN, !currentValue);
}

void myISR()
{
  //PORTD |= (1<<6);
  pulsecountl++;
  if (pulsecountl > pulsememl)
  {
    pulsecountl = 0;
    pulsememl = stepl;
    if (pulsememl < 0)
    {
      PORTD &= ~(1 << dir_pin_l);
      //PORTD |= 0b00001000;
      pulsememl *= -1;
    }
    else
      PORTD |= (1 << dir_pin_l);
    //else PORTD &= 0b11110111;
  }
  else if (pulsecountl == 1)
    PORTD |= (1 << step_pin_l);
  else if (pulsecountl == step_pin_l)
    PORTD &= ~(1 << 2);

  //right motor pulse calculations
  pulsecountr++;
  if (pulsecountr > pulsememr)
  {
    pulsecountr = 0;
    pulsememr = stepr;
    if (pulsememr < 0)
    {
      PORTD |= (1<<dir_pin_r);
      //PORTD &= 0b11011111;
      pulsememr *= -1;
    }
    else
      PORTD &= ~(1<<dir_pin_r);
    //else PORTD |= 0b00100000;
  }
  else if (pulsecountr == 1)
    PORTD |= (1<<step_pin_r);
  else if (pulsecountr == 2)
    PORTD &= ~(1<<step_pin_r);
  //PORTD &= ~(1<<6);
}

