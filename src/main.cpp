/**
 *                             TX1 ************** VIN
 *                             RX0                GND -- Battery GND
 *                             RST                RST
 *                             GND                5V --- Battery 5V
 *              Encoder CLK     D2                A7
 *              Encoder CLK --- D3                A6
 *               Encoder DT --- D4  Arduino Nano  A5 --- OLED 128x64 SCK
 *       Vibration Motor IN --- D5   ATmega328P   A4 --- OLED 128x64 SDA
 *                              D6                A3
 *                              D7                A2
 *                              D8                A1
 *                              D9                A0
 *                             D10     ......     REF
 *                             D11     :    :     3V3 -- not working now...
 *                             D12 ****:    :**** D13
*/
#include <Arduino.h>
#include <ISRs.h>
#include <agents.h>
#include <multiApp.h>
#include <display.h>

size_t calcFreeMemorySpaceSize() {
  unsigned int target = 512, delta = 512;
  unsigned char* spacePtr = nullptr;
  while(!(delta <= 0)) {
    spacePtr = (unsigned char*)malloc((size_t)target * sizeof(unsigned char));
    if(spacePtr) {  // free space size > target
      target += delta;
      free(spacePtr);
    }else {         // free space size < target
      delta = delta / 2;
      target -= delta;
    }
  }
  return (size_t)target;
}

size_t calcFreeMemory2() {
  extern int __heap_start, *__brkval;
  uint8_t v;
  return (int)&v - (__brkval==0? (int) &__heap_start : (int) __brkval);  // include the heap
}

class MultiAppManager: AppInterface {
private:
  AppInterface* app = nullptr;  
public:
  // const PROGMEM char* menuItems[4] = { "Demo0", "Dice", "Pomodoro", "Menu" };  // <- wrong way
  // inline static const char* name PROGMEM = "MultiAppManager";  // not work
  // const char* name PROGMEM = "MultiAppManager";  // not work
  
  inline const char* name() {  return PSTR("MultiAppManager");  }
  MultiAppManager() {
    // load(0), then delete AppInterfece will work when delete Demo0
    // load(1), the delete operator will not work when delete Demo0
    // load(2), the delete operator will not work when delete both Demo0 and Dice
    // load(2);
    // Conclusion: in constructor, every memory new here will not be a dynamic allocated memory but a static member...
    // maybe the original memory become a "Hole" (?
  }

  void setup() {
    if(app) {
      app->setup();
    }else {
      // load(1);  // Good
      load(3);  // Good
    }
  }

  void loop() {
    if(app) {
      app->loop();
      // checkc app->exit()
      if(app->exit()){
        Serial.println(F("EXIT"));
        load(0);
      }
    }else {
      UIHelper.loop();
      if(UIHelper.getSelectedItem()) {
        load(UIHelper.getSelectedItem());
      }
    }
  }

//  EXAMPLE
  void load(uint8_t appIdx = 0) {
    if(app) {
      delete app;  ///////////////
      app = nullptr;
    }
    if(0){}  // skip
    else if(appIdx == 0) {
      setMenu();
      // app = menu;
    }
    else if(appIdx == 1) {
      app = new Demo0();
      app->setup();
    }
    else if(appIdx == 2){
      app = new Dice();
      app->setup();
    }
    else if(appIdx == 3) {
      app = new Pomodoro();
      app->setup();
    }
    else {
      setMenu();
      // app = menu
    }
  }
  void setMenu() {
    const static char string_0[] PROGMEM = "Demo0";
    const static char string_1[] PROGMEM = "Dice";
    const static char string_2[] PROGMEM = "Pomodoro";
    const static char string_3[] PROGMEM = "Menu";
    const static char * const menuItems[] PROGMEM = { string_0, string_1, string_2, string_3 };
    UIHelper.setMenuItems(menuItems, sizeof(menuItems)/sizeof(const char*));
  }
  
} multi;

void setup() {
  // ========== init Serial ==========
  Serial.begin(9600);
  // ========== init Serial end ==========

  // ========== init SSD1306 ==========
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // ========== init SSD1306 end ==========

  // move to REAgent
  // ========== init rotary encoder ==========
  #define CLK 2
  #define DT 3
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(0, readEncoder, CHANGE);
  attachInterrupt(1, swClick, CHANGE);
  // ========== init rotary encoder end ==========

  // set timer2 interrupt at 1kHz
  TimerAgent.init();

  
  Serial.println(F("[START]"));
  multi.setup();

  display.display();
}




void loop() {
  // routine of agents
  SWAgent.update();
  REAgent.update();

  display.clearDisplay();

  multi.loop();
  
  // show free memory space
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(display.width()-6*3, 0);
  display.print(calcFreeMemorySpaceSize());

  // routine of display
  display.display();
  // routine of delay
  delay(10);
}