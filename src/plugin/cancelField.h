/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
www.r-site.net

implementing a cancelable field
this field restores the original value on escape

use it as:

altFIELD(cancelField,...);

the remaining parameters are the same as the regular FIELD

***/

#ifndef RSITE_ARDUINO_MENU_CANCEL_FIELD
  #define RSITE_ARDUINO_MENU_CANCEL_FIELD

#include "../menuDefs.h"

using namespace Menu;

class cancelFieldOptions {
public:
  static bool quitOnEsc;
  static bool accelSendEsc;
};

//custom field edit
template<typename T>
class cancelField:public menuField<T> {
public:
  T original;//to use when canceling
  bool editing;
  cancelField(constMEM menuFieldShadow<T>& shadow):menuField<T>(shadow),editing(false) {}
  void doNav(navNode& nav,navCmd cmd) override {
    if (!editing) {
      original=menuField<T>::target();
      editing=true;
    }
    switch(cmd.cmd) {
      case selCmd:
      case idxCmd:
        //MENU_DEBUG_OUT<<"IDX"<<endl;
        menuField<T>::tunning=true;//prepare for exit
        return doNav(nav,cancelFieldOptions::accelSendEsc?escCmd:enterCmd);
      case escCmd:
        editing=!cancelFieldOptions::quitOnEsc;
        //menuField<T>::tunning=false;
        //MENU_DEBUG_OUT<<"ESC"<<endl;
        menuField<T>::target()=original;
        break;
      case enterCmd:
        //MENU_DEBUG_OUT<<"ENTER"<<endl;
        if (menuField<T>::tunning||nav.root->nav2D||!menuField<T>::tune())
          editing=false;
        break;
      default: break;
    }
    menuField<T>::doNav(nav,cmd);
  }
};

#endif
