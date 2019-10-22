/* -*- C++ -*- */
#pragma once

#include "api.h"

////////////////////////////////////////////////////////////////////////////////
// printers
struct TextMeasure:public Void<> {
  template<typename T>
  static inline Idx measure(T o) {
    #ifdef ARDUINO
      return String(o).length();
    #else
      return _str(o);
    #endif
  }
  protected:
    #ifndef ARDUINO
    static inline Idx _str(const char*o){return std::string(o).length();}
    template<typename T>
    static inline Idx _str(T i){return std::string(std::to_string(i)).length();}
    #endif
};

template<typename P=TextMeasure>
struct FullPrinter:public P {
  using This=FullPrinter<P>;
  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav) {
    Out::newView();
    Out::template fmt<Roles::Panel,true,It,Out,Nav>(nav);
    Out::template fmt<Roles::Menu,true,It,Out,Nav>(nav);

    //title
    it.template fmt<Roles::Prompt,true,It,Out,Nav>(nav);
    it.template fmt<Roles::Title,true,It,Out,Nav>(nav);
    it.template print<It,Nav,Out,Roles::Title>(it,nav);
    it.template fmt<Roles::Title,false,It,Out,Nav>(nav);
    it.template fmt<Roles::Prompt,false,It,Out,Nav>(nav);

    it.template printItems<It,Nav,Out,Roles::Body>(it,nav);

    Out::template fmt<Roles::Menu,false,It,Out,Nav>(nav);
    Out::template fmt<Roles::Panel,false,It,Out,Nav>(nav);
  }
};
