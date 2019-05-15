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

template<typename O=Nil>
struct Empty:public O {
  constexpr static inline idx_t size() {return 0;}
  template<typename Nav,typename Out>
  static inline void print(Nav&,Out&) {}
  template<typename Nav,typename Out>
  static inline void printItem(Nav& nav,Out& out,idx_t) {}
  constexpr static inline bool enabled(idx_t) {return true;}
  // template<typename Nav,typename Out> static inline void printItems(Out& out) {print(out);}
};

//static ------------------------------------------------------------
template<const char**text,typename O=Empty<>>
struct StaticText:public O {
  using O::O;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.raw(text[0]);
    // O::printItem(nav,out);
  }
};

template<typename O,typename... OO>
class StaticMenu:public StaticMenu<O> {
  public:
    using This=StaticMenu<O>;
    using Next=StaticMenu<OO...>;
    constexpr inline idx_t size() {return next.size()+1;}
    // template<typename Nav,typename Out,idx_t n=0>
    // inline void printItems(Nav& nav,Out& out) {
    //   out.template printItem<Nav,Out,This>(nav,out,*this,n);
    //   next.template printItems<Nav,Out,n+1>(nav,out);
    // }
    template<typename Nav,typename Out>
    inline void printItem(Nav& nav,Out& out,idx_t n) {
      n?next.template printItem<Nav,Out>(nav,out,n-1):O::print(nav,out);
    }
    template<idx_t n>
    inline void enable(bool o) {
      return n?next.template enable<n-1>(o):next.enable(o);
    }
  protected:
    Next next;
};

template<typename O>
struct StaticMenu<O>:public O {
  using This=StaticMenu<O>;
  constexpr static inline idx_t size() {return 1;}
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {}
  template<typename Nav,typename Out>
  inline void printItem(Nav& nav,Out& out,idx_t) {O::print(nav,out);}
  template<idx_t n>
  inline bool enabled() const {
    return n?true:O::enabled();
  }
  template<idx_t n>
  inline void enable(bool o) {
    if(!n) O::enable(o);
  }
};

//dynamic -----------------------------------------------------------
template<typename O>
struct Prompt:public Item,public O {
  using O::O;
  inline void printItem(NavNode& nav,MenuOut& out,idx_t n) override {
    O::template printItem<NavNode,MenuOut>(nav,out,n);
  }
  inline void print(NavNode& nav,MenuOut& out) override {
    O::print(nav,out);
  }
};
