#include <Arduino.h>
#include <ESP32-Chimera-Core.h>
#define tft M5.Lcd


void setup()
{

  // initialize the M5Stack object
  M5.begin();

  // Lcd display
  tft.fillScreen(TFT_WHITE);
  delay(500);
  tft.fillScreen(TFT_RED);
  delay(500);
  tft.fillScreen(TFT_GREEN);
  delay(500);
  tft.fillScreen(TFT_BLUE);
  delay(500);
  tft.fillScreen(TFT_BLACK);
  delay(500);

  // text print
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.printf("Display Test!");

  // draw graphic
  delay(1000);
  tft.drawRect(100, 100, 50, 50, TFT_BLUE);
  delay(1000);
  tft.fillRect(100, 100, 50, 50, TFT_BLUE);
  delay(1000);
  tft.drawCircle(100, 100, 50, TFT_RED);
  delay(1000);
  tft.fillCircle(100, 100, 50, TFT_RED);
  delay(1000);
  tft.drawTriangle(30, 30, 180, 100, 80, 150, TFT_YELLOW);
  delay(1000);
  tft.fillTriangle(30, 30, 180, 100, 80, 150, TFT_YELLOW);

}

// the loop routine runs over and over again forever
void loop()
{
  //rand draw
  tft.fillTriangle(random(tft.width()-1), random(tft.height()-1), random(tft.width()-1), random(tft.height()-1), random(tft.width()-1), random(tft.height()-1), random(0xfffe));
  M5.update();

}
