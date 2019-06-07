/* -*- C++ -*- */
#pragma once

#include "base.h"

//////////////////////////////////////////////
template<typename I>
struct Empty:public I {
  template<typename N,typename O,typename H>
  static inline void print(N&,O&,H&) {}
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
    template<typename Item>
    static inline bool enabled(Item& i,idx_t n) {return n?i.next.enabled(i,n-1):enabled(i);}
    template<typename Item>
    static inline idx_t selIdx(Item& i,idx_t n) {return n?i.next.selIdx(i,n-1):selIdx(i);}
    template<typename Item>
    static inline idx_t size(Item& i) {return i.next.size()+1;}
    template<typename Item>
    static inline idx_t size(Item& i,idx_t n) {return n?i.next.size(n-1):size(i);}
    template<typename N,typename O,typename H>
    inline void print(N& n,O& o,H& i) {
      This::printItem(n,o,i,0);
      next.print(n,o,next);
    }
    template<typename N,typename O,typename H>
    static inline void printItem(N& n,O& o,H& i,idx_t c) {
      n?i.next.printItem(n,o,i,c-1):print(n,o,i);
    }
  protected:
    Next next;
};

template<typename I>
struct StaticMenu<I>:public I {
  using This=StaticMenu<I>;
  template<typename Item>
  static inline idx_t size(Item&) {return 1;}
  template<typename N,typename O,typename H>
  static inline void print(N& n,O& o,I& i) {I::print(n,o,i);}
  template<typename N,typename O,typename H>
  static inline void printItem(N& n,O& o,H& i,idx_t) {i.I::print(n,o);}
};


template<typename I,typename N=NavPos<>>
struct ItemNav:public I,public N {
  template<typename O>
  inline void printMenu(O& out) {out.print(*this,out,*this);}
};

template<typename I>
struct StaticItem:public I {
  using This=StaticItem<I>;
  template<typename N,typename O>
  inline void print(N& n,O& o) {
    I::template print<N,O,This>(n,o,*this);
  }
};

struct IItem {
  virtual inline void print(INavNode&,IMenuOut&) {}
};

template<typename I>
struct Item:public IItem, public I {
  inline void print(INavNode& nav,IMenuOut& out) override {I::print(nav,out,*this);}
};
