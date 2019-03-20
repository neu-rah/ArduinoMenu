/* -*- C++ -*- */
#pragma once

#include <menu.h>
#include <base/roles.h>
/*
[item]: redirect to Out(item)
  Out(item): (Item to type + cycle)
------------------------------------------------ this is the output description, deliver it to MenuSystemDef
    Fmt: print<out,type> this is the perfect place because out->type are implicit (minimizing versions) and can translate anything not just roles
      MenuOut->RawOut

1)let role tags select current translator
  - runtime thing
2)make definitions open and hook up formats and translators with the output
  +this makes sense
  -cant make structures of open type, they have to move in at definition time
*/

// template<typename O=typename MenuSystemDef<Out>::Empty>
// class TextFmtDef:public O {
// public:
//   using Out=typename O::OutDef;
//   using OutDef=Out;
//   using Def=MenuSystemDef<Out>;
//   using Base=typename Def::Base;
//   using Empty=typename Def::Empty;
//   template<Roles role>
//   inline Out& out(Out& o) const {
//     return O::out<role>(o);
//   }
//   template<Roles role>
//   inline Out& out<role>(Out& o) const {
//     O::out<role>(o);
//     return o<<endl;
//   }
// };
