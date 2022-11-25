#ifndef __AGENTS_H__
    #define __AGENTS_H__
#include <Arduino.h>
#include <ISRs.h>

class __SWAgent {
private:
  bool __clicked = false;
  unsigned long __longPressStartTime = 0;
  unsigned long __longPressDeltaTime = 0;
  bool __isHolding = false;
public:
  void update() {
    if(clicked) {
      clicked = false;  // clear ISR's buffer !!  // TODO: change variable name of all ISRs!!!
      __clicked = true;
      __isHolding = false;
    }else if(sw == LOW){
      if(__isHolding) {
        __longPressDeltaTime = millis() - __longPressStartTime;
      }else {
        __longPressStartTime = millis();
        __longPressDeltaTime = 0;
        __isHolding = true;
      }
    }
  }
  inline bool isClicked() {
      return __clicked;
  }
  inline bool isHolding() {
    return __isHolding;
  }
  inline unsigned long getLongPressDeltaTime() {  // TODO: get better name, maybe
    return __longPressDeltaTime;
  }
  inline void clearClicked() {
      __clicked = false;
  }
} SWAgent;

class __RotaryEncoderAgent {
private:
  long lastCount = count;
  int8_t offset = 0;
public:
  inline void update() {
    offset += (int8_t)(count - lastCount);
    lastCount = count;
  }
  inline int8_t getOffset() {
    return offset;
  }
  inline void clearOffset() {
    offset = 0;
  }
} REAgent;

#endif