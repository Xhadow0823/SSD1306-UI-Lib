#ifndef __UI_H__
  #define __UI_H__
#include <Arduino.h>
#include <agents.h>
#include <display.h>

class UIComponentInterface {
protected:
public:
  UIComponentInterface() { }
  virtual void draw() { }
};

class UIWindow: public UIComponentInterface {
private:
  int8_t windowMargin = 2;
public:
  UIWindow() { }

  void draw() {
    display.fillRect(windowMargin, windowMargin, width(), height(), SSD1306_BLACK);
    display.drawRect(windowMargin, windowMargin, width(), height(), SSD1306_WHITE);
    // todo: (optional) add window border thickness
  }
  inline void setWindowMargin(int8_t margin) {
    windowMargin = margin;
  }
  inline int8_t getWindowMargin() {
    return windowMargin;
  }
  inline uint8_t width() {
    return display.width()-2*windowMargin;
  }
  inline uint8_t height() {
    return display.height()-2*windowMargin;
  }
  inline int16_t innerStartX() {
    return windowMargin + 1;  // 1 for the border line
  }
  inline int16_t innerStartY() {
    return windowMargin + 1;  // 1 for the border line
  }
  inline int16_t innerWidth() {
    return width() - 2*1;  // 1 for the border line
  }
  inline int16_t innerHeight() {
    return height() - 2*1;  // 1 for the border line
  }
};

class UIList: public UIComponentInterface {
private:
  int16_t _x = 0, _y = 0;
  int16_t _width = 32, _height = 32;
  int16_t cursor = -1;
  
  const char** itemListPtr = nullptr;
  uint8_t itemListSize = 0;
public:
  UIList() { }
  void draw() {
    // update()...
    // cursor += REAgent.getOffset();
    // cursor = (cursor+itemListSize)%itemListSize;

    // draw()...
    const int sizeScale = 1;
    const int lineHeight = sizeScale*8 + 3;  // 3: top border + top margin + bottom margin
    int indexOffset = 0;
    const int maxVisibleNum = _height/lineHeight;
    // predict wheather the item cursor pointed would overflow height or not
    indexOffset = (lineHeight*cursor+lineHeight > _height)? (cursor-(maxVisibleNum)+1) : 0;
    for(int8_t i = 0; (i+indexOffset) < itemListSize; i++) {
      if(i+1 > maxVisibleNum)  // item i is out of the bottom
        break;

      const bool highLight = (cursor == (i+indexOffset)) && SWAgent.isHolding();
      display.fillRect(_x, _y + lineHeight*i, _width, lineHeight, highLight? SSD1306_WHITE : SSD1306_BLACK);
      
      if(cursor == (i+indexOffset))
        display.drawRect(_x, _y + lineHeight*i, _width, lineHeight, SSD1306_WHITE);

      display.setTextSize(sizeScale);
      display.setTextColor(highLight? SSD1306_BLACK: SSD1306_WHITE);
      display.setCursor(_x + 2, _y + 2 + lineHeight*i);
      char buffer[32] = "";
      sprintf(buffer, "%2d.%s", (i+indexOffset+1), itemListPtr==nullptr? "BUTT" : itemListPtr[i+indexOffset]);
      display.print(buffer);

      // debug
      // static char sBuffer[128] = "";
      // if(cursor == i+indexOffset) {
      //   snprintf(sBuffer, 128, "{\ti=%d, offset=%d, \titemEnd=%d }", i, indexOffset, _y + lineHeight*i + lineHeight);
      //   Serial.println(sBuffer);
      //   delay(500);
      // }
    }
  }
  inline void setPosition(int16_t x, int16_t y) {
    _x = x;  _y = y;
  }
  inline void setSize(int16_t width, int16_t height) {
    _width = width;  _height = height;
  }
  inline void setItems(void* items, uint8_t size) {
    itemListPtr = (const char**)items;
    itemListSize = size;
  }
  inline void resetCursor() {
    cursor = -1;
  }
  inline int16_t getCursor() {
    return cursor;
  }
  inline void setCursor(int16_t pos) {
    cursor = pos;
  }
  inline int16_t itemListLength() {
    return itemListSize;
  }
  inline const char** itemNameArray() {
    return itemListPtr;
  }
};

#endif