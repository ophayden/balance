int stepr = 0,
    pulsecountr = 0,
    pulsememr = 0;
int stepl = 0,
    pulsecountl = 0,
    pulsememl = 0;

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
  else if (pulsecountl == 2)
    PORTD &= ~(1 << step_pin_l);

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
