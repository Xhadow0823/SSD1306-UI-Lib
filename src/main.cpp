/**
 *                             TX1 ************** VIN
 *                             RX0                GND -- Battery GND
 *                             RST                RST
 *                             GND                5V --- Battery 5V
 *              Encoder CLK --- D2                A7
 *               Encoder SW --- D3                A6
 *               Encoder DT --- D4  Arduino Nano  A5 --- OLED 128x64 SCK
 *       Vibration Motor IN --- D5   ATmega328P   A4 --- OLED 128x64 SDA
 *                              D6                A3
 *                              D7                A2
 *                              D8                A1
 *                              D9                A0
 *                             D10     ......     REF
 *                  Buzzer --- D11     :    :     3V3 -- not working now...
 *                             D12 ****:    :**** D13
*/
#include <Arduino.h>
#include <ISRs.h>
#include <agents.h>
#include <display.h>
#include <utilities.h>
#include <multiApp/core.h>


void setup() {
  // ========== init Serial ==========
  Serial.begin(9600);
  // ========== init Serial end ==========

  // ========== init SSD1306 ==========
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // ========== init SSD1306 end ==========

  // move to REAgent
  // ========== init rotary encoder ==========
  #define CLK 2
  #define DT 3
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(0, readEncoder, CHANGE);
  attachInterrupt(1, swClick, CHANGE);
  // ========== init rotary encoder end ==========

  // set timer2 interrupt at 1kHz
  TimerAgent.init();

  
  Serial.println(F("[START]"));
  multi.setup();

  display.display();
}




void loop() {
  // routine of agents
  SWAgent.update();
  REAgent.update();

  display.clearDisplay();

  multi.loop();
  
  // show free memory space
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(display.width()-6*3, 0);
  display.print(calcFreeMemorySpaceSize());

  // routine of display
  display.display();
  // routine of delay
  delay(10);
}