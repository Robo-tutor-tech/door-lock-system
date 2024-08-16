#include <Buzzer.hpp>

Buzzer::Buzzer(int pin, int delayInMs)
{
  this->pin = pin;
  this->delayInMs = delayInMs;
  pinMode(this->pin, OUTPUT);
}

void Buzzer::beep()
{
  digitalWrite(this->pin, HIGH);
  delay(this->delayInMs);
  digitalWrite(this->pin, LOW);
}

void Buzzer::beepBeep()
{
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(this->pin, HIGH);
    delay(this->delayInMs);
    digitalWrite(this->pin, LOW);
    delay(this->delayInMs);
  }
}

void Buzzer::continuousBeep()
{
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(this->pin, HIGH);
    delay(this->delayInMs);
    delay(this->delayInMs);
  }
  digitalWrite(this->pin, LOW);
}