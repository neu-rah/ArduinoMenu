/* -*- C++ -*- */
#pragma once

#include "base.h"
#include "out.h"

/////////////////////////////////////////////////////////////////////
template<typename I=Nil>
struct Empty {
  using This=Empty<I>;

  template<typename Item>
  static inline bool enabled(Item&) {return true;}
  template<typename Item>
  static inline bool enabled(Item& i,idx_t n) {return i.enabled();}

  template<typename Item>
  static inline idx_t selIdx(Item&) {return 0;}
  template<typename Item>
  static inline idx_t selIdx(Item& i,idx_t n) {return i.selIdx();}

  template<typename Item>
  static inline idx_t size(Item&) {return 0;}
  template<typename Item>
  static inline idx_t size(Item& i,idx_t n) {return i.size();}

  template<typename Item,typename Out>
  static inline void print(Item& i,Out& o) {}
  template<typename Item,typename Out>
  static inline void printItem(Item& i,Out& o,idx_t n) {i.print(i,o);}
};

template<const char**text,typename I=Empty<>>
struct StaticText:public I {
  template<typename Item,typename Out>
  static inline void print(Item& i,Out& o) {
    Out::raw(text[0]);
    I::template print<Item,Out>(i,o);
  }
};

template<typename I>
class EnDis: public I {
  public:
    template<typename Item>
    static inline bool enabled(Item& i) {return i.en;}
  protected:
    bool en=true;
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
    template<typename Item,typename Out>
    static inline void print(Item& i,Out& o) {
      This::printItem(i,o,0);
      i.next.print(i.next,o);
    }
    template<typename Item,typename Out>
    static inline void printItem(Item& i,Out& o,idx_t n) {n?i.next.printItem(i,o,n-1):print(i,o);}
  protected:
    Next next;
};

template<typename I>
struct StaticMenu<I>:public I {
  using This=StaticMenu<I>;
  template<typename Item>
  static inline idx_t size(Item&) {return 1;}
  template<typename Item,typename Out>
  static inline void print(Item& i,Out& o) {i.I::template print<Out>(o);}
  template<typename Item,typename Out>
  static inline void printItem(Item& i,Out& o,idx_t n) {i.I::print(o);}
};

template<typename N>
class NavNode {
  public:
    template<typename Item>
    static inline idx_t selIdx(Item& i) {return i.sel;}
  protected:
    idx_t sel;
};

template<typename I>
struct Item:public I {
  inline bool enabled() {return I::enabled(*this);}
  inline bool enabled(idx_t n) {return I::enabled(*this,n);}
  inline idx_t selIdx() {return I::selIdx(*this);}
  inline idx_t selIdx(idx_t n) {return I::selIdx(*this,n);}
  template<typename Out>
  inline void print(Out& o) {I::print(*this,o);}
  template<typename Out>
  inline void print(Out& o,idx_t n) {I::print(*this,o,n);}
};

////////////////////////////////////////////////////////////////////////
struct MenuItem {
  virtual inline bool enabled() const=0;
  virtual inline bool enabled(idx_t) const=0;
  virtual inline idx_t selIdx() const=0;
  virtual inline idx_t selIdx(idx_t n) const=0;
  virtual inline void print(MenuOut& o)=0;
  virtual inline void print(MenuOut& o,idx_t n)=0;
};

template<typename I>
struct Prompt:public MenuItem,public I {
  inline bool enabled() const override {return I::enabled(*this);}
  inline bool enabled(idx_t n) const override {return I::enabled(*this,n);}
  inline idx_t selIdx() const override {return I::selIdx(*this);}
  inline idx_t selIdx(idx_t n) const override {return I::selIdx(*this,n);}
  inline void print(MenuOut& o) override {I::print(*this,o);}
  inline void print(MenuOut& o,idx_t n) override {I::print(*this,o,n);}
};
