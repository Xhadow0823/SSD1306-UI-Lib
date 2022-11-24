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
    Adafruit_SSD1306& display;

    const unsigned long rollingPeriod = 1000;  // ms
    // uint16_t nCategory = 6;
    uint16_t nCategory = 256;
    // int startOffset = 1;  // e.g. number mode, nCate = 6, startOffset = 1  => 1, 2, 3, 4, 5, 6
    int startOffset = 0;  // e.g. number mode, nCate = 6, startOffset = 1  => 1, 2, 3, 4, 5, 6

    union RandomShit{
      long number;
      int16_t ascii;
      char alphabet;
      double doubleNumber;
    } randomShit;

    #define autoSetRandomShit(x) \
      switch(diceMode){ \
        case DiceMode::number:\
          randomShit.number = (long)(x);\
          break;\
        case DiceMode::ascii:\
          randomShit.ascii = (int16_t)(x);\
          break;\
        case DiceMode::alphabet:\
          randomShit.alphabet = (char)(x);\
          break;\
        case DiceMode::doubleNumber:\
          randomShit.doubleNumber = (double)(x);\
          break;\
      }\

    #define autoGetRandomShit() \
      (diceMode==DiceMode::number? randomShit.number : (\
        diceMode==DiceMode::ascii? randomShit.ascii : (\
          diceMode==DiceMode::alphabet? randomShit.alphabet : (\
            randomShit.doubleNumber)))) \

    unsigned long rollingStartTime = 0;
    
    enum DiceMode {
        number, alphabet, ascii, doubleNumber
    };
    DiceMode diceMode = DiceMode::doubleNumber;

    enum Mode {
        setting, standBy, rolling
    };
    Mode mode = Mode::standBy;
public:
    Dice(Adafruit_SSD1306& displayPtr): display(displayPtr) { }
    void setup() {

    }
    void loop() {
      if(SWAgent.isClicked()){
        rollingStartTime = millis();
        mode = Mode::rolling;
      }
      if(mode == Mode::rolling) {
        generateRandomShit();
        // for DEBUG
        display.fillRect(1, 2, 5, 5, WHITE);
        if(millis() - rollingStartTime >= rollingPeriod) {
          mode = Mode::standBy;
        }
      }
      if(mode == Mode::standBy) {
        // do nothing
      }

      display.setTextSize(2); // Draw 2X-scale text // 6,8 "12,16"
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(display.width()/2-6, display.height()/2-8);
      switch(diceMode){
        case DiceMode::number:
          display.println(autoGetRandomShit());
          break;
        case DiceMode::ascii:
          display.drawRect(0, 0, 5, 5, WHITE);
          display.setTextSize(1);
          display.setCursor(7, 0);
          display.println(String(randomShit.ascii));

          display.setTextSize(2);
          display.setCursor(display.width()/2-6, display.height()/2-8);
          display.cp437(true); 
          display.write( randomShit.ascii=='\n'? ' ' : randomShit.ascii );
          break;
        case DiceMode::alphabet:
          display.cp437(false); 
          display.setTextSize(1);
          display.setCursor(7, 0);
          display.println((int)autoGetRandomShit());
          display.setTextSize(2);
          display.setCursor(display.width()/2-6, display.height()/2-8);

          display.write(autoGetRandomShit());
          break;
        case DiceMode::doubleNumber:
          display.println(autoGetRandomShit());
          break;
      }
    }
    void generateRandomShit() {
      switch(diceMode){
        case DiceMode::number:
          nCategory = 6;  startOffset=1;
          autoSetRandomShit(random()%nCategory + startOffset);
          break;
        case DiceMode::ascii:
          nCategory = 256;  startOffset=0;
          autoSetRandomShit(random()%nCategory + startOffset);
          break;
        case DiceMode::alphabet:
          autoSetRandomShit( random()%26 + (random()%2? 65: 97) );
          break;
        case DiceMode::doubleNumber:
          autoSetRandomShit(random()%1000 / 1000.0);
          break;
      }
    }
    void openMenu() {

    }
};


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