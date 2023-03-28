#ifndef __BUZZER_HELPER_H__
  #define __BUZZER_HELPER_H__
#include <Arduino.h>

#define BUZZER_PIN 11

class BuzzerHelper {
private:
  uint16_t lastBeepTime = 0;
  bool* beepTable = nullptr;
  uint8_t beepTableSize = 0;
  uint8_t beepIdx = 0;
  uint16_t interval = 125;
  // very quich: 0~53
  // 54~108
  // 109~162
  // 163~255
public:
  BuzzerHelper() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println(F("CONSTRUCT!"));
  }
  ~BuzzerHelper() {
    deleteTable();
    Serial.println(F("DELETE!"));
  }

  void play(uint16_t current) {
    if(current - lastBeepTime > interval) {
      lastBeepTime = current;
      digitalWrite(BUZZER_PIN, beepTable[beepIdx]);
      this->beepIdx = (beepIdx+1)%8;
    }
  }
  void play() {
    uint16_t current = millis();
    play(current);
  }
  void stop() {
    digitalWrite(BUZZER_PIN, LOW);
    beepIdx = 0;
  }

  /**
   * param: size of the beep table, beepTable[0], beepTable[1], ...
  */
  void setTable(uint8_t argCount, ...) {
    deleteTable();

    beepTableSize = argCount;
    va_list tab;
    va_start(tab, argCount);
    beepTable = new bool[argCount];
    for(uint8_t i = 0; i < argCount; i++) {
      beepTable[i] = (bool)va_arg(tab, int);
    }
    va_end(tab);
  }
  void deleteTable() {
    if(beepTable) {
      delete beepTable;
      beepTableSize = 0;
      beepIdx = 0;
      beepTable = nullptr;
    }
  }
  inline uint16_t setInterval(uint16_t interval) {
    return this->interval = interval;
  }
  inline uint16_t getInterval() {
    return interval;
  }
};


#endif