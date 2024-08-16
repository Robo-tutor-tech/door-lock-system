#include <Arduino.h>
#include <lcd.hpp>
#include <Buzzer.hpp>
#include <utils.h>
#include <MyKeypad.hpp>
#include <SoftwareSerial.h>

void sendSms(SoftwareSerial &gsm, String text);
void setup()
{
  LCD lcd;
  lcd.println("Welcome!!");
  Buzzer buzzer(A2, 100);
  buzzer.beep();
  uint8_t rows[] = {5, 6, 7, 8};
  uint8_t cols[] = {9, 10, 11, 12};
  MyKeypad keypad(makeKeymap(keys), rows, cols, 4, 4, buzzer, lcd);
  const uint8_t door = 4, espCam = A3;
  pinMode(door, OUTPUT);
  pinMode(espCam, OUTPUT);
  digitalWrite(espCam, HIGH);
  SoftwareSerial gsm(3, 2);
  gsm.begin(9600);
  Serial.begin(9600);
  Serial.println("Welcome");
  delay(1000);

  while (true)
  {
    if (char key = keypad.getKey())
    {
      if (key == '1')
      {
        lcd.println("Enter your id\n");
        String id = keypad.readNumber();
        if (id.length() > 0)
        {
          lcd.println("Generating otp\n");
          Serial.println("GENERATE:" + id);
          digitalWrite(espCam, LOW);
          delay(1000);
        }
      }
      if (key == '2')
      {
        lcd.println("Enter OTP\n");
        String otp = keypad.readNumber();
        if (otp.length() > 0)
        {
          lcd.println("verifying otp\n");
          Serial.println("VERIFY:" + otp);
          digitalWrite(espCam, LOW);
          delay(1000);
        }
      }
    }
    digitalWrite(espCam, HIGH);
    if (Serial.available() > 0)
    {
      String data = Serial.readStringUntil('\n');
      data.trim();
      Serial.println(data);
      if (data.startsWith("ERROR:"))
      {
        lcd.println(data.substring(6));
        buzzer.continuousBeep();
      }
      if (data.startsWith("SUCCESS:"))
      {
        lcd.println("OTP verified\nOpening...");
        digitalWrite(door, HIGH);
        delay(10000);
        digitalWrite(door, LOW);
      }
      if (data.startsWith("SMS:"))
      {
        sendSms(gsm, data.substring(4));
      }
    }
    lcd.println("Door lock\nSystem");
    delay(10);
  }
}

void loop()
{
}

void sendSms(SoftwareSerial &gsm, String text)
{
  gsm.println("AT+CMGF=1");
  delay(500);
  Serial.println(text.substring(0, 10));
  Serial.println(text.substring(10));
  gsm.print("AT+CMGS=\"+91");
  gsm.print(text.substring(0, 10));
  gsm.println("\"");
  delay(100);
  gsm.print(text.substring(10));
  delay(100);
  gsm.println((char)26);
  delay(500);
}