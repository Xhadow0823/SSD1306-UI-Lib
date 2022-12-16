/**
 *                             TX1 ************** VIN
 *                             RX0                GND -- Battery GND
 *                             RST                RST
 *                             GND                5V --- Battery 5V
 *              Encoder CLK     D2                A7
 *              Encoder CLK --- D3                A6
 *               Encoder DT --- D4  Arduino Nano  A5 --- OLED 128x64 SCK
 *       Vibration Motor IN --- D5   ATmega328P   A4 --- OLED 128x64 SDA
 *                              D6                A3
 *                              D7                A2
 *                              D8                A1
 *                              D9                A0
 *                             D10     ......     REF
 *                             D11     :    :     3V3 -- not working now...
 *                             D12 ****:    :**** D13
*/
#include <Arduino.h>
#include <ISRs.h>
#include <agents.h>
#include <multiApp.h>
#include <display.h>

// ISRs
void readEncoder();
void swClick();

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  #define CLK 2
  #define DT 3
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);

  attachInterrupt(0, readEncoder, CHANGE);
  attachInterrupt(1, swClick, CHANGE);

  //set timer2 interrupt at 1kHz
  cli();
  TCCR2A = 0;  // set entire TCCR2A register to 0
  TCCR2B = 0;  // same for TCCR2B
  TCNT2  = 0;  //initialize counter value to 0
  OCR2A = 249;  // = (16*10^6) / (64*1000) - 1 (must be <256)
  TCCR2A |= (1 << WGM21);  // turn on CTC mode
  TCCR2B |= (1 << CS22);
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  sei();

  display.display();
}



void loop() {
  SWAgent.clearClicked();
  SWAgent.update();
  REAgent.clearOffset();
  REAgent.update();

  display.clearDisplay();

  

  // demo0.loop();
  // UIHelper0.openMenu();
  

  display.display();

  delay(10);
}