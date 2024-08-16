#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

const char *ssid = "Robotutor", *password = "Robotutor";
String serverName = "http://robotutortech.ddns.net:7456";
void connect();
String verifyOtp(WiFiClient &client, String data);
String generateOtp(WiFiClient &client, String data);
void sendSms(SoftwareSerial &gsm, String text);

void setup()
{
  const uint8_t cam = D0;
  pinMode(cam, OUTPUT);
  digitalWrite(cam, HIGH);
  Serial.begin(9600);
  SoftwareSerial gsm(D1, D2);
  gsm.begin(9600);
  WiFiClient client;
  Serial.println("Welcome");
  connect();
  delay(1000);
  while (true)
  {
    connect();
    if (Serial.available() > 0)
    {
      String data = Serial.readStringUntil('\n');
      data.trim();
      if (data.startsWith("GENERATE:"))
      {
        digitalWrite(cam, LOW);
        delay(1000);
        digitalWrite(cam, HIGH);
        String res = generateOtp(client, data.substring(9, 13));
        if (res.startsWith("SMS:"))
        {
          sendSms(gsm, res.substring(4));
        }
        else
        {
          Serial.println(res);
        }
        delay(500);
        digitalWrite(cam, LOW);
        delay(1000);
        digitalWrite(cam, HIGH);
      }
      if (data.startsWith("VERIFY:"))
      {
        String res = verifyOtp(client, data.substring(7, 11));
        Serial.println(res);
      }
    }
    delay(100);
  }
}

void loop()
{
}

void connect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

String verifyOtp(WiFiClient &client, String data)
{
  HTTPClient http;
  http.begin(client, serverName + "/verify-otp");
  http.addHeader("Content-Type", "text/plain");
  int httpResponseCode = http.POST(data);
  String response = http.getString();
  http.end();
  return (httpResponseCode == 200 ? "SUCCESS:" : "ERROR:") + response;
}

String generateOtp(WiFiClient &client, String data)
{
  HTTPClient http;
  http.begin(client, serverName + "/generate-otp");
  http.addHeader("Content-Type", "text/plain");
  int httpResponseCode = http.POST(data);
  String response = http.getString();
  http.end();
  return (httpResponseCode == 200 ? "SMS:" : "ERROR:") + response;
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