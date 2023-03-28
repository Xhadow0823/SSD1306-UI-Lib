#ifndef __MULTIAPP_APPS_H__
  #define __MULTIAPP_APPS_H__
#include <Arduino.h>
#include <UI.h>
#include <multiApp/AppInterface.h>
#include <multiApp/core.h>
#include <multiApp/UIHelper.h>
#include <multiApp/buzzerHelper.h>
#include <eepromAgent.h>

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
    // inline static const char* PROGMEM name = "Dice";
    inline const char* name() {  return PSTR("Dice");  }
    // const PROGMEM char * menuItems[4] = { "font size", "change mode", "change categories", "exit" };
    // for denug
    void setup() {
      // set the UI helper menu item and functions
      // const static char * const menuItems[4] PROGMEM = { "font size", "change mode", "change category", "exit" };  // <- wrong way, this will use SRAM space
      // const static char * const menuItems[4] PROGMEM = { PSTR("font size"), PSTR("change mode"), PSTR("change category"), PSTR("exit") };  // not work
      const static char string_0[] PROGMEM = "font size";
      const static char string_1[] PROGMEM = "change mode";
      const static char string_2[] PROGMEM = "change category";
      const static char string_3[] PROGMEM = "exit";
      const static char * const menuItems[] PROGMEM = { string_0, string_1, string_2, string_3 };
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
    inline const char* name() {  return PSTR("Demo0");  }

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
  
  struct Config {  // save in eeprom
    uint8_t displayRotation;  // 0
    uint8_t pomoTime,          // 25
            shortBreakTime,    // 5
            longBreakTime,     // 15
            longBreakInterval; // 4
    bool showSec;  // true
  };

  // temp
  char countDownStringBuffer[8] = "";
  uint16_t lastBlinkTime = 0;
  bool blinkDisappear = false;

  // helper
  BuzzerHelper buzzer = BuzzerHelper();
public:
inline const char* name() {  return PSTR("Pomodoro");  }

void setup() {
  // initialize everything
  initialize();
}
void loop() {
  // handle click event
  if(SWAgent.isClicked()   && !isMenuOpen) {  // todo: refactor this block
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
      isMenuOpen = true;
      SWAgent.clearClicked();  // search a better solution for open menu problem
    }
  }
  // check time
  updateCountDownTimer();
  // draw
  draw();
  drawMenu();
  drawDebugRect();
}
void gotoNext() {
  Config config = getConfig();
  if(!isBreak) {  // last period is pomo!
    if(nPomo+1 >= config.longBreakInterval) {  // nPomo start from 0
      countDownFrom = config.longBreakTime;
    }else {
      countDownFrom = config.shortBreakTime;
    }
  }else {  // last period is break
    // countDownFrom = pomoTime;
    countDownFrom = config.pomoTime;
    (++nPomo) %= config.longBreakInterval;
  }
  isBreak = !isBreak;
  isCurrentOver = false;
  // reactivate count down timer
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
uint8_t interval = 125;
void draw() {
  // get the string to show
  if(getConfig().showSec) {  // is this getConfig too slow ?
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
    buzzer.play(current);
  }else {
    blinkDisappear = false;
    buzzer.stop();
  }
  // debug for buzzer
  interval = constrain(interval+REAgent.getOffset(), 0, 255);
  if(REAgent.getOffset()) {
    Serial.println( buzzer.setInterval(interval) );
  }
  
  // draw
  display.setTextSize(2); // 12x16
  display.setTextColor(blinkDisappear? SSD1306_BLACK : SSD1306_WHITE);
  display.setCursor( display.width()/2-(strlen(countDownStringBuffer)*(6*2))/2, display.height()/2-(8*2)/2 );
  display.print(countDownStringBuffer);
}


void initialize() {
  // initialize all status
  TimerAgent.stop();
  countDownFrom = getConfig().pomoTime;
  isBreak = false;  isPause = true;  isCurrentOver = false;
  nPomo = 0;
  // load config
  Config config = getConfig();
  display.setRotation(config.displayRotation);
  // set all components
  setMenuItems();
  buzzer.setTable(8, 1,0,1,0,0,0,0,0);  // █░█░░░░░
}

void drawDebugRect() {  // DEBUG
  if(SWAgent.isHolding()) {
    display.setTextSize(1);  // Draw 2X-scale text // 6,8
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 1);
    display.print(SWAgent.getLongPressDeltaTime());
    display.drawRoundRect(10, 2, SWAgent.getLongPressDeltaTime()/3000.0 * 108, display.height()-4, 5, SSD1306_WHITE);
  }
}

// ========== menu ==========
void drawMenu() {
  if(isMenuOpen) {
    UIHelper.openMenu();
    if(SWAgent.isClicked()) {
      switch(UIHelper.getSelectedItem()) {
        default:
        case 1: // close menu
          isMenuOpen = false;
          break;
        case 2: // skip
          isMenuOpen = false;
          isCurrentOver = true;  // wait for click to call gotoNext()
          break;
        case 3: // rotate
          menuDoRotate();
          break;
        case 4: // reset
          menuDoReset();
          Serial.println(F("reset config"));
          break;
        case 5: // exit
          // digitalWrite(11, LOW);
          __exit = true;
          break;
      }
    }
  }
}
void setMenuItems() {
  const static char string_0[] PROGMEM = "close";
  const static char string_1[] PROGMEM = "skip";
  const static char string_2[] PROGMEM = "rotate";
  const static char string_3[] PROGMEM = "reset";
  const static char string_4[] PROGMEM = "exit";
  const static char * const mainMenuItems[] PROGMEM = { string_0, string_1, string_2, string_3, string_4 };
  UIHelper.setMenuItems(mainMenuItems, 5);
}
void menuDoRotate() {
  uint8_t temp = 0;
  display.setRotation(temp = (display.getRotation()+1)%4);
  Config config =  getConfig();
  config.displayRotation = temp;
  updateConfig(config);
}
void menuDoReset() {
  resetConfig();
  initialize();
}
// ========== menu end ==========

// ========== config management ==========
inline const struct Config getConfig() const {
  Config temp;
  EEPROM.get(EEPROM_POMODORO_ADDRESS_START_AT, temp);
  return temp;
}
inline const struct Config resetConfig() const {
  Config temp{ 0, 25, 5, 15, 4, true };
  return EEPROM.put(EEPROM_POMODORO_ADDRESS_START_AT, temp);
}
inline const struct Config updateConfig(const struct Config config) const {
  return EEPROM.put(EEPROM_POMODORO_ADDRESS_START_AT, config);
}
// ========== config management end ==========
};


#endif