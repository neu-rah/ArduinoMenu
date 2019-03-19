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

// template<typename Out,typename O=typename MenuSystemDef<Out>::Empty>
// class JsonFmt:public O {
// public:
//   using Def=MenuSystemDef<Out>;
//   using Base=typename Def::Base;
//   using Empty=typename Def::Empty;
//   FlashText(PGM_P t):text(t) {}
//   // template<Roles>
//   inline Out& out(Out& o) const {
//     o<<text;
//     return o;}
// };
