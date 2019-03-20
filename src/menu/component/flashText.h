/* -*- C++ -*- */
#pragma once

#include <menu.h>

template<typename O=typename MenuSystemDef<Out>::Empty>
class FlashTextDef:public O {
protected:
  const char *text PROGMEM;
public:
  using Out=typename O::OutDef;
  using OutDef=Out;
  using Def=MenuSystemDef<Out>;
  using Base=typename Def::Base;
  using Empty=typename Def::Empty;
  FlashTextDef(PGM_P t):text(t) {}
  // template<Roles>
  inline Out& out(Out& o) const {
    o<<reinterpret_cast<const __FlashStringHelper *>(text);
    return o;}
};
