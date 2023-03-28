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
    clearClicked();
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
  // called before each loop
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
    clearOffset();
    offset += (int8_t)(count - lastCount);
    lastCount = count;
  }
  inline int8_t getOffset() {
    return offset;
  }
  // called before each loop
  inline void clearOffset() {
    offset = 0;
  }
} REAgent;

class __TimerAgent {
private:
public:
  inline void init() {
    stop();
  }
  void restart() {
    // ========== set timer2 interrupt at 1kHz ==========
    cli();
    clearReg();
    TCCR2A = 0;               // set entire TCCR2A register to 0
    TCCR2B = 0;               // same for TCCR2B
    TCNT2  = 0;               // initialize counter value to 0
    OCR2A  = 249;             // = (16*10^6) / (64*1000) - 1 (must be <256)
    TCCR2A |= (1 << WGM21);   // turn on CTC mode
    TCCR2B |= (1 << CS22);
    TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
    sei();
    // ========== set timer2 interrupt at 1kHz end ==========
  }
  void pause() {
    cli();
    TCCR2B &= ~(1 << CS22);
    TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
    sei();
  }
  void resume() {
    cli();
    TCCR2B |= (1 << CS22);
    TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
    sei();
  }
  void stop() {
    cli();
    clearReg();
    TCCR2A = 0;               // set entire TCCR2A register to 0
    TCCR2B = 0;               // same for TCCR2B
    TCNT2  = 0;               // initialize counter value to 0
    OCR2A  = 249;             // = (16*10^6) / (64*1000) - 1 (must be <256)
    TCCR2A |= (1 << WGM21);   // turn on CTC mode
    TCCR2B &= ~(1 << CS22);
    TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
    sei();
  }
  inline void clearReg() {
    m = 0; s = 0; ms = 0;
  }
} TimerAgent;

#endif