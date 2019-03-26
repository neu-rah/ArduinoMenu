/* -*- C++ -*- */
#pragma once
#include <menu.h>

//an example of extending the base schema, here a multilanguage text

static int langSel=0;

namespace Menu {
  template<typename TO,typename T=const char *>
  struct Lang {
    using TextsType=T;
    Lang(TextsType& lt) {texts=&lt;}
    template<size_t id>
    class Text {
    public:
      inline void out(MenuOut &o) {TO(texts[id]).out(o);}
    };
    static inline void setLangTable(TextsType lt) {texts=lt;}
  protected:
    static TextsType texts;
  };
};
