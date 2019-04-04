/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// multilanguage field...
// TODO: change language table to a menu structure

#include <menu.h>

//an example of extending the base schema, here a multilanguage text

static int langSel=0;

namespace Menu {
  template<typename T> //=const char *>
  struct Lang {
    public:
      using This=Lang<T>;
      using TextsType=T;
    protected:
      TextsType* texts;
    public:
      Lang(TextsType* lt):texts(lt) {}
      template<This& lang,size_t id,typename O>
      class Text:public O {
      public:
        using O::O;
        inline void out(MenuOut &o) const {lang.texts[id].out(o);}
      };
      inline void setLangTable(TextsType* lt) {texts=lt;}
  };
};
