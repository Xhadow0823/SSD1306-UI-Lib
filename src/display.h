#ifndef __DISPLAY_H__
  #define __DISPLAY_H__
// #include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH   128  // OLED display width, in pixels
#define SCREEN_HEIGHT  64   // OLED display height, in pixels
#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // THIS DISPLAY IS 0x3C !!!  // it can be look up with Wire scan

// this system only have this display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#endif