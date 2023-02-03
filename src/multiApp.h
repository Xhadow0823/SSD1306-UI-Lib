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
    virtual ~AppInterface() { }  // IMPORTANT
    inline static const char* PROGMEM name = "AppInterface";

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
  // inline static const char* items[] = {"kiwi", "Dodo", "peacock", "Sparrow", "Pigeon", "crow", "exit"};  // default
public:
  __UIHelper() {
    window.setWindowMargin(8);  // for debug
    // window.setWindowMargin(4);
    list.setPosition(window.innerStartX()+1, window.innerStartY()+1);
    list.setSize(window.innerWidth()-2, window.innerHeight()-2);
    // list.setItems(items, sizeof(items) / sizeof(const char*));  // for debug
    list.setItems(nullptr, 0);
    list.setCursor(menuCursor);
  }
  inline static const char* PROGMEM name = "UIHelper";
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
  /** pointer items MUST be public? */
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

    
public:
    Dice() { }
    inline static const char* PROGMEM name = "Dice";
    // for Debug
    const PROGMEM char * menuItems[4] = { "Font++", "change mode", "change categories", "exit" };
    // for denug
    void setup() {
      // set the UI helper menu item and functions
      UIHelper.setMenuItems(menuItems, sizeof(menuItems)/sizeof(const char*));
      // UIHelper.setMenuItems(nullptr, sizeof(menuItems)/sizeof(const char*));
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
          // display.println(String(randomShit.ascii+__xOffset));
          display.print((uint8_t)randomShit.ascii+__xOffset);

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
      // FIXME: click the menu item after menu was opened
      if(mode==Mode::setting) { 
        uint8_t funcN = UIHelper.openMenu();
        if(funcN != 0){
          Serial.println(funcN);
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
    inline static const char* PROGMEM name = "Demo0";
    void setup() {
      TimerAgent.stop();
      pause = true;
    }

    void loop() {
      if(SWAgent.isClicked() && SWAgent.getLongPressDeltaTime() < 3000) {
        if(pause) {
          // restart
          TimerAgent.restart();
        }else {
          // pause
          TimerAgent.pause();
        }
        pause = !pause;
      }
      if(s >= 5) {
        TimerAgent.stop();
        __exit = true;
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
        display.print(SWAgent.getLongPressDeltaTime());
        display.drawRoundRect(10, 2, SWAgent.getLongPressDeltaTime()/3000.0 * 108, display.height()-4, 5, SSD1306_WHITE);
      }
    }
};


class Pomodoro: public AppInterface {
private:
  // register
  uint8_t countDownFrom = 0;
  bool isBreak = false;
  int8_t cntMin = 0, cntSec = 0;  // count down to show
  bool isPause = true;
  uint8_t nPomo = 0;  // if nPomo == (longBreakInterval=4), then break for longBreakTime  // add 1 when a pomo is over
  bool isMenuOpen = false;
  bool isCurrentOver = false;
  
  // basic setting
  // uint8_t pomoTime = 25, shortBreakTime = 5, longBreakTime = 15, longBreakInterval = 4;
  uint8_t pomoTime = 2, shortBreakTime = 1, longBreakTime = 2, longBreakInterval = 2;
  bool showSec = true;

  // temp
  char countDownStringBuffer[8] = "";
  uint16_t lastBlinkTime = 0;
  bool blinkDisappear = false;

public: 
inline static const char* PROGMEM name = "Pomodoro";
void setup() {
  TimerAgent.stop();
  reset();
}
void loop() {
  // handle click event
  if(SWAgent.isClicked()) {
    if(SWAgent.getLongPressDeltaTime() < 1000) {  // click
      if(isPause) {
        if(isCurrentOver) {
          gotoNext(); // update regs and restart timer
        }else {
          TimerAgent.resume();
        }
        isPause = false;
      }
    }else {  // long click
      TimerAgent.pause();
      isPause = true;
      // open menu
    }
  }
  // check time
  updateCountDownTimer();
  // draw
  draw();
  drawDebugRect();
}
void gotoNext() {
  if(!isBreak) {  // last period is pomo!
    if(nPomo+1 >= longBreakInterval) {  // nPomo start from 0
      countDownFrom = longBreakTime;
      nPomo = 0;
    }else {
      countDownFrom = shortBreakTime;
    }
  }else {  // last period is break
    countDownFrom = pomoTime;
    nPomo++;
  }
  isBreak = !isBreak;
  isCurrentOver = false;
  // activate count down timer
  TimerAgent.restart();
}
void updateCountDownTimer() {
  // a litte weird here...
  cntSec = (60 - s) % 60;
  cntMin = countDownFrom - m + (cntSec==0?0:-1);
  if(m >= countDownFrom) {  // time is up
    cntMin = 0;  cntSec = 0;
    isCurrentOver = true;
    isPause = true;
    TimerAgent.pause();
  }
}
void draw() {
  // get the string to show
  if(showSec) {
    snprintf(countDownStringBuffer, 8, "%02d:%02d", cntMin, cntSec);
  }else {
    snprintf(countDownStringBuffer, 8, "%02d", cntMin);
  }

  // DEBUG
  // draw pause symbol
  const uint8_t margin = 5; const uint8_t pauseSize = 10;
  if(isPause) {
    display.fillRect(0+margin, display.height()-margin - pauseSize, pauseSize, pauseSize, SSD1306_WHITE);
    display.fillRect(0+margin + (pauseSize*0.8/2), display.height()-margin - pauseSize, pauseSize*0.2, pauseSize, SSD1306_BLACK);
  }
  // draw over symbol
  if(isCurrentOver) {
    display.fillRect(0+margin + pauseSize + margin, display.height()-margin - pauseSize, pauseSize, pauseSize, SSD1306_WHITE);
  }
  // draw pomo/break symbol
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0+margin + pauseSize + margin + pauseSize + margin, display.height()-margin - 16);
  display.write(!isBreak? "P" : "B");
  // draw nPomo
  display.setCursor(0+margin + pauseSize + margin + pauseSize + margin + 12 + margin, display.height()-margin - 16);
  display.write( nPomo +'0' );

  // calc
  if(isPause && isCurrentOver) {
    uint16_t current = millis();
    if(current - lastBlinkTime > 200) {
      lastBlinkTime = current;
      blinkDisappear = !blinkDisappear;
    }
  }else {
    blinkDisappear = false;
  }
  
  // draw
  display.setTextSize(2); // 12x16
  display.setTextColor(blinkDisappear? SSD1306_BLACK : SSD1306_WHITE);
  display.setCursor( display.width()/2-(strlen(countDownStringBuffer)*(6*2))/2, display.height()/2-(8*2)/2 );
  display.print(countDownStringBuffer);
}
void reset() {
  countDownFrom = pomoTime;
  isBreak = false;  isPause = true;  isCurrentOver = false;
  nPomo = 0;
}
void openMenu() {}

void drawDebugRect() {  // DEBUG
  if(SWAgent.isHolding()) {
    display.setTextSize(1);  // Draw 2X-scale text // 6,8
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 1);
    display.print(SWAgent.getLongPressDeltaTime());
    display.drawRoundRect(10, 2, SWAgent.getLongPressDeltaTime()/3000.0 * 108, display.height()-4, 5, SSD1306_WHITE);
  }
}

};

#endif