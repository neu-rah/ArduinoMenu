/* -*- C++ -*- */
#pragma once

#include <menu.h>
/*
[item]: redirect to Out(item)
  Out(item): (Item to type + cycle)
------------------------------------------------ this is the output description, deliver it to MenuSystemDef
    Fmt: print<out,type> this is the perfect place because out->type are implicit (minimizing versions) and can translate anything not just roles
      MenuOut->RawOut
*/

template<typename O=typename MenuSystemDef<Out>::Empty>
class JsonFmtDef:public O {
public:
  using Out=typename O::OutDef;
  using OutDef=Out;
  using Def=MenuSystemDef<Out>;
  using Base=typename Def::Base;
  using Empty=typename Def::Empty;
  template<Roles>
  inline Out& out(Out& o) const {
    o<<text;
    return o;
  }
};
