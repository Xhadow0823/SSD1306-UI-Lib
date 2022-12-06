#ifndef __AGENTS_H__
    #define __AGENTS_H__
#include <Arduino.h>
#include <ISRs.h>

#include <stdarg.h>

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

struct VBAction {
  uint8_t strength = 255;
  unsigned int duration = 50;
  VBAction(const uint8_t strength, const unsigned int duration): strength(strength), duration(duration) { }
  VBAction() {}
  inline const VBAction * const tempPtr() { return this; }
};

#define tempVBAction(...) (VBAction(__VA_ARGS__).tempPtr())

class __VBAgent {
private:
  const uint8_t motorPin = 5;
  uint8_t analogValue = 255;

  unsigned long startTime = 0;
  unsigned long duration = 0;
public:
  // __VBAgent() { }
  void init() {
    pinMode(motorPin, OUTPUT);
  }
  void update() {
    // if(duration!=0) {
      if(millis() - startTime < duration) {
        analogWrite(motorPin, analogValue);
        // Serial.println("bzz");
      }else {
        // time's up
        analogWrite(motorPin, 0);
        duration = 0;
        // Serial.println("bzz END");
      }
    // }
  }

  void once(VBAction&& action) {
    // set the task to do 
    // set startTime
    startTime = millis();
    // set duration
    duration = action.duration;
    analogValue = action.strength;
  }
  #define NUMARGS(...)  (sizeof((const VBAction *[]){__VA_ARGS__})/sizeof(const VBAction *))
  #define vbserial(...) serial(NUMARGS(__VA_ARGS__), __VA_ARGS__)
  void serial(int actionCnt, ...) const {
    // push some action input task queue
    // once(VBAction(255, 50));
    // once(VBAction(200, 50));
    // once(VBAction(150, 50));
    // once(VBAction(100, 50));
    // ...
    va_list actions;
    va_start(actions, actionCnt);

    for(int i = 0; i < actionCnt; i++) {
      const VBAction* a = va_arg(actions, const VBAction*);
      Serial.println(String(a->strength) + " " + String(a->duration));
    }
    
    va_end(actions);
  }
} VBAgent;

#endif