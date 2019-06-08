/* -*- C++ -*- */
#pragma once

#include "base.h"

////////////////////////////////////
template<typename N>
struct Drift:public N {
  template<typename Out> static inline void printMenu(Out& out) {}
  template<typename Out> static inline void newView() {Out::nl();}
  constexpr static inline idx_t pos() {return 0;}
  constexpr static inline idx_t size() {return 0;}
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
  constexpr static inline Modes mode() {return Modes::Normal;}
  template<typename Nav> constexpr static inline bool _up   (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _down (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _left (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _right(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _enter(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _esc  (Nav& nav) {return false;}
};

template<typename Data,typename N>
class StaticNav:public N {
  public:
    using This=StaticNav<Data,N>;
    template<typename Out>
    inline void printMenu(Out& out) {out.printMenu(*this,out,data);}
    inline idx_t size() {return data.size();}
    inline void enable(idx_t n,bool o) {data.enable(n,o);}
    inline bool enabled(idx_t n) const {return data.enabled(n);}
    inline bool up() {return N::template _up<This>(*this);}
    inline bool down() {return N::template _down<This>(*this);}
    inline bool left() {return N::template _left<This>(*this);}
    inline bool right() {return N::template _right<This>(*this);}
    inline bool enter() {return N::template _enter<This>(*this);}
    inline bool esc() {return N::template _esc<This>(*this);}
  protected:
    Data data;
};

template<typename N>
class NavPos:public N {
  public:
    inline idx_t pos() {return at;}
    inline bool selected(idx_t idx) const {return at==idx;}
    template<typename Nav>
    inline bool _up(Nav& nav) {
      if (at<nav.size()-1) {at++;return true;}
      return N::template _up<Nav>(nav);
    }
    template<typename Nav>
    inline bool _down(Nav& nav) {
      if (at>0) {at--;return true;}
      return N::template _down<Nav>(nav);
    }
  protected:
    idx_t at;
};

template<typename Data,typename N>
class StaticNavTree:public StaticNav<N> {
};

struct INavNode {
  virtual inline void printMenu(IMenuOut& out)=0;
};

template<typename N=Nil>
struct NavNode:public INavNode,public N {
  inline void printMenu(IMenuOut& out) override {N::print(out);}
};
