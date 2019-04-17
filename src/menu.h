/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  #include <streamFlow.h>
#endif


// template<template<typename,typename...> class T,typename O>
// struct Bind {
//   template<typename... OO>
//   struct To:public T<O,OO...> {};
// };

template<template<typename,typename> class T,typename O>
struct Do {
  template<typename OO>
  struct Bind:public T<O,OO> {};
};

namespace AM5 {

  // enum class Roles {Panel,Menu,Title,Body,Item,Accel,Cursor,Label,Value,Unit};

  // struct Nil {};

  struct Empty {
    template<typename H>
    static inline void out() {}
    template<typename H>
    static inline void out(size_t) {}
    constexpr static inline size_t size() {return 0;}
    constexpr static inline bool up() {return false;}
    constexpr static inline bool down() {return false;}
    constexpr static inline bool left() {return down();}
    constexpr static inline bool right() {return up();}
    constexpr static inline bool enter() {return false;}
    constexpr static inline bool esc() {return false;}
    static inline void enable(bool) {}
    constexpr static inline bool enabled() {return true;}
    template<typename H,bool io> inline void fmtItem(H& p) {}
    template<typename H,bool io> inline void fmtMenu(H& p) {}
    template<typename H,bool io> inline void fmtMenuBody(H& p) {}
    template<typename H,bool io> inline void fmtTitle(H& p) {}
    template<typename H,bool io> inline void fmtIndex(H& p) {}
    template<typename H,bool io> inline void fmtCursor(H& p) {}
  };

  template<typename O,size_t p>
  struct PrintHead {
    using This=PrintHead<O,p>;
    using Printer=O;
    O& printer;
    // size_t pos;
    // size_t line;
    constexpr static inline size_t pos() {return p;}
    inline bool selected() const {return printer.selected(p);}
    inline bool enabled() const {return printer.template enabled<p>();}
    template<typename H,bool io> inline void fmtItem() {
      // Serial<<"fmtItem<"<<p<<">";
      printer.fmtItem<This,io>(*this);}
    template<typename H,bool io> inline void fmtMenu() {printer.fmtMenu<This,io>(*this);}
    template<typename H,bool io> inline void fmtMenuBody() {printer.fmtMenuBody<This,io>(*this);}
    template<typename H,bool io> inline void fmtTitle() {printer.fmtTitle<This,io>(*this);}
    template<typename H,bool io> inline void fmtIndex() {printer.fmtIndex<This,io>(*this);}
    template<typename H,bool io> inline void fmtCursor() {printer.fmtCursor<This,io>(*this);}
  };

  #ifdef ARDUINO
    template<decltype(Serial)& out,typename O=Empty>
    struct SerialOutDef:public O {
      template<typename T>
      static inline void raw(T o) {out<<o;};
      static inline void nl() {out<<endl;}
    };

    using ConstText=const char[];
    // using FlashString=const PROGMEM ConstText;

    template<typename T,T text,typename O=Empty>
    struct StaticFlashTextDef:public O {
      template<typename H>
      static inline void out() {
        O::raw(
          reinterpret_cast<const __FlashStringHelper *>(
            text[0]
          )
        );
      }
    };
  #else
    template<ostream& out,typename O=Empty>
    struct StdOutDef:public O {
      template<typename T>
      static inline void raw(T o) {out<<o;};
      static inline void nl() {out<<endl;}
    };
  #endif

  template<typename O>
  class NavPosDef:public O {
    public:
      inline bool selected(size_t n) const {return at==n;}
      inline bool up() {
        if (at<O::size()-1) {at++;return true;}
        return O::up();
      }
      inline bool down() {
        if (at>0) {at--;return true;}
        return O::down();
      }
      inline size_t pos() const {return at;}
    protected:
      size_t at=0;
  };

  template<typename O>
  class EnDisDef:public O {
    public:
      // EnDisDef(bool o=true):en(o) {}
      inline void enable(bool b=true) {
        // Serial<<"EnDis! "<<b<<endl;
        en=b;}
      inline bool enabled() const {
        // Serial<<"EnDis::enabled? "<<en<<endl;
        return en;}
    protected:
      bool en;
  };

  template<const char** text,typename O=Empty>
  struct StaticTextDef:public O {
    // using O::O;
    template<typename H>
    static inline void out() {
      O::raw(text[0]);
    }
  };

  template<typename O,typename... OO>
  class StaticMenuDataDef:public StaticMenuDataDef<O> {
    public:
      using This=StaticMenuDataDef<O>;
      using Next=StaticMenuDataDef<OO...>;
      constexpr static inline size_t size() {return Next::size()+1;}
      template<typename H,size_t n>
      inline void printItems(H& ph) {
        // Serial<<"print Items "<<n<<" pos:"<<ph.pos()<<endl;
        This::template printItem<H,n>(ph);
        next.printItems<H,n+1>(ph);
      }

      template<size_t i>
      inline void enable(bool b=true) {
        if (i) next.template enable<i-1>(b);
        else O::enable(b);
      }
      template<size_t i>
      inline bool enabled() const {
        if (i) return next.template enabled<i-1>();
        else return O::enabled();
      }
    protected:
      Next next;
  };

  template<typename O>
  struct StaticMenuDataDef<O>:public O {
    constexpr static inline size_t size() {return 1;}
    template<typename H,size_t n>
    inline void printItems(H& ph) {printItem<H,n>(ph);}
    template<typename H,size_t n>
    inline void printItem(H& oph) {
      // Serial<<"print item "<<n<<endl;
      PrintHead<typename H::Printer,n> ph{oph.printer};
      ph.template fmtItem<H,true>();
      ph.template fmtIndex<H,true>();
      ph.template fmtCursor<H,true>();
      O::template out<H>();
      ph.template fmtCursor<H,false>();
      ph.template fmtIndex<H,false>();
      ph.template fmtItem<H,false>();
    }
    template<size_t i>
    inline void enable(bool b=true) {
      if (!i) O::enable(b);
    }
    template<size_t i>
    inline bool enabled() const {
      if (!i) return O::enabled();
      return true;
    }
  };

  template<typename O>
  struct TextFmt:public O {
    template<typename H,bool io>
    inline void fmtCursor(H& p) {
      if (io) {
        O::raw(p.selected()?(p.enabled()?'>':'-'):' ');
        O::template fmtItem<H,io>(p);
      } else {
        O::template fmtItem<H,io>(p);
      }
    }
    template<typename H,bool io>
    inline void fmtIndex(H& p) {
      if (io) {
        O::raw('[');
        O::raw(p.pos());
        O::raw(']');
        O::template fmtItem<H,io>(p);
      } else {
        O::template fmtItem<H,io>(p);
      }
    }
    template<typename H,bool io>
    inline void fmtItem(H& p) {
      if (io) O::template fmtItem<H,io>(p);
      else {
        O::template fmtItem<H,io>(p);
        O::nl();
      }
    }
    template<typename H,bool io>
    inline void fmtTitle(H& p) {
      if (io) {
        O::template fmtTitle<H,io>(p);
      } else {
        O::template fmtTitle<H,io>(p);
        O::nl();
      }
    }
  };

  // the advantage of using dub-part printer is that
  // the user can either ommit (same as ommit the formats)
  // or reorder them, not using sub-printers yet
  template<typename O>
  struct FullPrinterDef:public O {
    inline void printMenu() {
      // Serial<<"full menu printer"<<endl;
      using This=PrintHead<FullPrinterDef<O>,0>;
      This ph{*this};
      O::template fmtMenu<This,true>(ph);
      O::template fmtMenuBody<This,true>(ph);
      O::template fmtTitle<This,true>(ph);
      O::template out<O>();
      O::template fmtTitle<This,false>(ph);
      O::template printItems<This,0>(ph);
      O::template fmtMenuBody<This,false>(ph);
      O::template fmtMenu<This,false>(ph);
    }
  };

  template<typename O>
  struct Cap:public O {
    using This=Cap<O>;
    inline void printMenu() {O::printMenu();}
    static inline void out() {O::template out<O>();}
  };

};
