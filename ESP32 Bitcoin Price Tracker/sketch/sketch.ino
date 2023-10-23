#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "secrets.h" 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

const String url = "http://api.coindesk.com/v1/bpi/currentprice/GBP";
const String url2 = "http://api.coindesk.com/v1/bpi/currentprice/BTC.json";
const String url3 = "http://api.coindesk.com/v1/bpi/currentprice/EUR.json";
const String cryptoCode = "BTC";

const unsigned char bitcoinIcon [] PROGMEM = {
0x00, 0x7e, 0x00, 0x03, 0xff, 0xc0, 0x07, 0x81, 0xe0, 0x0e, 0x00, 0x70, 0x18, 0x28, 0x18, 0x30, 
0x28, 0x0c, 0x70, 0xfc, 0x0e, 0x60, 0xfe, 0x06, 0x60, 0xc7, 0x06, 0xc0, 0xc3, 0x03, 0xc0, 0xc7, 
0x03, 0xc0, 0xfe, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xc3, 0x83, 0xc0, 0xc1, 0x83, 0x60, 0xc3, 0x86, 
0x60, 0xff, 0x06, 0x70, 0xfe, 0x0e, 0x30, 0x28, 0x0c, 0x18, 0x28, 0x18, 0x0e, 0x00, 0x70, 0x07, 
0x81, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0x7e, 0x00
};

HTTPClient http;
String lastPrice;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);           
  display.setTextColor(SSD1306_WHITE);      
  display.setCursor(0,0); 
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.print("CONNECTED to SSID: ");
  Serial.println(ssid);

  display.print("Connected to ");
  display.println(ssid);
  display.display();
  delay(5000);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Getting current data...");

    http.begin(url);
    int httpCode = http.GET();
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);
    if (httpCode > 0) {
      StaticJsonDocument<768> doc;
      DeserializationError error = deserializeJson(doc, http.getString());
    

      if (error) {
        Serial.print(F("deserializeJson failed: "));
        Serial.println(error.f_str());
        delay(2500);
        return;
      }

    http.begin(url2);
    int httpCode = http.GET();
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      StaticJsonDocument<768> doc2;
      DeserializationError error = deserializeJson(doc2, http.getString());

      if (error) {
        Serial.print(F("deserializeJson failed: "));
        Serial.println(error.f_str());
        delay(2500);
        return;
      }

    http.begin(url3);
    int httpCode = http.GET();
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      StaticJsonDocument<768> doc3;
      DeserializationError error = deserializeJson(doc3, http.getString());

      if (error) {
        Serial.print(F("deserializeJson failed: "));
        Serial.println(error.f_str());
        delay(2500);
        return;
      }

      Serial.print("HTTP Status Code: ");
      Serial.println(httpCode);

      String BTCGBPPrice = doc["bpi"]["GBP"]["rate_float"].as<String>();
      if(BTCGBPPrice == lastPrice) {
        Serial.print("Price hasn't changed (Current/Last): ");
        Serial.print(BTCGBPPrice);
        Serial.print(" : ");
        Serial.println(lastPrice);
        delay(1250);
        return;
      } else {
        lastPrice = BTCGBPPrice;
      }

      String BTCUSDPrice = doc2["bpi"]["USD"]["rate_float"].as<String>();
      if(BTCUSDPrice == lastPrice) {
        Serial.print("Price hasn't changed (Current/Last): ");
        Serial.print(BTCUSDPrice);
        Serial.print(" : ");
        Serial.println(lastPrice);
        delay(1250);
        return;
      } else {
        lastPrice = BTCUSDPrice;
      }
    
      String BTCEURPrice = doc3["bpi"]["EUR"]["rate_float"].as<String>();
        if(BTCEURPrice == lastPrice) {
          Serial.print("Price hasn't changed (Current/Last): ");
          Serial.print(BTCEURPrice);
          Serial.print(" : ");
          Serial.println(lastPrice);
          delay(1250);
          return;
        } else {
          lastPrice = BTCEURPrice;
        }

      String lastUpdated = doc["time"]["updated"].as<String>();

      display.clearDisplay();
      display.drawBitmap((128/2) - (24/2), 0, bitcoinIcon, 24, 24, WHITE);
      display.display();

      display.setTextSize(1);
      printCenter(BTCGBPPrice + " GBP", 0, 32);

      display.setTextSize(1);
      printCenter(BTCUSDPrice + " USD", 0, 40);

      display.setTextSize(1);
      printCenter(BTCEURPrice + " EUR", 0, 48);
      
      display.setTextSize(1);
      printCenter("1.0000 BTC", 0, 56);

      display.display();
      http.end();
      }
    }
  }
    delay(5000);
  }
}

void printCenter(const String buf, int x, int y)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
  display.setCursor((x - w / 2) + (128 / 2), y);
  display.print(buf);
}