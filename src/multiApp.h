#ifndef __MULTI_APP_H__
    #define __MULTI_APP_H__
#include <Arduino.h>
#include <agents.h>
#include <string.h>
#include <UI.h>
#include <display.h>

// TODO: make UIHelper better
// TODO: add a multi-app manager to run two apps

class AppInterface {
protected:
    bool __exit = false;
public:
    AppInterface() { }
    inline static const char* name = "AppInterface";

    virtual void setup() { }
    virtual void loop() { }
    virtual inline bool exit() { return __exit; }
};

class __UIHelper: public AppInterface {
private:
  int16_t menuCursor = 0;
  int16_t selectedItem = 0;  // 0: not selected yet
  UIWindow window;
  UIList list;
public:
  __UIHelper() {
    window.setWindowMargin(10);
    list.setPosition(window.innerStartX()+1, window.innerStartY()+1);
    list.setSize(window.innerWidth()-2, window.innerHeight()-2);
    static const char* items[] = {"kiwi", "Dodo", "peacock", "Sparrow", "Pigeon", "crow", "exit"};  // default
    list.setItems(nullptr, sizeof(items) / sizeof(const char*));
    list.setCursor(menuCursor);
  }
  inline static const char* name = "UIHelper";
  void setup() {
    // nothing...
  }
  void loop() {
    openMenu();
  }
  int16_t openMenu() {
    menuCursor += REAgent.getOffset();
    menuCursor = (menuCursor+list.itemListLength()) % (list.itemListLength());
    list.setCursor(menuCursor);

    window.draw();
    list.draw();
    selectedItem = !SWAgent.isClicked()? 0 : (menuCursor+1);
    return selectedItem;
  }
  inline int16_t getSelectedItem() {
    return selectedItem;
  }
  void setMenuItems(const char** items, size_t size) {
    list.setItems(items, size);
  }
} UIHelper;



class Dice : public AppInterface {
private:
    const unsigned long rollingPeriod = 1000;  // ms
    // uint16_t nCategory = 6;
    uint16_t nCategory = 256;
    // int startOffset = 1;  // e.g. number mode, nCate = 6, startOffset = 1  => 1, 2, 3, 4, 5, 6
    int startOffset = 0;  // e.g. number mode, nCate = 6, startOffset = 1  => 1, 2, 3, 4, 5, 6

    long __xOffset = 0;

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
    DiceMode diceMode = DiceMode::ascii;

    enum Mode {
        setting, standBy, rolling
    };
    Mode mode = Mode::standBy;

    // for Debug
    const char * menuItems[4] = { "Font++", "change mode", "change categories", "exit" };
public:
    Dice() { }
    inline static const char* name = "Dice";
    
    // for denug
    void setup() {
      // set the UI helper menu item and functions
      UIHelper.setMenuItems(menuItems, sizeof(menuItems)/sizeof(const char*));
    }
    void loop() {
      if(mode != Mode::setting) {
        __xOffset += REAgent.getOffset();
      }
      if(SWAgent.getLongPressDeltaTime() > 1000) {
        mode = Mode::setting;
      }
      else if(mode != Mode::setting && SWAgent.isClicked()){
        rollingStartTime = millis();
        mode = Mode::rolling;
      }
      if(mode == Mode::rolling) {
        generateRandomShit();
        // for DEBUG
        display.fillRect(1, 2, 5, 5, SSD1306_WHITE);
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
          display.drawRect(0, 0, 5, 5, SSD1306_WHITE);
          display.setTextSize(1);
          display.setCursor(7, 0);
          display.println(String(randomShit.ascii+__xOffset));

          display.setTextSize(2);
          display.setCursor((display.width()/2-6 + __xOffset)%(128-11), display.height()/2-8);
          display.cp437(true); 
          display.write( randomShit.ascii+__xOffset=='\n'? ' ' : randomShit.ascii+__xOffset );
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

      // test
      if(mode==Mode::setting) { 
        int funcN = UIHelper.openMenu();
        if(funcN != 0){
          Serial.println(String("f: ") + String(funcN));
          if(funcN == 4) {  // menu "exit"
            __exit = true;
            return ;
          }
          mode = Mode::standBy;
        }
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
};


class Demo0: public AppInterface {
private:
    bool pause = 0;
public:
    // Demo0() { }
    inline static const char* name = "Demo0";
    void setup() {
      
    }

    void loop() {
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(display.width()-6*5, display.height() - 8);
      display.print(pause? "pause" : "not");
      display.setCursor(display.width()-6*4, display.height() - 8*2);
      display.print(m); display.print(' ');  display.print(s);

      if(SWAgent.isClicked() && SWAgent.getLongPressDeltaTime() < 3000) {
        cli();
        if(pause) {
          // go
          ms = 0;  s = 0;  m = 0;
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
      if(s >= 5) {
        __exit = true;
        cli();
        // start at next loop
        pause = false;
        TCCR2B &= ~(1 << CS22);
        TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
        TCNT2  = 0;  // set counter value to 0
        ms = 0;  s = 0;  m = 0;
        sei();
        // TCCR2B |= (1 << CS22);
        // TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
        // ^ have to run two following line to start timer...////////////////////
        return ;
      }
      display.invertDisplay(pause);

      display.setTextSize(2);  // Draw 2X-scale text // 6,8 "12,16"
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(12+count, display.height()/2 - 8);
      static char buffer[12] = "";
      sprintf(buffer, "%d:%d:%d", m, s, ms);
      display.print(buffer);

      if(SWAgent.isHolding()) {
        display.setTextSize(1);  // Draw 2X-scale text // 6,8
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(2, 1);
        display.print(String(SWAgent.getLongPressDeltaTime()));
        display.drawRoundRect(10, 2, SWAgent.getLongPressDeltaTime()/3000.0 * 108, display.height()-4, 5, SSD1306_WHITE);
      }
    }
};

#endif