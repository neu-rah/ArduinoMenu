/* -*- C++ -*- */
#pragma once
/**
* @file nav.h
* @author Rui Azevedo
* @brief ArduinoMenu navigation implementations
*/

/** \defgroup Navigation Navigation system
 *  @{
 */
#include "base.h"

//Output Device Operation
enum class OutOps {RawOut,Measure};
template<OutOps>
struct OutOp {};

using RawOutOp=OutOp<OutOps::RawOut>;
using MeasureOp=OutOp<OutOps::Measure>;

////////////////////////////////////
/*
* The Drift class is the terminal navigation element
*/
template<typename N>
struct Drift:public N {
  template<typename Out> static inline void printMenu(Out& out) {}
  template<typename Out> static inline void newView() {Out::nl();}
  constexpr static inline idx_t pos() {return 0;}
  constexpr static inline idx_t size() {return 0;}
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
  constexpr static inline Modes _mode() {return Modes::Normal;}
  // template<typename Nav> constexpr static inline Modes mode(Nav& nav) {return nav._mode(nav);}
  template<typename Nav> constexpr static inline bool _up   (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _down (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _left (Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _right(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _enter(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _esc  (Nav& nav) {return false;}
};

/**
* The NavBase class. Provides common navigation functionality
* @brief Navigates an inner data structure
*/
template<typename Data,typename N>
class StaticNav:public N {
  public:
    using This=StaticNav<Data,N>;
    template<typename Out>
    inline void printMenu(Out& out) {
      enterMenuRender();
      out.newView();
      out.printMenu(*this,out,data);
      exitMenuRender();
    }
    inline idx_t size() {return data.size();}
    inline void enable(idx_t n,bool o) {data.enable(n,o);}
    inline bool enabled(idx_t n) const {return data.enabled(n);}
    inline Modes mode() const {return N::_mode();}
    inline bool up() {return N::template _up<This>(*this);}
    inline bool down() {return N::template _down<This>(*this);}
    inline bool left() {return N::template _left<This>(*this);}
    inline bool right() {return N::template _right<This>(*this);}
    inline bool enter() {
      trace(MDO<<"StaticNav::enter"<<endl);
      return N::template _enter<This>(*this);}
    inline bool esc() {return N::template _esc<This>(*this);}
    inline NavAgent activate() {
      trace(MDO<<"StaticNav::activate."<<endl);
      return data->activate();}
    inline NavAgent activate(idx_t n) {
      trace(MDO<<"StaticNav::activate "<<n<<endl);
      return data.activateItem(n);}
    inline void enterMenuRender() {onMenu=true;}
    inline void exitMenuRender() {onMenu=false;}
  protected:
    bool onMenu=false;
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
    template<typename Nav>
    inline bool _enter(Nav& nav) {
      trace(MDO<<"NavPos::_enter"<<endl);
      return (!nav.enabled(nav.pos()))&&(nav.activate(at)||N::_enter(nav));
    }
  protected:
    idx_t at;
};

/**
* The ItemNav class allow items to handle navigation (needed for fields)
* items can handle up|down|enter|esc
* left|right are a thing of the navigation system that can steal
* focus from the field, an enter is sent to the field instead, to validate the entry
*/
template<typename N>
class ItemNav:public N {
  public:
    using N::N;
    // using OutType=typename N::OutType;
    // using DataType=typename N::DataType;
    template<typename Nav>
    inline bool _down(Nav& nav) {
      return focus?focus.down():N::_down(nav);
    }
    template<typename Nav>
    inline bool _up(Nav& nav) {
      return focus?focus.up():N::_up(nav);
    }
    template<typename Nav>
    inline bool _left(Nav& nav) {
      if (focus) {
        focus.enter();
        focus=Empty<>::activate();
      }
      return N::_left(nav);
    }
    template<typename Nav>
    inline bool _right(Nav& nav) {
      if (focus) {
        focus.enter();
        focus=Empty<>::activate();
      }
      return N::_right(nav);
    }
    template<typename Nav>
    inline bool _enter(Nav& nav) {
      trace(MDO<<"ItemNav::_enter"<<endl);
      if (focus) {
        if (focus.enter()) return true;
        focus=Empty<>::activate();//blur if enter return false
      } else {
        if (!nav.enabled(nav.pos())) return false;
        focus=nav.activate(nav.pos());
        N::_enter(nav);
        if (focus.result()) return true;
      }
      return false;
    }
    template<typename Nav>
    inline bool _esc(Nav& nav) {
      if (focus) {
        if (focus.esc()) focus=Empty<>::activate();
        return true;
      }
      return N::_esc(nav);
    }
    inline Modes _mode() const {
      trace(MDO<<"ItemNav::_mode"<<endl);
      return focus?focus.mode():N::_mode();}
    inline bool hasFocus() const {return focus;}
  protected:
    NavAgent focus;
};

// template<typename N>
// struct StaticNavNode:public N {
//   inline Modes mode() {return N::mode(*this);}
// };

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
/** @}*/
