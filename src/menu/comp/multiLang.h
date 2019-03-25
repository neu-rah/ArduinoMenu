/* -*- C++ -*- */
#pragma once
#include <menu.h>

//an example of extending the base schema, here a multilanguage text

namespace Menu {
  template<typename Code,typename T=const char *>
  struct Lang {
    static Code sel;
    static T texts[][Code::cnt];
    template<size_t id,typename TO>
    struct Text:public Empty {
      inline void out(MenuOut &o) {
        TO(texts[id][Lang::sel]).out(o);
      }
    };
  };
  template<typename Code,typename T>
  Code Lang<Code,T>::sel=(Code)0;
};
