#ifndef __MULTI_APP_H__
    #define __MULTI_APP_H__
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <agents.h>

class AppInterface {
private:
    const char* __name = nullptr;
    bool __exit = false;
    Adafruit_SSD1306* __display = nullptr;
public:
    AppInterface() { }

    virtual void setup() { }
    virtual void loop() { }
    virtual inline const char* name() { return __name; }
    virtual inline void exit() { __exit = true; }
};



class Dice : AppInterface {
private:
    const char* name = "Dice";
    const uint8_t rollingTime = 1;  // s
    uint16_t nCategory = 6;
    bool startOffset = 1;  // e.g. number mode, nCate = 6, startOffset = 1  => 1, 2, 3, 4, 5, 6

    enum DiceMode {
        number, alphabet, ascii, doubleNumber
    };
    DiceMode diceMode = DiceMode::number;

    enum Mode {
        setting, standBy, rolling
    };
    Mode mode = Mode::standBy;
public:
    Dice() { }
    void setup() {

    }
    void loop() {

    }
    void openMenu() {

    }
};

// TODO: move to multiApp.h and set display reference in constructor
class Demo0: AppInterface {
private:
    const char* __name = "DEMO0";
    Adafruit_SSD1306& display;

    bool pause = 0;
public:
    Demo0(Adafruit_SSD1306& displayPtr): display(displayPtr) { }
    void setup() {
      
    }

    void loop() {
      if(SWAgent.isClicked() && SWAgent.getLongPressDeltaTime() < 3000) {
        cli();
        if(pause) {
          // go
          ms = 0;  s = 0; m = 0;
          TCNT2  = 0;  // set counter value to 0
          TCCR2B |= (1 << CS22);
          TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
        }else {
          // stop
          TCCR2B &= ~(1 << CS22);
          TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
        }
        pause = !pause;
        sei();
      }
      display.invertDisplay(pause);

      display.setTextSize(2); // Draw 2X-scale text // 6,8 "12,16"
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(5+count, 8);
      display.println(String(m) + ":" + String(s) + ":" + String(ms));

      if(SWAgent.isHolding()) {
        display.setTextSize(1); // Draw 2X-scale text // 6,8
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(2, 1);
        display.print(String(SWAgent.getLongPressDeltaTime()));
        display.drawRoundRect(10, 2, SWAgent.getLongPressDeltaTime()/3000.0 * 108, 28, 5, WHITE);
      }
    }
};

#endif