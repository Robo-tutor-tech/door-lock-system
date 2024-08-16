#include <MyKeypad.hpp>

MyKeypad::MyKeypad(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, Buzzer &buzzer, LCD &lcd) : keypad(userKeymap, row, col, numRows, numCols)
{
  this->buzzer = &buzzer;
  this->lcd = &lcd;
}

String MyKeypad::readNumber()
{
  String number = "";
  unsigned long int start = millis();
  while (millis() - start <= 60000)
  {
    char key = this->keypad.getKey();
    if (key)
    {
      buzzer->beep();
      switch (key)
      {
      case 'A':
        number = number.substring(0, number.length() - 1);
        break;
      case 'B':
        number = "";
        break;
      case 'C':
        return "";
      case 'D':
        return number;
      default:
        number += key;
        break;
      }
      this->lcd->println(number + "\n");
    }
    delay(50);
  }
  return number;
}

char MyKeypad::getKey()
{
  char key = this->keypad.getKey();
  if (key)
  {
    this->buzzer->beep();
  }
  return key;
}