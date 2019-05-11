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

// enum class Roles {Panel,Menu,Title,Body,Item,Index,Cursor,Name,Mode,Value,Unit};

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

  template<typename Nav,typename Out,typename I> static inline void fmtPanelStart(Nav& nav,Out& out,I& i) {Out::template fmtPanel<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtPanelEnd(Nav& nav,Out& out,I& i) {Out::template fmtPanel<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtMenuStart(Nav& nav,Out& out,I& i) {Out::template fmtMenu<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtMenuEnd(Nav& nav,Out& out,I& i) {Out::template fmtMenu<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtTitleStart(Nav& nav,Out& out,I& i) {Out::template fmtTitle<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtTitleEnd(Nav& nav,Out& out,I& i) {Out::template fmtTitle<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtBodyStart(Nav& nav,Out& out,I& i) {Out::template fmtBody<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtBodyEnd(Nav& nav,Out& out,I& i) {Out::template fmtBody<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtItemStart(Nav& nav,Out& out,I& i) {Out::template fmtItem<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtItemEnd(Nav& nav,Out& out,I& i) {Out::template fmtItem<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtIndexStart(Nav& nav,Out& out,I& i) {Out::template fmtIndex<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtIndexEnd(Nav& nav,Out& out,I& i) {Out::template fmtIndex<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtCursorStart(Nav& nav,Out& out,I& i) {Out::template fmtCursor<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtCursorEnd(Nav& nav,Out& out,I& i) {Out::template fmtCursor<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtNameStart(Nav& nav,Out& out,I& i) {Out::template fmtName<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtNameEnd(Nav& nav,Out& out,I& i) {Out::template fmtName<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtModeStart(Nav& nav,Out& out,I& i) {Out::template fmtMode<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtModeEnd(Nav& nav,Out& out,I& i) {Out::template fmtMode<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtValueStart(Nav& nav,Out& out,I& i) {Out::template fmtValue<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtValueEnd(Nav& nav,Out& out,I& i) {Out::template fmtValue<false,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtUnitStart(Nav& nav,Out& out,I& i) {Out::template fmtUnit<true,Nav,Out,I>(nav,out,i);}
  template<typename Nav,typename Out,typename I> static inline void fmtUnitEnd(Nav& nav,Out& out,I& i) {Out::template fmtUnit<false,Nav,Out,I>(nav,out,i);}
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
    out.fmtItemStart(nav,out,i);
    i.printItem(nav,out);
    out.fmtItemEnd(nav,out,i);
  }
};

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
      out.fmtItemStart(nav,out,i);
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i);
    } else {
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i);
      out.fmtItemEnd(nav,out,i);
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
  inline void fmtPanelStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtPanel<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtPanelEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtPanel<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtMenuStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtMenu<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtMenuEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtMenu<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtTitleStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtTitle<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtTitleEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtTitle<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtBodyStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtBody<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtBodyEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtBody<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtItemStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtItem<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtItemEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtItem<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtIndexStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtIndex<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtIndexEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtIndex<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtCursorStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtCursor<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtCursorEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtCursor<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtNameStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtName<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtNameEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtName<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtModeStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtMode<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtModeEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtMode<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtValueStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtValue<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtValueEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtValue<false,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtUnitStart(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtUnit<true,NavNode,MenuOut,Item>(nav,out,item);}
  inline void fmtUnitEnd(NavNode& nav,MenuOut& out,Item& item) override {O::template fmtUnit<false,NavNode,MenuOut,Item>(nav,out,item);}
};
