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