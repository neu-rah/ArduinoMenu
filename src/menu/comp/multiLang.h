/* -*- C++ -*- */
#pragma once
#include <menu.h>

//an example of extending the base schema, here a multilanguage text

namespace Menu {
  template<typename Code,size_t langStringsCnt,typename T=const char *>
  struct Lang {
    static Code sel;
    using TextsType=T[langStringsCnt][Code::langsCnt];
    static TextsType texts;
    template<size_t id,typename TO>
    struct Text:public Empty {
      using This=Lang::Text<id,TO>;
      Text() {
        // Serial<<"building multilang text id:"<<id<<" lang:"<<sel<<endl;
      }
      inline void out(MenuOut &o) {
        Serial<<"using multilang text id:"<<id<<" sel:"<<(int)sel<<endl;
        Serial<<hex(texts)<<endl;
        // Serial<<hex(pgm_read_ptr(texts))<<endl;
        TextsType *tmp=(TextsType*)(pgm_read_ptr(texts));
        Serial<<"TMP:"<<endl;
        dumpPgm(Serial,tmp[id][Lang::sel],16);
        Serial<<"texts:"<<endl;
        dumpPgm(Serial,texts[id][Lang::sel],16);
        // TO(reinterpret_cast<const char*>(pgm_read_ptr(texts[id][Lang::sel]))).out(o);
      }
    };
  };
  template<typename Code,size_t langStringsCnt,typename T>
  Code Lang<Code,langStringsCnt,T>::sel=(Code)0;
};
