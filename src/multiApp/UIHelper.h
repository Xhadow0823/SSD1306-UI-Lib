#ifndef __MULTIAPP_UIHELPER_H__
  #define __MULTIAPP_UIHELPER_H__
#include <Arduino.h>
#include <agents.h>
#include <UI.h>
#include <multiApp/AppInterface.h>


// TODO: make UIHelper better
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
  
  inline const char* name() {  return PSTR("UIHelper");  }

  void setup() {
    // nothing...
  }
  void loop() {
    openMenu();
  }
  // return selectd index of item (start from 1). return 0 if nothing is selected
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
  void setMenuItems(const char* const * items, size_t size) {
    list.setItems(items, size);
  }
  inline uint16_t getMenuLength() {
    return list.itemListLength();
  }
} UIHelper;

#endif