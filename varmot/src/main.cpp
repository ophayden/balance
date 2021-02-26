#include <Arduino.h>


int period=200, step_r=20, count, stepcount_r;
int pulsememl, pulsememr;

int step_pin=2, dir_pin=3;

unsigned long looptime;
int value_i = 0;
const int values[] = {5, 20, 200};

void setup(){
  // Serial.begin(9600);

  DDRD |= 0b00111100;
  // DDRB |= 0b00100000;

  TCNT2 = 0;
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A |= 0b00000010;
  TCCR2B |= 0b00000010;
  TIMSK2 |= 0b00000010;
  OCR2A = 40;
  //sei();
}

void loop(){
  delay(20);
  // PORTD |= 0b10000000;
  // PORTD &= 0b01111111;

}

ISR(TIMER2_COMPA_vect){
  count++;

  if(count > pulsememl){  
    count = 0;
    pulsememl = period;
    if(pulsememl < 0){ 
      PORTD &= (0 << dir_pin); 
      pulsememl *= -1;   
    }
    else PORTD |= (1 << dir_pin); 
  }

  else if(count == 1)PORTD |= (1 << step_pin);
  else if(count == 2)PORTD &= (0 << step_pin);
  
  
}
