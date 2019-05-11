/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu item implementations
*/

#include "base.h"

template<typename O=Nil> struct Empty:public O {
  template<typename Nav,typename Out> static inline void print(Nav&,Out&) {}
  template<typename Nav,typename Out> static inline void printItem(Nav&,Out&) {}
  // template<typename Nav,typename Out> static inline void printItems(Out& out) {print(out);}
};

//static ------------------------------------------------------------
template<const char**text,typename O=Empty<>>
struct StaticText:public O {
  using O::O;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.raw(text[0]);
    O::printItem(nav,out);
  }
};

template<typename O,typename... OO>
class StaticMenu:public StaticMenu<O> {
  public:
    using This=StaticMenu<O>;
    using Next=StaticMenu<OO...>;
    template<typename Nav,typename Out>
    inline void printItems(Nav& nav,Out& out) {
      out.template printItem<Nav,Out,This>(nav,out,*this);
      next.template printItems<Nav,Out>(nav,out);
    }
  protected:
    Next next;
};

template<typename O>
struct StaticMenu<O>:public O {
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {}
  template<typename Nav,typename Out>
  inline void printItem(Nav& nav,Out& out) {O::print(nav,out);}
  template<typename Nav,typename Out>
  inline void printItems(Nav& nav,Out& out) {O::print(nav,out);}
};

//dynamic -----------------------------------------------------------
template<typename O>
struct Prompt:public Item,public O {
  using O::O;
  inline void printItem(NavNode& nav,MenuOut& out) override {
    O::print(nav,out);
  }
  inline void print(NavNode& nav,MenuOut& out) override {
    O::print(nav,out);
  }
};

template<typename O=Empty<>>
struct VectorMenu:public O,vector<Item*> {
  using vector<Item*>::vector;
  template<typename Nav,typename Out>
  static inline void print(Nav& bav,Out& out) {}
  template<typename... OO>
  inline VectorMenu(OO... oo):vector<Item*>{oo...} {}
  template<typename Nav,typename Out>
  inline void printItem(Nav& nav,Out& out) {O::print(nav,out);}
  template<typename Nav,typename Out>
  inline void printItems(Nav& nav,Out& out) {
    for(auto i: *this) {
      out.fmtItemStart(nav,out,*i);
      out.printItem(nav,*i);
      out.fmtItemEnd(nav,out,*i);
    }
  }
};
