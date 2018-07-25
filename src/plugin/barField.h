/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

www.r-site.net

custom field edit (experimental)
this edit is an example of a navTarget element ,
nomally drawn as part of the parent (inplace edit)
being draw on its own screen (quite radical change)
the element still draws inline when not editing

the ideal of settings would be by output device
and as functions instead of data
...

eventually on this schema plugins will speciallize on device types
but hopefully simpler to write

use it as:

altFIELD(barField,...)

or

BARFIELD(...)

the remaining parameters are the same as the regular FIELD

***/

#ifndef RSITE_ARDUINO_MENU_BAR_FIELD
  #define RSITE_ARDUINO_MENU_BAR_FIELD

#include "../menuDefs.h"
#include "cancelField.h"

using namespace Menu;

#define BARFIELD(...) altFIELD(barField,__VA_ARGS__)

class barFieldOptions {
public:
  //these are made string to allow unicode,
  //BUT you should only provide ONE character
  //otherwise stuff will overflow!
  static const char* fill;
  static const char* empty;
};

template<typename T>
class barField:public cancelField<T>,public barFieldOptions {
public:
  barField(constMEM menuFieldShadow<T>& shadow):cancelField<T>(shadow) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override {
    bool ed=this==root.navFocus;
    if (ed) {
      //out.clear();
      out.clearLine(0);
      out.setCursor(0,0);
      if (out.maxX()>1) {
        cancelField<T>::printTo(root,sel,out,idx,len);
        out.clearLine(1);
        out.setCursor(0,1);
      }
      int bar=map(menuField<T>::target(),menuField<T>::low(),menuField<T>::high(),0,out.maxX());
      for (int n=0;n<bar;n++) out.print(fill);
      for (int n=bar;n<out.maxX();n++) out.print(empty);
      return 0;
    } else return cancelField<T>::printTo(root,sel,out,idx,len);
  }
};

#endif
