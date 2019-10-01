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
  inline static void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(MDO<<"FullPrinter::printMenu");
      out.template fmt<Roles::Panel,true,It,Out,Nav>(0,nav,out);
      out.template fmt<Roles::Menu,true,It,Out,Nav>(0,nav,out);

      //title
      it.template fmt<Roles::Title,true,It,Out,Nav>(0,nav,out);
      it.template print<It,Nav,Out,Roles::Title>(it,nav,out,ref,n);
      it.template fmt<Roles::Title,false,It,Out,Nav>(0,nav,out);

      if (out.isRange()) {
        //ensure that selected option is withing visible range
        while(out.top()>nav.pos())
          out.setTop(out.top()-1);
        while(nav.pos()>=out.top()+out.freeY())
          out.setTop(out.top()+1);
      }

      for(Idx n=out.top();n<it.size();n++) {
        if (!out.freeY()) break;
        out.clrLine(out.posY());
        it.template fmt<Roles::Prompt,true ,It,Out,Nav>(n,nav,out);
        it.template fmt<Roles::Index, true ,It,Out,Nav>(n,nav,out);
        it.template fmt<Roles::Index, false,It,Out,Nav>(n,nav,out);
        it.template fmt<Roles::Cursor,true ,It,Out,Nav>(n,nav,out);
        it.template fmt<Roles::Prompt,false,It,Out,Nav>(n,nav,out);
        it.template printItem<It,Nav,Out,Roles::Prompt>(it,nav,out,ref,n);
        out.template fmt<Roles::Prompt,false,It,Out,Nav>(n,nav,out);
      }

      out.template fmt<Roles::Body,true,It,Out,Nav>(0,nav,out);
      out.template fmt<Roles::Body,false,It,Out,Nav>(0,nav,out);
      out.template fmt<Roles::Menu,false,It,Out,Nav>(0,nav,out);
      out.template fmt<Roles::Panel,false,It,Out,Nav>(0,nav,out);
  }
};
