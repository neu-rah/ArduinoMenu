/* -*- C++ -*- */
#pragma once

#include <menu.h>

template<typename O=typename MenuSystemDef<Out>::Empty>
class JsonFmtDef:public O {
public:
  using Out=typename O::OutDef;
  using OutDef=Out;
  using Def=MenuSystemDef<Out>;
  using Base=typename Def::Base;
  using Empty=typename Def::Empty;
  template<Roles>
  inline RawOut& out(RawOut& o) const {
    o<<text;
    return o;
  }
};
