#ifndef __MULTIAPP_CORE_H__
  #define __MULTIAPP_CORE_H__
#include <Arduino.h>
#include <agents.h>
#include <UI.h>
#include <display.h>
#include <multiApp/AppInterface.h>
#include <multiApp/apps.h>
#include <multiApp/UIHelper.h>

class MultiAppManager: AppInterface {
private:
  AppInterface* app = nullptr;

public:
  const char* _name;
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
    _name = PSTR("MultiAppManager");
  }

  void setup() {
    if(app) {
      app->setup();
    }else {
      // load(1);  // Good
      load(1);  // Good
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

#endif