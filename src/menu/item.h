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
  constexpr static inline bool enabled() {return true;}
  constexpr static inline bool enabled(idx_t) {return true;}
  static inline void enable(idx_t,bool) {}
  constexpr static inline bool activate() {return false;}
  constexpr static inline bool activate(idx_t) {return false;}
  // inline Item& operator[](idx_t) {return *(Item*)this;}
};

//static ------------------------------------------------------------
template<const char**text,typename O=Empty<>>
struct StaticText:public O {
  using O::O;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.template raw(text[0]);
  }
};

using ActionHandler=bool (*)();
static inline bool doNothing() {return false;}
template<typename O,ActionHandler act=doNothing>
struct Action:public O {
  using This=Action<O,act>;
  using O::O;
  inline bool activate() {return act();}
};

template<typename O,typename... OO>
class StaticMenu:public StaticMenu<O> {
  public:
    using This=StaticMenu<O>;
    using Next=StaticMenu<OO...>;
    constexpr inline idx_t size() {return next.size()+1;}
    template<typename Nav,typename Out>
    inline void printItem(Nav& nav,Out& out,idx_t n) {
      n?next.template printItem<Nav,Out>(nav,out,n-1):O::print(nav,out);
    }
    inline void enable(idx_t n,bool o) {
      if (n) next.enable(n-1,o);
      else O::enable(n,o);
    }
    inline bool enabled(idx_t n) const {
      trace(MDO<<"StaticMenu<O,OO...>::enabled"<<endl);
      return n?next.enabled(n-1):O::enabled(0);
    }
    inline bool activate(idx_t n) {return n?next.activate(n-1):activate();}
    // inline Item& operator[](idx_t n) {
    //   trace(MDO<<"StaticMenu<O,OO...>::operator[] "<<n<<endl);
    //   return n?next.operator[](n-1):*reinterpret_cast<Item*>(this);
    // }
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
  inline bool enabled(idx_t n) const {
    trace(MDO<<"StaticMenu<O>::emabled"<<endl);
    return n?true:O::enabled(0);
  }
  inline void enable(idx_t n,bool o) {
    if(!n) O::enable(n,o);
  }
  inline bool activate() {return O::act();}
  // inline Item& operator[](idx_t n) {
  //   trace(MDO<<"StaticMenu<O>::operator[] "<<n<<endl);
  //   return *reinterpret_cast<Item*>(this);
  // }
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
  inline void enable(idx_t n,bool b) override {O::enable(n,b);}
  virtual inline bool enabled(idx_t n) const override {return O::enabled(n);}
  inline bool activate() override {return O::activate();}
  inline bool activate(idx_t n) override {return O::activate(n);}
};
