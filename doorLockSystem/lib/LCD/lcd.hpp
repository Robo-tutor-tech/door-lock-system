#ifndef LCD_HPP
#define LCD_HPP

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class LCD
{
private:
  uint8_t rows = 2, cols = 16;
  LiquidCrystal_I2C lcd;
  String createRowText(String text);

public:
  LCD() : LCD(0x27, 16, 2){};
  LCD(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows);
  void println(String text);
  void clear();
  void clearAndPrintln(String text);
};

#endif