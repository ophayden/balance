#include "Arduino.h"
IntervalTimer myTimer;

int count, temp_period, period=200;
int step_pin=2, dir_pin=3;
const int ledPin = LED_BUILTIN;  // the pin with a LED
void blinkLED();

void setup() {
  DDRD |= (1<<2) | (1<<3);
  DDRB |= (1<<5);
  myTimer.begin(blinkLED, 20);  
}

int ledState = LOW;
volatile unsigned long blinkCount = 0; // use volatile for shared variables

void blinkLED() {
  count++;
  if(count > temp_period){
    count = 0;
    temp_period = period;
  }
  else if(count == 1)PORTD |= (1<<2);
  else if(count == 2)PORTD &= ~(1<<2);
}

void loop() {
  PORTB |= (1<<5);
  delayMicroseconds(20);
  PORTB &= ~(1<<5);
  delayMicroseconds(20);
}
