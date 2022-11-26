#ifndef __MULTI_APP_H__
    #define __MULTI_APP_H__
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <agents.h>
#include <string.h>
#include <UI.h>

// TODO: make UIHelper better
// TODO: add a multi-app manager to run two apps

class AppInterface {
protected:
    const char* __name = nullptr;
    bool __exit = false;
    // Adafruit_SSD1306& display;
public:
    AppInterface() { }

    virtual void setup() { }
    virtual void loop() { }
    virtual inline const char* name() { return __name; }
    virtual inline void exit() { __exit = true; }
};

class __UIHelper: public AppInterface {
private:
  Adafruit_SSD1306& display;
  long cursor = 0;

  UIWindow* window;
  UIList* list;
public:
  __UIHelper(Adafruit_SSD1306& display): display(display) {
    window = new UIWindow(display);
    list = new UIList(display);
  }
  __UIHelper() { }
  void loop() {
    // openMenu();
  }
  void openMenu() {
    cursor += REAgent.getOffset();
    window->draw();
    list->setPosition(window->innerStartX()+1, window->innerStartY()+1);
    list->setSize(window->innerWidth()-2, window->innerHeight()-2);
    static const char* items[] = {"kiwi", "dodo", "peacock", "exit"};
    list->setItems(items, sizeof(items) / sizeof(const char*));
    list->draw();



    // drawButtons(0, 0, 0, 0, SWAgent.isHolding());
    
    // if(SWAgent.isClicked()) {
    //   display.startscrollright(0x00, 0x0F);
    //   delay(2000);
    //   display.stopscroll();
    // }

    // drawLabel(2+1+2+1+28+1 + 3, 2+1+2+1 + 1, 50, 22, 1, "Beach");
    
  }
  void openMenu(Adafruit_SSD1306& display) {
    cursor += REAgent.getOffset();

    
    // const int nItems = 5;
    // const int buttonHeight=5;
    // const int buttonMargin = (display.height()-2*margin-nItems*buttonHeight) / nItems;
    // for(int i = 0; i < nItems; i++) {
    //   if((cursor%nItems) == i) {
    //     display.fillRoundRect(margin, margin + buttonMargin*(i+1) + buttonHeight*i, 25, buttonHeight, 3, WHITE);
    //   }else {
    //     display.drawRoundRect(margin, margin + buttonMargin*(i+1) + buttonHeight*i, 25, buttonHeight, 3, WHITE);
    //   }
    // }
  }

  void drawWindow() {
    const int8_t windowMargin = 2;
    display.fillRect(windowMargin, windowMargin, display.width()-2*windowMargin, display.height()-2*windowMargin, SSD1306_BLACK);
    display.drawRect(windowMargin, windowMargin, display.width()-2*windowMargin, display.height()-2*windowMargin, SSD1306_WHITE);
  }

  void drawButtons(int16_t x, int16_t y, int16_t w, int16_t h, bool isPressed = false) {
    const int8_t windowW = 128 - 2*2 -1*2, 
                 windowH =  32 - 2*2 -1*2,
                 buttonMargin = 2;
    display.fillRect(2+1+buttonMargin, 2+1+buttonMargin, 28, 22, isPressed? SSD1306_WHITE : SSD1306_BLACK);
    display.drawRect(2+1+buttonMargin, 2+1+buttonMargin, 28, 22, SSD1306_WHITE);

    display.setTextSize(1); // Draw 2X-scale text // 6,8 "12,16"
    display.setTextColor(isPressed? SSD1306_BLACK : SSD1306_WHITE);
    display.setCursor(2+1+buttonMargin + 2, 2+1+buttonMargin + 1 + 11 - 4);
    display.print("BUTT");
  }

  void drawLabel(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t size = 1, const char* content = nullptr) {
    // calc the string width 
    uint8_t maxStringLen = w / (6*size+1);  maxStringLen>strlen(content)? strlen(content) : maxStringLen;
    char buffer[32] = "";
    
    display.setTextSize(size); // 6,8 12,16 ...
    display.setCursor(x, y + (h-8*size)/2);
    display.print(strncpy(buffer, content, maxStringLen<32? maxStringLen : 31));
  }

  void clickOnMenu() {
    // set the function of menu item
  }
} UIHelper;

class Dice : AppInterface {
private:
    const char* name = "Dice";
    Adafruit_SSD1306& display;

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
    Dice(Adafruit_SSD1306& displayPtr): display(displayPtr) { }
    void setup() {

    }
    void loop() {
      if(mode != Mode::setting) {
        __xOffset += REAgent.getOffset();
      }
      if(SWAgent.getLongPressDeltaTime() > 1000) {
        mode = Mode::setting;
      }
      else if(SWAgent.isClicked()){
        if(mode == Mode::setting){ 
          UIHelper.clickOnMenu();
          mode = Mode::standBy;
        }else {
          rollingStartTime = millis();
          mode = Mode::rolling;
        }
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
        UIHelper.openMenu(display);
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