#include <M5Core2.h>
#include <MHZ19_uart.h>
#include <WiFi.h>
#include "time.h"
#include "Ambient.h"

// Ambient
Ambient ambient;
unsigned int channelId = 0;           // Your Channel Id
const char* writeKey = "XXX";         // Your WRITE KEY

// WiFi
const char* ssid       = "SSID";      // Your SSID
const char* password   = "PASSWORD";  // Your Password
WiFiClient client;

// Serial
const int rx_pin = 13; //Serial rx pin no
const int tx_pin = 14; //Serial tx pin no

// MH Sensor
MHZ19_uart mhz19;

// Time
const char* ntpServer = "time1.aliyun.com";
const long  gmtOffset_sec = 3600 * 9;
const int   daylightOffset_sec = 3600;

void setup()
{
  M5.begin();
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);

  WiFi.begin(ssid, password);
  M5.Lcd.println("Connecting WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.clear();
  //Serial.begin(9600);
  mhz19.begin(rx_pin, tx_pin);
  mhz19.setAutoCalibration(false);
  // Time
  configTime(9 * 3600L , daylightOffset_sec, ntpServer);
  M5.Lcd.drawString("MH-Z19 is warming up now.", 0, 20, 4);
  // ambient
  ambient.begin(channelId, writeKey, &client);

  delay(10 * 1000);
  M5.Lcd.clear();
}

char co2Message[256];
char tempMessage[256];
char timeInfo[64];

tm getTime() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  return timeinfo;
}

void loop()
{
  int co2ppm = mhz19.getCO2PPM();
  int temp = mhz19.getTemperature();

  tm nowTime = getTime();
  M5.Lcd.clear();
  sprintf(timeInfo, "%02d:%02d", nowTime.tm_hour, nowTime.tm_min);
  M5.Lcd.drawString(timeInfo, 0, 20, 4);
  
  sprintf(co2Message,  "CO2 : %5d ppm", co2ppm);
  M5.Lcd.drawString(co2Message, 0, 80, 4);
  sprintf(tempMessage, "TEMP: %5d c", temp);
  M5.Lcd.drawString(tempMessage, 0, 120, 4);

  // ambient sending
  ambient.set(1, co2ppm);
  ambient.set(2, temp);
  ambient.send();
  
  delay(1000 * 60);
}
