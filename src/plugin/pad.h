/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

www.r-site.net

buttons pad!
***/

#ifndef RSITE_ARDUINO_MENU_PAD
  #define RSITE_ARDUINO_MENU_PAD

#include "../menu.h"
#include "cancelField.h"

using namespace Menu;

class menuPadOptions {
public:
  static bool quitOnEsc;
  static bool accelSendEsc;
};

//custom field edit
class menuPad:public menuNode {
public:
  
};
