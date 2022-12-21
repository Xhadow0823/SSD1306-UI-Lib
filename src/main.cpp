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

// Demo0 demo0;

class MultiAppManager: AppInterface {
private:
  AppInterface* app = nullptr;
  const char* menuItems[4] = { "Demo0", "Dice", "Menu" };
public:
  inline static const char* name = "MultiAppManager";

  MultiAppManager() {
    load(1);
  }

  void setup() {
    if(app) {
      app->setup();
    }
  }

  void loop() {
    if(app) {
      app->loop();
      // checkc app->exit()
      if(app->exit()){
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
    else {
      setMenu();
      // app = menu
    }
  }
  void setMenu() {
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

  // ========== init rotary encoder ==========
  #define CLK 2
  #define DT 3
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(0, readEncoder, CHANGE);
  attachInterrupt(1, swClick, CHANGE);
  // ========== init rotary encoder end ==========

  // ========== set timer2 interrupt at 1kHz ==========
  cli();
  TCCR2A = 0;  // set entire TCCR2A register to 0
  TCCR2B = 0;  // same for TCCR2B
  TCNT2  = 0;  //initialize counter value to 0
  OCR2A = 249;  // = (16*10^6) / (64*1000) - 1 (must be <256)
  TCCR2A |= (1 << WGM21);  // turn on CTC mode
  TCCR2B |= (1 << CS22);
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  sei();
  // ========== set timer2 interrupt at 1kHz end ==========

  
  multi.setup();
  display.display();
}




void loop() {
  // routine of agents
  SWAgent.clearClicked();
  SWAgent.update();
  REAgent.clearOffset();
  REAgent.update();

  display.clearDisplay();

  

  // demo0.loop();
  // UIHelper0.openMenu();
  multi.loop();
  
  // routine of display
  display.display();

  // routine of delay
  delay(10);
}