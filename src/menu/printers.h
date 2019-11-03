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
  inline static void printParent(It& it,Nav& nav,Out& out) {
    nav.level--;
    printMenu<It,Nav,Out>(it,nav,out);
    nav.level++;
  }
  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav,Out& out) {
    trace(MDO<<"FullPrinter::printMenu"<<endl);
    out.newView();
    out.template fmt<Roles::Panel,true,Nav,Out>(nav,out);
    out.template fmt<Roles::Menu,true,Nav,Out>(nav,out);

    //title
    out.template fmt<Roles::Prompt,true,Nav,Out>(nav,out);
    out.template fmt<Roles::Title,true,Nav,Out>(nav,out);
    it.template print<It,Nav,Out,Roles::Title>(it,nav,out);
    out.template fmt<Roles::Title,false,Nav,Out>(nav,out);
    out.template fmt<Roles::Prompt,false,Nav,Out>(nav,out);

    it.template printItems<It,Nav,Out,Roles::Body>(it,nav,out);

    out.template fmt<Roles::Menu,false,Nav,Out>(nav,out);
    out.template fmt<Roles::Panel,false,Nav,Out>(nav,out);
  }
};
