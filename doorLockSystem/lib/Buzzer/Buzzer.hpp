#ifndef BUZZER_HPP
#define BUZZER_HPP

#include <Arduino.h>

class Buzzer
{
private:
  int pin;
  int delayInMs;

public:
  Buzzer() = delete;
  Buzzer(int pin, int delay);
  void beep();
  void beepBeep();
  void continuousBeep();
};

#endif