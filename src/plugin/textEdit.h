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
#include "cancelField.h"

using namespace Menu;

class textEditOptions {
public:
  static bool quitOnEsc;
  static bool accelSendEsc;
};

//custom field edit
template<typename T>
class textEdit:public cancelField<T> {
public:
  textEdit(const menuFieldShadow<T>& shadow):menuField<T>(shadow) {}
  void doNav(navNode& nav,navCmd cmd) override {
    if (!editing) {
      original=target();
      editing=true;
    }
    switch(cmd.cmd) {
      case selCmd:
      case idxCmd:
        menuField<T>::tunning=true;//prepare for exit
        return menuField<T>::doNav(nav,cancelFieldOptions::accelSendEsc?escCmd:enterCmd);
      case escCmd:
        menuField<T>::tunning=editing=!cancelFieldOptions::quitOnEsc;
        target()=original;
        break;
      case enterCmd:
        if (menuField<T>::tunning||options->nav2D||!menuField<T>::tune())
          editing=false;
        break;
      default: break;
    }
    menuField<T>::doNav(nav,cmd);
  }
};

#endif
