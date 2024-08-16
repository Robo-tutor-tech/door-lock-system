#ifndef KEYPAD_HPP
#define KEYPAD_HPP

#include <Keypad.h>
#include <Buzzer.hpp>
#include <lcd.hpp>

class MyKeypad
{
private:
  Buzzer *buzzer;
  LCD *lcd;
  Keypad keypad;

public:
  MyKeypad(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, Buzzer &buzzer, LCD &lcd);
  String readNumber();
  char getKey();
};

#endif