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
    template<bool io> inline void fmtItem() {}
    template<bool io> inline void fmtMenu() {}
    template<bool io> inline void fmtMenuBody() {}
    template<bool io> inline void fmtTitle() {}
    template<bool io> inline void fmtIndex() {}
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

  template<const char** text,typename O=Empty>
  struct StaticTextDef:public O {
    template<typename H>
    static inline void out() {
      O::raw(text[0]);
    }
  };

  template<typename O,typename... OO>
  struct StaticMenuDataDef:public O {
    using Next=StaticMenuDataDef<OO...>;
    constexpr static inline size_t size() {return Next::size()+1;}
    template<typename H>
    static inline void printItem(size_t n) {
      if(n) Next::template printItem<H>(n-1);
      else O::template out<H>();
    }
  };

  template<typename O>
  struct StaticMenuDataDef<O>:public O {
    constexpr static inline size_t size() {return 1;}
    template<typename H>
    static inline void printItem(size_t n) {
      if (!n) O::template out<H>();
    }
  };

  template<typename O>
  struct TextFmt:public O {
    template<bool io>
    inline void fmtIndex() {
      if (io) {
        O::raw('[');
        O::raw(O::pos());
        O::raw(']');
        O::template fmtItem<io>();
      } else {
        O::template fmtItem<io>();
      }
    }
    template<bool io>
    inline void fmtItem() {
      if (io) O::template fmtItem<io>();
      else {
        O::template fmtItem<io>();
        O::nl();
      }
    }
    template<bool io>
    inline void fmtTitle() {
      if (io) {
        O::template fmtTitle<io>();
      } else {
        O::template fmtTitle<io>();
        O::nl();
      }
    }
  };

  // the advantage of using dub-part printer is that
  // the user can either ommit (same as ommit the formats)
  // or reorder them, not using sub-printers yet
  template<typename O>
  struct FullPrinterDef:public O {
    template<typename PH>
    inline void printMenu(PH& ph) {
      // cout<<"full menu printer"<<endl;
      ph.template fmtMenu<true>();
      ph.template fmtMenuBody<true>();
      O::template out<O>();
      ph.template fmtTitle<true>();
      ph.template fmtTitle<false>();
      for(size_t n=0;n<O::size();n++) {
        ph.template fmtItem<true>();
        ph.template fmtIndex<true>();
        ph.template fmtIndex<false>();
        O::template printItem<O>(n);
        ph.template fmtItem<false>();
      }
      ph.template fmtMenuBody<false>();
      ph.template fmtMenu<false>();
    }
  };

  template<typename O>
  struct Cap:public O {
    using This=Cap<O>;
    inline void printMenu() {O::template printMenu<This>(*this);}
    static inline void out() {O::template out<O>();}
    // template<bool io> static inline void fmtIndex() {O::template fmtIndex<io>();}
    // template<> static inline void fmtIndex<true>() {
    //   O::fmtIndex<true>();
    //   O::fmtIndex<false>();
    // }
  };

};
