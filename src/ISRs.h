#ifndef __ISRs_H__
    #define __ISRs_H__
#include <Arduino.h>

// TODO: change all ISR name and its variables
volatile byte lastSw = -1;
volatile byte sw = HIGH;
volatile bool clicked = 0;
volatile unsigned int clickCount = 0;
// note: do NOT debounce in ISR, it would be not predictable
void swClick() {  // ISR
  sw = PIND&(1<<3) ? HIGH : LOW;  // read the D3
  if(lastSw != sw && sw == HIGH) {
      clicked = true;
      clickCount++;  // for debug
  }
  lastSw = sw;
}

volatile byte lastClk = -1;
volatile unsigned int count = 0;
void readEncoder() {  // ISR
  byte clk = PIND&(1<<2) ? HIGH : LOW;  // read the D2(CLK) pin
  byte dt = PIND&(1<<4) ? HIGH : LOW;  // read the D4(DT) pin
  if(lastClk != clk && clk == HIGH) {
      if(clk != dt) {
        count++;
      }else {
        count--;
      }
  }
  lastClk = clk;
}

#endif