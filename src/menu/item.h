/* -*- C++ -*- */
#pragma once

#include "base.h"

//////////////////////////////////////////////
template<typename I>
struct Empty:public I {
  template<typename N,typename O,typename H>
  static inline void print(N&,O&,H&) {}
  template<typename N,typename O,typename H>
  static inline void printItem(N&,O&,H&,idx_t) {}
  // constexpr static inline bool isRange() {return false;}
  constexpr static inline idx_t size() {return 0;}
};

template<const char**text,typename I=Empty<>>
struct StaticText:public I {
  template<typename N,typename O,typename H>
  inline void print(N& n,O& o,H& i) {
    o.raw(text[0]);
    I::template print<N,O,H>(n,o,i);
  }
};

template<typename I,typename... II>
class StaticMenu:public StaticMenu<I> {
  public:
    using This=StaticMenu<I>;
    using Next=StaticMenu<II...>;
    friend Next;
    template<typename Item>
    static inline bool enabled(Item& i,idx_t n) {return n?i.next.enabled(i,n-1):enabled(i);}
    template<typename Item>
    static inline idx_t selIdx(Item& i,idx_t n) {return n?i.next.selIdx(i,n-1):selIdx(i);}
    constexpr static inline idx_t size() {return Next::size()+1;}
    // template<typename N,typename O,typename H>
    // inline void print(N& n,O& o,H& i) {
    //   This::printItem(n,o,i,0);
    //   next.print(n,o,next);
    // }
    template<typename N,typename O,typename H>
    inline void printItem(N& n,O& o,H& i,idx_t c) {
      // _trace(MDO<<"StaticMenu<...>::printItem "<<c<<endl);
      c>0?i.next.printItem(n,o,next,c-1):I::print(n,o,i);
    }
  protected:
    Next next;
};

template<typename I>
struct StaticMenu<I>:public I {
  using This=StaticMenu<I>;
  constexpr static inline idx_t size() {return 1;}
  template<typename N,typename O,typename H>
  inline void print(N& n,O& o,H& i) {/*I::print(n,o,i);*/}
  template<typename N,typename O,typename H>
  static inline void printItem(N& n,O& o,H& i,idx_t c) {
    // _trace(MDO<<"StaticMenu::printItem "<<c<<endl);
    i.I::print(n,o,i);
  }
};


template<typename I,typename N=NavPos<>>
struct ItemNav:public I,public N {
  template<typename O>
  inline void printMenu(O& out) {out.print(*this,out,*this);}
};

template<typename I>
struct StaticItem:public I {
  using This=StaticItem<I>;
  template<typename N,typename O,typename H>
  inline void print(N& n,O& o,H& i) {
    I::template print<N,O,This>(n,o,i);
  }
};

struct IItem {
  virtual inline void print(INavNode&,IMenuOut&) {}
};

template<typename I>
struct Item:public IItem, public I {
  inline void print(INavNode& nav,IMenuOut& out) override {I::print(nav,out,*this);}
  template<typename N,typename O,typename H>
  static inline void print(N& nav,O& out,H& i) {I::print(nav,out,i);}
};
