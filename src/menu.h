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

using idx_t = int;//size_t sucks for scrolling

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
    constexpr static inline size_t top() {return 0;}
    constexpr static inline idx_t orgX() {return 0;}
    constexpr static inline idx_t orgY() {return 0;}
    constexpr static inline idx_t width() {return 80;}
    constexpr static inline idx_t height() {return size();}
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
    constexpr static inline size_t pos() {return p;}
    inline bool selected() const {return printer.selected(p);}
    inline bool enabled() const {return printer.template enabled<p>();}
    template<typename H,bool io> inline void fmtItem() {printer.fmtItem<This,io>(*this);}
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
      inline bool enabled() const {return en;}
      inline void enable(bool b) {en=b;}
    protected:
      bool en=true;
  };

  template<const char** text,typename O=Empty>
  struct StaticTextDef:public O {
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
      template<typename H,size_t n,size_t start,size_t end>
      inline void printItems(H& ph) {
        // Serial<<"print Items "<<n<<" pos:"<<ph.pos()<<endl;
        if (n>=end) return;
        if (n>=start) This::template printItem<H,n>(ph);
        next.printItems<H,n+1,start,end>(ph);
      }

      template<size_t i>
      inline void enable(bool b=true) {
        // Serial<<"StaticMenuDataDef... "<<(b?"enable ":"disable ")<<i<<endl;
        if (i) next.template enable<i-1>(b);
        else O::enable(b);
      }
      template<size_t i>
      inline bool enabled() const {
        // Serial<<"StaticMenuDataDef... enabled "<<i<<endl;
        if (i) return next.template enabled<i-1>();
        else return O::enabled();
      }
    protected:
      static Next next;//being static saves a couple of byte, can be because each item is unique
  };

  template<typename O,typename... OO>
  StaticMenuDataDef<OO...> StaticMenuDataDef<O,OO...>::next;

  template<typename O>
  struct StaticMenuDataDef<O>:public O {
    constexpr static inline size_t size() {return 1;}
    template<typename H,size_t n,size_t start,size_t end>
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
      // Serial<<"StaticMenuDataDef enabled "<<i<<endl;
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
        // O::raw('[');
        if (p.pos()<9) O::raw(p.pos()+1);
        else O::raw(' ');
        O::raw(')');
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
        O::raw('[');
        O::template fmtTitle<H,io>(p);
      } else {
        O::template fmtTitle<H,io>(p);
        O::raw(']');
        O::nl();
      }
    }
  };

  //static panel ------------------------------------------------
  // describes output dimension,
  // may be whole device, but must not exceed
  // it has origin coordinates to be displaced around
  template<idx_t x,idx_t y,idx_t w,idx_t h,typename O=Empty>
  struct StaticPanel:public O {
    constexpr static inline idx_t orgX() {return x;}
    constexpr static inline idx_t orgY() {return y;}
    constexpr static inline idx_t width() {return w;}
    constexpr static inline idx_t height() {return h;}
  };

    //its different than a scroll viewport
    //as it refers to the top line of the menu structure
    //minimize printing on line menus
    template<typename O>
    class RangePanel:public O {
      public:
        // constexpr static inline bool isRangePanel() {return true;}
        inline size_t top() const {return topLine;}
        inline void setTop(size_t n) {topLine=n;}
      protected:
        size_t topLine=0;
    };

  // the advantage of using sub-part printer is that
  // the user can either ommit (same as ommit the formats)
  // or reorder them, not using sub-printers yet...
  // so we call the fmt functions directly here
  // TODO: instead of having multiple printers
  // make this one depend on panel size
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
      O::template printItems<This,O::top(),O::top(),O::top()+O::height()>(ph);//we lost range control here
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
