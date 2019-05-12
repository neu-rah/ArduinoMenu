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
  template<typename T> static inline void raw(T) {}
  template<typename Nav,typename Out,typename I>
  static inline void printMenu(Nav&,Out& out,I& i) {}
  template<typename Nav,typename Out,typename I>
  static inline void printItem(Nav&,Out& out,I& i) {i.printItem(out);}

  template<bool io,typename Nav,typename Out,typename I> static inline void fmtPanel(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMenu(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtTitle(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtBody(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtItem(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtIndex(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtCursor(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtName(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMode(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtValue(Nav&,Out&,I&) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtUnit(Nav&,Out&,I&) {}

  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,Nav& nav,Out& out,I& i) {
    fmt(role,true,nav,out,i);
    fmt(role,false,nav,out,i);
  }
  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,bool io,Nav& nav,Out& out,I& i) {
    //we could do better with templates, but we need this to be compatible with virtual interface too
    switch(role) {
      case Roles::Panel: io?out.template fmtPanel<true>(nav,out,i):out.template fmtPanel<false>(nav,out,i);break;
      case Roles::Menu: io?out.template fmtMenu<true>(nav,out,i):out.template fmtMenu<false>(nav,out,i);break;
      case Roles::Title: io?out.template fmtTitle<true>(nav,out,i):out.template fmtTitle<false>(nav,out,i);break;
      case Roles::Body: io?out.template fmtBody<true>(nav,out,i):out.template fmtBody<false>(nav,out,i);break;
      case Roles::Item: io?out.template fmtItem<true>(nav,out,i):out.template fmtItem<false>(nav,out,i);break;
      case Roles::Index: io?out.template fmtIndex<true>(nav,out,i):out.template fmtIndex<false>(nav,out,i);break;
      case Roles::Cursor: io?out.template fmtCursor<true>(nav,out,i):out.template fmtCursor<false>(nav,out,i);break;
      case Roles::Name: io?out.template fmtName<true>(nav,out,i):out.template fmtName<false>(nav,out,i);break;
      case Roles::Mode: io?out.template fmtMode<true>(nav,out,i):out.template fmtMode<false>(nav,out,i);break;
      case Roles::Value: io?out.template fmtValue<true>(nav,out,i):out.template fmtValue<false>(nav,out,i);break;
      case Roles::Unit: io?out.template fmtUnit<true>(nav,out,i):out.template fmtUnit<false>(nav,out,i);break;
    }
  }
};

//static output -----------------------------
template<typename O=Void<>>
struct FullPrinter:public O {
  template<typename Nav,typename Out,typename I>
  static inline void printMenu(Nav& nav,Out& out,I& i) {
    out.template fmtPanel<true>(nav,out,i);
    out.template fmtMenu<true>(nav,out,i);
    out.template fmtTitle<true>(nav,out,i);
    i.print(nav,out);
    out.template fmtTitle<false>(nav,out,i);
    out.template fmtBody<true>(nav,out,i);
    i.template printItems<Nav,Out>(nav,out);
    out.template fmtBody<false>(nav,out,i);
    out.template fmtMenu<false>(nav,out,i);
    out.template fmtPanel<false>(nav,out,i);
  }
  template<typename Nav,typename Out,typename I>
  static inline void printItem(Nav& nav,Out& out,I& i) {
    MDO<<"printItem"<<endl;
    out.template fmtItem<true>(nav,out,i);
    out.template fmtIndex<true>(nav,out,i);
    out.template fmtIndex<false>(nav,out,i);
    i.printItem(nav,out);
    out.template fmtItem<false>(nav,out,i);
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
  static inline void fmtTitle(Nav& nav,Out& out,I& i) {
    if (io) {
      out.raw(open);
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i);
    } else {
      O::template fmtTitle<io>(nav,out,i);
      out.raw(close);
    }
  }
};

template<typename O>
struct TextFmt:public O {
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtTitle(Nav& nav,Out& out,I& i) {
    if (io) {
      out.fmt(Roles::Item,true,nav,out,i);
      //this gets called with the virtual class, cant use the templates
      //out.template fmtItem<true>(nav,out,i);
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i);
    } else {
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i);
      out.fmt(Roles::Item,false,nav,out,i);
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtItem(Nav& nav,Out& out,I& i) {
    if(io) O::template fmtItem<io,Nav,Out,I>(nav,out,i);
    else {
      O::template fmtItem<io,Nav,Out,I>(nav,out,i);
      out.nl();
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtIndex(Nav& nav,Out& out,I& i) {
    MDO<<"{Index}";
  }
};

template<typename Dev,Dev dev,typename O=FullPrinter<>>
struct RawOut:public O {
  static inline void nl() {dev<<endl;}
  template<typename T> static inline void raw(T o) {dev<<o;}
};

//dynamic output --------------------------------
template<typename O>
struct MenuOutDef:public O,public MenuOut {
  inline void nl() override {O::nl();}
  inline void raw(char c) override {O::raw(c);}
  inline void raw(const char*text) override {O::raw(text);}
  inline void printItem(NavNode& nav,Item& i) override {i.printItem(nav,*this);}
  inline void fmt(Roles role,bool io,NavNode& nav,MenuOut& out,Item& i) override {
    switch(role) {
      case Roles::Panel: io?O::template fmtPanel<true>(nav,out,i):O::template fmtPanel<false>(nav,out,i);break;
      case Roles::Menu: io?O::template fmtMenu<true>(nav,out,i):O::template fmtMenu<false>(nav,out,i);break;
      case Roles::Title: io?O::template fmtTitle<true>(nav,out,i):O::template fmtTitle<false>(nav,out,i);break;
      case Roles::Body: io?O::template fmtBody<true>(nav,out,i):O::template fmtBody<false>(nav,out,i);break;
      case Roles::Item: io?O::template fmtItem<true>(nav,out,i):O::template fmtItem<false>(nav,out,i);break;
      case Roles::Index: io?O::template fmtIndex<true>(nav,out,i):O::template fmtIndex<false>(nav,out,i);break;
      case Roles::Cursor: io?O::template fmtCursor<true>(nav,out,i):O::template fmtCursor<false>(nav,out,i);break;
      case Roles::Name: io?O::template fmtName<true>(nav,out,i):O::template fmtName<false>(nav,out,i);break;
      case Roles::Mode: io?O::template fmtMode<true>(nav,out,i):O::template fmtMode<false>(nav,out,i);break;
      case Roles::Value: io?O::template fmtValue<true>(nav,out,i):O::template fmtValue<false>(nav,out,i);break;
      case Roles::Unit: io?O::template fmtUnit<true>(nav,out,i):O::template fmtUnit<false>(nav,out,i);break;
    }
  }
};
