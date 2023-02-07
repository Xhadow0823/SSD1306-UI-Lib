#ifndef __MULTIAPP_APPINTERFACE_H__
  #define __MULTIAPP_APPINTERFACE_H__

class AppInterface {
protected:
    bool __exit = false;
public:
    AppInterface() { }
    virtual ~AppInterface() { }  // IMPORTANT
    // inline static const char* PROGMEM name = "AppInterface";

    virtual void setup() { }
    virtual void loop() { }
    virtual inline bool exit() { return __exit; }
};

#endif