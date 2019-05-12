/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu output implementations
*/

#include "base.h"

template<typename O=Nil> struct Void:public O {
  static inline void nl() {}
  // template<typename T> static inline void raw(T) {}
  // template<typename Nav,typename Out,typename I>
  // static inline void printMenu(Nav&,Out& out,I& i) {}
  // template<typename Nav,typename Out,typename I>
  // static inline void printItem(Nav&,Out& out,I& i) {i.printItem(out);}

  template<bool io,typename Nav,typename Out,typename I> static inline void fmtPanel(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMenu(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtTitle(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtBody(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtItem(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtIndex(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtCursor(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtName(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMode(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtValue(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtUnit(Nav&,Out&,I&,idx_t) {}

  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,Nav& nav,Out& out,I& i,idx_t n) {
    fmt(role,true,nav,out,i,n);
    fmt(role,false,nav,out,i,n);
  }
  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,bool io,Nav& nav,Out& out,I& i,idx_t n) {
    //we could do better with templates, but we need this to be compatible with virtual interface too
    switch(role) {
      case Roles::Panel: io?out.template fmtPanel<true>(nav,out,i,n):out.template fmtPanel<false>(nav,out,i,n);break;
      case Roles::Menu: io?out.template fmtMenu<true>(nav,out,i,n):out.template fmtMenu<false>(nav,out,i,n);break;
      case Roles::Title: io?out.template fmtTitle<true>(nav,out,i,n):out.template fmtTitle<false>(nav,out,i,n);break;
      case Roles::Body: io?out.template fmtBody<true>(nav,out,i,n):out.template fmtBody<false>(nav,out,i,n);break;
      case Roles::Item: io?out.template fmtItem<true>(nav,out,i,n):out.template fmtItem<false>(nav,out,i,n);break;
      case Roles::Index: io?out.template fmtIndex<true>(nav,out,i,n):out.template fmtIndex<false>(nav,out,i,n);break;
      case Roles::Cursor: io?out.template fmtCursor<true>(nav,out,i,n):out.template fmtCursor<false>(nav,out,i,n);break;
      case Roles::Name: io?out.template fmtName<true>(nav,out,i,n):out.template fmtName<false>(nav,out,i,n);break;
      case Roles::Mode: io?out.template fmtMode<true>(nav,out,i,n):out.template fmtMode<false>(nav,out,i,n);break;
      case Roles::Value: io?out.template fmtValue<true>(nav,out,i,n):out.template fmtValue<false>(nav,out,i,n);break;
      case Roles::Unit: io?out.template fmtUnit<true>(nav,out,i,n):out.template fmtUnit<false>(nav,out,i,n);break;
    }
  }
};

//static output -----------------------------
template<typename O=Void<>>
struct FullPrinter:public O {
  template<typename Nav,typename Out,typename I>
  static inline void printMenu(Nav& nav,Out& out,I& i) {
    out.template fmtPanel<true>(nav,out,i,0);
    out.template fmtMenu<true>(nav,out,i,0);
    out.template fmtTitle<true>(nav,out,i,0);
    i.print(nav,out);
    out.template fmtTitle<false>(nav,out,i,0);
    out.template fmtBody<true>(nav,out,i,0);
    i.template printItems<Nav,Out>(nav,out);
    out.template fmtBody<false>(nav,out,i,0);
    out.template fmtMenu<false>(nav,out,i,0);
    out.template fmtPanel<false>(nav,out,i,0);
  }
  template<typename Nav,typename Out,typename I>
  static inline void printItem(Nav& nav,Out& out,I& i,idx_t n) {
    out.template fmtItem<true>(nav,out,i,n);
    out.template fmtIndex<true>(nav,out,i,n);
    out.template fmtIndex<false>(nav,out,i,n);
    out.template fmtCursor<true>(nav,out,i,n);
    out.template fmtCursor<false>(nav,out,i,n);
    i.printItem(nav,out,n);
    out.template fmtItem<false>(nav,out,i,n);
  }
};

// template<typename O>
// struct BodyPrinter:public O {
//   template<typename Nav,typename Out,typename I>
//   static inline void printMenu(Nav& nav,Out& out,I& i) {
//     size_t sz=i.size();
//     for(size_t n=0;n<sz;n++) {
//       i.printItem(nav,out,n);
//     }
//   }
// };

template<typename O=Void<>,char open='[',char close=']'>
struct TitleWrap:public O {
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtTitle(Nav& nav,Out& out,I& i,idx_t n) {
    if (io) {
      out.raw(open);
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i,n);
    } else {
      O::template fmtTitle<io>(nav,out,i,n);
      out.raw(close);
    }
  }
};

template<typename O>
struct TextFmt:public O {
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtTitle(Nav& nav,Out& out,I& i,idx_t n) {
    if (io) {
      out.fmt(Roles::Item,true,nav,out,i,n);
      //this gets called with the virtual class, cant use the templates
      //out.template fmtItem<true>(nav,out,i);
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i,n);
    } else {
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i,n);
      out.fmt(Roles::Item,false,nav,out,i,n);
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtItem(Nav& nav,Out& out,I& i,idx_t n) {
    if(io) O::template fmtItem<io,Nav,Out,I>(nav,out,i,n);
    else {
      O::template fmtItem<io,Nav,Out,I>(nav,out,i,n);
      out.nl();
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtIndex(Nav& nav,Out& out,I& i,idx_t n) {
    if(io) {
      O::template fmtIndex<io,Nav,Out,I>(nav,out,i,n);
      if (n<9) out.raw(n+1);
      else out.raw(' ');
      out.raw(')');
    } else {
      O::template fmtItem<io,Nav,Out,I>(nav,out,i,n);
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  inline void fmtCursor(Nav& nav,Out& out,I& i,idx_t n) {
    if (io) {
      out.raw(nav.selected(n)?(nav.enabled(n)?'>':'-'):' ');
      O::template fmtCursor<io,Nav,Out,I>(nav,out,i,n);
    } else {
      O::template fmtCursor<io,Nav,Out,I>(nav,out,i,n);
    }
  }
};

template<typename Dev,Dev dev,typename O=FullPrinter<>>
struct RawOut:public O {
  template<typename T> static inline void raw(T o) {dev<<o;}
};

//dynamic output --------------------------------
template<typename O>
struct MenuOutDef:public O,public MenuOut {
  inline void nl() override {O::nl();}
  inline void raw(char c) override {O::raw(c);}
  inline void raw(const char*text) override {O::raw(text);}
  inline void raw(int n) override {O::raw(n);}
  virtual inline void raw(unsigned int n) {O::raw(n);}
  virtual inline void raw(long n) {O::raw(n);}
  virtual inline void raw(unsigned long n) {O::raw(n);}
  virtual inline void raw(double n) {O::raw(n);}
  inline void printItem(NavNode& nav,Item& i,idx_t n) override {i.printItem(nav,*this,n);}
  inline void fmt(Roles role,bool io,NavNode& nav,MenuOut& out,Item& i,idx_t n) override {
    switch(role) {
      case Roles::Panel: io?O::template fmtPanel<true>(nav,out,i,n):O::template fmtPanel<false>(nav,out,i,n);break;
      case Roles::Menu: io?O::template fmtMenu<true>(nav,out,i,n):O::template fmtMenu<false>(nav,out,i,n);break;
      case Roles::Title: io?O::template fmtTitle<true>(nav,out,i,n):O::template fmtTitle<false>(nav,out,i,n);break;
      case Roles::Body: io?O::template fmtBody<true>(nav,out,i,n):O::template fmtBody<false>(nav,out,i,n);break;
      case Roles::Item: io?O::template fmtItem<true>(nav,out,i,n):O::template fmtItem<false>(nav,out,i,n);break;
      case Roles::Index: io?O::template fmtIndex<true>(nav,out,i,n):O::template fmtIndex<false>(nav,out,i,n);break;
      case Roles::Cursor: io?O::template fmtCursor<true>(nav,out,i,n):O::template fmtCursor<false>(nav,out,i,n);break;
      case Roles::Name: io?O::template fmtName<true>(nav,out,i,n):O::template fmtName<false>(nav,out,i,n);break;
      case Roles::Mode: io?O::template fmtMode<true>(nav,out,i,n):O::template fmtMode<false>(nav,out,i,n);break;
      case Roles::Value: io?O::template fmtValue<true>(nav,out,i,n):O::template fmtValue<false>(nav,out,i,n);break;
      case Roles::Unit: io?O::template fmtUnit<true>(nav,out,i,n):O::template fmtUnit<false>(nav,out,i,n);break;
    }
  }
};
