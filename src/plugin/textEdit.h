/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

www.r-site.net

testing text edit field
***/

#ifndef RSITE_ARDUINO_MENU_TEXT_EDIT
  #define RSITE_ARDUINO_MENU_TEXT_EDIT

#include "../menu.h"
#include "cancelField.h"

using namespace Menu;

class textEditOptions {
public:
  static bool quitOnEsc;
  static bool accelSendEsc;
};

//custom field edit
class textEdit:public navTarget {
public:
  String value;
  String original;
  idx_t cursor=0;
  bool editing;
  textEdit(const promptShadow& shadow,char* value):navTarget(shadow),value(value) {}
  void doNav(navNode& nav,navCmd cmd) override {
    Serial.print("textEdit::doNav:");
    Serial.println(cmd);
    if (!editing) {
      original=value;
      editing=true;
    }
    switch(cmd.cmd) {
      case selCmd:
      case idxCmd:
        return navTarget::doNav(nav,cancelFieldOptions::accelSendEsc?escCmd:enterCmd);
      case escCmd:
        editing=!cancelFieldOptions::quitOnEsc;
        value=original;
        break;
      case enterCmd:
        editing=false;
        break;
      default: break;
    }
    navTarget::doNav(nav,cmd);
  }
};

#endif
