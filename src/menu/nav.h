/* -*- C++ -*- */
#pragma once
/**
* @file nav.h
* @author Rui Azevedo
* @brief ArduinoMenu navigation implementations
*/
//////////////////////////////////////////////////////////////////
//Output Device Operation
enum class OutOps {RawOut,Measure};
template<OutOps>
struct OutOp {};

using RawOutOp=OutOp<OutOps::RawOut>;
using MeasureOp=OutOp<OutOps::Measure>;

template<typename N>
struct Drift:N {
  using This=StaticNav<N>;
  using Base=N;
  template<typename Out> static inline void newView() {Out::nl();}

  constexpr static inline idx_t size() {return 0;}
  inline void enable(idx_t n,bool o) {}
  inline bool enabled(idx_t n) const {return true;}
  inline bool up() {return Base::template up<This>(*this);}
  inline bool down() {return Base::template down<This>(*this);}
  inline bool left() {return Base::template left<This>(*this);}
  inline bool right() {return Base::template right<This>(*this);}
  inline bool enter() {return Base::template enter<This>(*this);}
  inline bool esc() {return Base::template esc<This>(*this);}
  template<typename Nav> constexpr static inline Modes mode(Nav& nav) {return nav._mode(nav);}
  template<typename Nav> constexpr static inline bool up   (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool down (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool left (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool right(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool enter(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool esc  (Nav& nav) {return false;}

  inline void print(INavNode& nav,IMenuOut& out,Item& i) {i.print(nav,out);}
  // template<typename N,typename O,typename H>
  // inline void print(N& nav,O& out,H& i) {I::print(nav,out,i);}
};

// template<typename N>
// class StaticNav:public N {
//   template<typename Out>
//   inline void printMenu(Out& out) {
//     // Base::enterMenuRender();
//     out.newView();
//     out.printMenu(*this,out,*this);
//     // Base::exitMenuRender();
//   }
// };

/**
* NavPos class keeps navigation selectd index track
* and handles navigation functions for the navigation system.
* this is a navigation component
*/
template<typename N,typename Idx>
class NavPosBase:public N {
  public:
    inline Idx pos() {return at;}
    inline bool selected(idx_t idx) const {return at==idx;}
    template<typename Nav>
    inline bool up(Nav& nav) {
      if (at<nav.size()-1) {at++;return true;}
      return N::template up<Nav>(nav);
    }
    template<typename Nav>
    inline bool down(Nav& nav) {
      if (at>0) {at--;return true;}
      return N::template down<Nav>(nav);
    }
    // template<typename Nav>
    // inline bool enter(Nav& nav) {
    //   trace(MDO<<"NavPos::enter"<<endl);
    //   return nav.enabled(nav.pos())&&N::enter(nav,at);
    //   // return (!nav.enabled(nav.pos()))&&(nav.activate(at)||N::enter(nav));
    // }
  protected:
    Idx at=0;
};

template<typename N=Drift<>>
using NavPos=NavPosBase<N>;

template<typename N=Drift<>>
using TinyNavPos=NavPosBase<N,int8_t>;

/**
* The StaticNavNode class is the top level static navigation object
*/
template<typename N,typename Def>
struct StaticNav:public N {
  template<typename Out>
  inline void printMenu(Out& out) {
    std::cout << "print menu!" << '\n';
    out.newView();
    out.printMenu(*this,out);
  }
  inline Modes mode() {return N::mode(*this);}
  inline bool up() {return N::up(*this);}
  inline bool down() {return N::down(*this);}
  inline bool left() {return N::left(*this);}
  inline bool right() {return N::right(*this);}
  inline bool enter() {return N::enter(*this);}
  inline bool esc() {return N::esc(*this);}
};

/**
* The INavNode class is the navigation interface definition
*/
struct INavNode {
  virtual inline void printMenu(IMenuOut& out)=0;
};

/**
* The NavNode class is the final virtual navigation interface
* it encapsulates the navigation static composition and abides to the virtual interface
*/
template<typename N=Nil<>>
struct NavNode:public INavNode,public N {
  inline void printMenu(IMenuOut& out) override {N::printMenu(out);}
};
/** @}*/
