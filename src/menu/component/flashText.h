/* -*- C++ -*- */
#pragma once

#include <menu.h>

template<typename Out,typename O=typename MenuSystemDef<Out>::Empty>
class FlashText:public O {
protected:
  const char *text PROGMEM;
public:
  using Def=MenuSystemDef<Out>;
  using Base=typename Def::Base;
  using Empty=typename Def::Empty;
  FlashText(PGM_P t):text(t) {}
  // template<Roles>
  inline Out& out(Out& o) const {
    o<<text;
    return o;}
};
