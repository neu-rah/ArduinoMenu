/* -*- C++ -*- */
#pragma once

#include "api.h"

////////////////////////////////////////////////////////////////////////////////
// printers
struct TextMeasure:public Void {
  template<typename T>
  static inline Idx measure(T o) {
    #ifdef ARDUINO
      return String(o).length();
    #else
      return _str(o);
    #endif
  }
  inline static constexpr int maxCharWidth() {return 1;}
  inline static constexpr int maxCharHeight() {return 1;}
  int textWidth(const char*s) const {return measure(s);}
  protected:
    #ifndef ARDUINO
    static inline Idx _str(const char*o){return std::string(o).length();}
    template<typename T>
    static inline Idx _str(T i){return std::string(std::to_string(i)).length();}
    #endif
};

//to avoid passing the output object back and forth a printer must be a top level vomposition
//because it needs do call back data for item printing
template<typename P=TextMeasure>
struct FullPrinter:public P {
  using This=FullPrinter<P>;
  template<typename It,typename Nav>
  inline void printParent(It& it,Nav& nav) {
    //this is ugly but short
    nav.level--;
    This::printMenu<It,Nav>(it,nav);
    nav.level++;
  }
  template<typename It,typename Nav>
  void printMenu(It& it,Nav& nav) {
    trace(MDO<<"FullPrinter::printMenu"<<endl);
    P::newView();
    P::template fmt<Roles::Panel,true>();
    P::template fmt<Roles::Menu,true>();

    //title
    P::template fmt<Roles::Item,true>();
    P::template fmt<Roles::Title,true>();
    it.template print<This,Roles::Title>(*this);
    P::template fmt<Roles::Title,false>();
    P::template fmt<Roles::Item,false>();

    //TODO: position top here
    This::posTop(nav);
    it.template printItems<Nav,This>(nav,*this,0,This::top());

    P::template fmt<Roles::Menu,false>();
    P::template fmt<Roles::Panel,false>();
  }
  template<typename It>
  void printItem(It& it,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    P::clrLine(P::posY());
    P::template fmt<Roles::Item,  true >(n,s,e,m);
    P::template fmt<Roles::Index, true >(n,s,e,m);
    P::template fmt<Roles::Index, false>(n,s,e,m);
    P::template fmt<Roles::Cursor,true >(n,s,e,m);
    P::template fmt<Roles::Cursor,false>(n,s,e,m);
    //TODO: need a custom print here! however received item is NOT the top
    //we can pass parameters along or use some sort of CRTP
    //however CRTP does not mix with Mixins, they are oposites
    //using a forward reference instead
    it.template printItem<This,Roles::Item>(*this,n,s,e,m);
    // it.print(*this);
    P::template fmt<Roles::Item,false>(n,s,e,m);
  }
};
