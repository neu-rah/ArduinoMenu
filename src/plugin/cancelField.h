/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

www.r-site.net

implementing a cancelable field
this field restores the original value on escape

use it as:

altFIELD(cancelField,...);

the remaining parameters are the same as the regular FIELD

***/

#ifndef RSITE_ARDUINO_MENU_CANCEL_FIELD
  #define RSITE_ARDUINO_MENU_CANCEL_FIELD

#include "../menu.h"

using namespace Menu;

class cancelFieldOptions {
public:
  static bool quitOnEsc;
  static bool accelSendEsc;
};

//custom field edit
template<typename T>
class cancelField:public menuField<T> {
protected:
  T original;//to use when canceling
  bool editing;
public:
  cancelField(const menuFieldShadow<T>& shadow):menuField<T>(shadow),editing(false) {}
  void doNav(navNode& nav,navCmd cmd) override {
    if (!editing) {
      original=target();
      editing=true;
    }
    navCmd ncmd=cancelFieldOptions::accelSendEsc?escCmd:enterCmd;
    switch(cmd.cmd) {
      case selCmd:
      case idxCmd:
        Serial<<"IDX"<<endl;
      case escCmd:
        Serial<<"ESC"<<endl;
        if (cmd==escCmd||cancelFieldOptions::accelSendEsc)
          target()=original;
        menuField<T>::tunning=true;
        if (cancelFieldOptions::quitOnEsc||cmd==idxCmd) {
          cmd=ncmd;
          editing=false;
        } else
          return menuField<T>::doNav(nav,cmd);
        break;
      case enterCmd:
        Serial<<"ENTER"<<endl;
        if (menuField<T>::tunning||options->nav2D||!tune())
          editing=false;
        break;
      default: break;
    }
    menuField<T>::doNav(nav,cmd);
  }
};

#endif
