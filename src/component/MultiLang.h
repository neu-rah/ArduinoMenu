/* -*- C++ -*- */
#pragma once
#include <menu.h>

//an example of extending the base schema, here a multilanguage text

template<typename Out,typename Code>
struct Lang {
  using Def=MenuSystemDef<Out>;
  using Base=typename Def::Base;
  using Empty=typename Def::Empty;
  static Code sel;
  static const char *texts[][Code::cnt];
  template<size_t id,typename O=Empty>
  struct Text:public O {
    template<Roles>
    inline Out& out(Out& o) const {
      o<<texts[id][Lang::sel];
      return O::out(o);
    }
  };
};

template<typename Sys,typename Code>
Code Lang<Sys,Code>::sel=(Code)0;
