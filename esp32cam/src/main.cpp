#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "Robotutor";
const char* password = "Robotutor";

// Server URL
String serverName = "robotutortech.ddns.net";
String serverPath = "/upload";
int serverPort = 7456;
int button = 14;

// Camera configuration
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT_PULLUP);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop() {
  if (digitalRead(button) == LOW) {

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      delay(1000);
      ESP.restart();
    }

    Serial.println("Connecting to server: " + serverName);

    if (client.connect(serverName.c_str(), serverPort)) {
      Serial.println("Connection successful!");
      String head = "--robotutor\r\nContent-Disposition: form-data; name=\"image\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
      String tail = "\r\n--robotutor--\r\n";

      uint32_t imageLen = fb->len;
      uint32_t extraLen = head.length() + tail.length();
      uint32_t totalLen = imageLen + extraLen;

      client.println("POST " + serverPath + " HTTP/1.1");
      client.println("Host: " + serverName);
      client.println("Content-Length: " + String(totalLen));
      client.println("Content-Type: multipart/form-data; boundary=robotutor");
      client.println();
      client.print(head);

      uint8_t* fbBuf = fb->buf;
      size_t fbLen = fb->len;
      for (size_t n = 0; n < fbLen; n += 1024) {
        if (n + 1024 < fbLen) {
          client.write(fbBuf, 1024);
          fbBuf += 1024;
        } else if (fbLen % 1024 > 0) {
          size_t remainder = fbLen % 1024;
          client.write(fbBuf, remainder);
        }
      }
      client.print(tail);
      esp_camera_fb_return(fb);
      while (digitalRead(button) == LOW)
        ;
    }
  }
  delay(100);
}