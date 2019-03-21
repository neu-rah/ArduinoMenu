/* -*- C++ -*- */
#pragma once

#include <menu.h>

template<typename O=MenuSystemDef<RawOutType>::Empty>
class FlashTextDef:public O {
protected:
  const char *text PROGMEM;
public:
  using SysDef=typename O::SysDef;
  using Out=typename SysDef::OutDef;
  using OutDef=Out;
  using Def=MenuSystemDef<Out>;
  using Base=typename Def::Base;
  using Empty=typename Def::Empty;
  FlashTextDef(PGM_P t):text(t) {}
  // template<Roles>
  inline RawOut& out(RawOut& o) const {
    o<<reinterpret_cast<const __FlashStringHelper *>(text);
    return o;}
};
