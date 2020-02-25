/* -*- C++ -*- */
#pragma once
/*this is a debug monitor of menu internal state, not a regular output!*/

#include "../../../staticMenu.h"
#include "../../ansiCodes.h"

namespace Menu {
  template<typename Dev,Dev& dev,typename Out>
  struct ANSIMonitor {
    template<typename O>
    struct Part:O {
      Out out;
      using This=ANSIMonitor<Dev,dev,Out>::Part<O>;
      using Base=O;
      using Target=Out;
      template<typename It,typename Nav,Op op=Op::Printing>
      void printMenu(It& it,Nav& nav,bool fullPrint) {
        // ansiFill(1,1,80,25)(*this);

        xy(1,1)(*this);
        eraseLine()(*this);
        MDO<<"AM5 Debug-monitor";

        xy(1,2)(*this);
        eraseLine()(*this);
        MDO<<"level:"<<nav.level;

        xy(1,3)(*this);
        eraseLine()(*this);
        MDO<<"path:[";
        for(int n=0;n<=nav.level;n++) {
          MDO<<(n?",":"");
          MDO<<nav.path[n];
        }
        MDO<<"]";

        for(int c=0;c<10;c++) {
          xy(1,11+c)(*this);
          eraseLine()(*this);
        }

        it.template printTitle<Nav,This,op>(nav,*this,fullPrint);
        it.template printItems
          <Nav,This,op>
          (nav,*this,fullPrint,0,This::top(),self);

        out.template printMenu<It,Nav,op>(it,nav,fullPrint);
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printTitle(It& it,Nav& nav) {
        xy(1,10)(*this);
        eraseLine()(*this);
        it.template print<Nav,This,op>(nav,*this);
        out.template printTitle<It,Nav,op>(it,nav);
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        xy(1,11+n)(*this);
        MDO<<"posY:"<<This::posY()<<" n:"<<n<<" s:"<<s<<" e:"<<e<<" m:"<<m<<" changed:"<<it.changed()<<" "<<(s?(e?">":"-"):" ");
        it.template print<Nav,This,op>(nav,*this);
        out.template printItem<It,Nav,op>(it,nav,n,s,e,m);
      }

      template<bool toPrint=true>
      inline void nl() {
        dev.println();
        // Base::template nl<true>();
      }
      template<typename T,bool toPrint=true>
      inline void raw(T o) {
        dev.print(o);
        // Base::template raw<T,toPrint>(o);
      }
    };
  };
};
