#include <lcd.hpp>

LCD::LCD(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows) : lcd(lcd_Addr, lcd_cols, lcd_rows)
{
  this->rows = lcd_rows;
  this->cols = lcd_cols;
  this->lcd.init();
  this->lcd.backlight();
}

String LCD::createRowText(String text)
{
  String result = text.substring(0, this->cols);
  for (uint8_t i = result.length(); i < this->cols; i++)
  {
    result += " ";
  }
  return result;
}

void LCD::println(String text)
{
  // Serial.println(text);
  if (text.indexOf("\n") == -1)
  {
    lcd.setCursor(0, 0);
    lcd.print(this->createRowText(text));
    return;
  }
  for (uint8_t i = 0; i < this->rows; i++)
  {
    lcd.setCursor(0, i);
    uint8_t index = text.indexOf("\n");
    if (index == -1)
    {
      lcd.print(this->createRowText(text));
    }
    else
    {
      String printableString = text.substring(0, index);
      lcd.print(this->createRowText(printableString));
      text = text.substring(index + 1);
    }
  }
}

void LCD::clearAndPrintln(String text)
{
  this->clear();
  this->println(text);
}

void LCD::clear()
{
  lcd.clear();
}