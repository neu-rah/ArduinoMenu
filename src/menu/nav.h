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
  //TODO: eliminate this functions and this class
  template<typename Out> static inline void printMenu(Out& out) {}
  template<typename Out> static inline void newView() {Out::nl();}
  constexpr static inline idx_t pos() {return 0;}
  // constexpr static inline idx_t size() {return 0;}
  // constexpr static inline bool selected(idx_t) {return false;}
  // constexpr static inline bool enabled(idx_t) {return true;}
  // constexpr static inline Modes _mode() {return Modes::Normal;}
  // template<typename Nav> constexpr static inline Modes mode(Nav& nav) {return nav._mode(nav);}
  // template<typename Nav> constexpr static inline bool up   (Nav& nav) {return false;}
  // template<typename Nav> constexpr static inline bool down (Nav& nav) {return false;}
  // template<typename Nav> constexpr static inline bool left (Nav& nav) {return false;}
  // template<typename Nav> constexpr static inline bool right(Nav& nav) {return false;}
  // template<typename Nav> constexpr static inline bool enter(Nav& nav) {return false;}
  // template<typename Nav> constexpr static inline bool esc  (Nav& nav) {return false;}
};

/**
* The NavBase class. Provides common navigation functionality
*/
template <typename N>
class NavBase:public N {
  public:
    // inline void enterMenuRender() {onMenu=true;}
    // inline void exitMenuRender() {onMenu=false;}
  protected:
    // bool onMenu=false;
};

/**
* The StaticNav class is the static navigation top object
* @brief Navigates an inner data structure
*/
template<typename N>
class StaticNav:public N {
  public:
    using This=StaticNav<N>;
    using Base=N;
    template<typename Out>
    inline void printMenu(Out& out) {
      // Base::enterMenuRender();
      out.newView();
      out.printMenu(*this,out,*this);
      // Base::exitMenuRender();
    }
    // inline idx_t size() {return data.size();}
    // inline void enable(idx_t n,bool o) {data.enable(n,o);}
    // inline bool enabled(idx_t n) const {return data.enabled(n);}
    // inline Modes mode() const {return Base::_mode();}
    // inline bool up() {return Base::template up<This>(*this);}
    // inline bool down() {return Base::template down<This>(*this);}
    // inline bool left() {return Base::template left<This>(*this);}
    // inline bool right() {return Base::template right<This>(*this);}
    template<typename Nav>
    static constexpr inline bool enter(Nav& nav) {return true;}//yes we accept navigation focus
    // inline bool enter() {
    //   trace(MDO<<"StaticNav::enter"<<endl);
    //   return Base::template enter<This>(*this);}
    // inline bool esc() {return Base::template esc<This>(*this);}
    // inline NavAgent activate() {
    //   trace(MDO<<"StaticNav::activate."<<endl);
    //   return data->activate();}
    inline NavAgent activate(idx_t n) {
      trace(MDO<<"StaticNav::activate "<<n<<endl);
      return Base::activateItem(n);}
  // protected:
  //   Data data;
};

/**
* NapPos class keeps navigation selectd index track
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
    template<typename Nav>
    inline bool enter(Nav& nav) {
      trace(MDO<<"NavPos::enter"<<endl);
      return nav.enabled(nav.pos())&&N::enter(nav,at);
      // return (!nav.enabled(nav.pos()))&&(nav.activate(at)||N::enter(nav));
    }
  protected:
    Idx at=0;
};

template<typename N=Drift<>>
using NavPos=NavPosBase<N>;

template<typename N=Drift<>>
using TinyNavPos=NavPosBase<N,int8_t>;


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
    inline bool down(Nav& nav) {
      return focus?focus.down():N::down(nav);
    }
    template<typename Nav>
    inline bool up(Nav& nav) {
      return focus?focus.up():N::up(nav);
    }
    template<typename Nav>
    inline bool left(Nav& nav) {
      if (focus) {
        focus.enter();
        focus=Empty<>::activate();
      }
      return N::left(nav);
    }
    template<typename Nav>
    inline bool right(Nav& nav) {
      if (focus) {
        focus.enter();
        focus=Empty<>::activate();
      }
      return N::right(nav);
    }
    template<typename Nav>
    inline bool enter(Nav& nav) {
      trace(MDO<<"ItemNav::_enter"<<endl);
      if (focus) {
        if (focus.enter()) return true;
        focus=Empty<>::activate();//blur if enter return false
        return true;//redraw anyway
      } else {
        if (!nav.enabled(nav.pos())) return false;
        focus=nav.activate(nav.pos());
        N::enter(nav);
        if (focus.result()) return true;
      }
      return false;
    }
    template<typename Nav>
    inline bool esc(Nav& nav) {
      _trace(MDO<<"ItemNav::_esc!!!!"<<endl);
      if (focus) {
        if (focus.esc()) focus=Empty<>::activate();
        return true;
      }
      return N::esc(nav);
    }
    inline Modes mode() const {
      trace(MDO<<"ItemNav::mode"<<endl);
      return focus.mode();
      // return focus?focus.mode():N::mode();
    }
    inline bool hasFocus() const {return focus;}
  protected:
    //we need a special case for sub-menus, no need to use the vtables of focus NavAgent
    NavAgent focus;
};

template<typename N>
class StaticTreeNode:public N {
  public:
    template<typename Out>
    inline void printMenu(Out& out) {
      _trace(MDO<<"StaticNav::printMenu "<<(open?">>":"")<<endl);
      if (open) {
        _trace(MDO<<"StaticNav::printMenu pos:"<<(int)N::pos()<<endl);
        N::printMenu_node(out,N::pos());
      } else N::printMenu(out);
    }
    template<typename Nav>
    inline bool enter(Nav& nav) {
      _trace(MDO<<"StaticNav::enter"<<endl);
      return open?N::enter_node(nav,N::pos()):open=N::enter(nav);
    }
    template<typename Nav>
    inline bool esc(Nav& nav) {
      _trace(MDO<<"StaticNav::esc"<<endl);
      return open=!N::esc(nav);
    }
    template<typename Nav>
    inline bool up(Nav& nav) {
      _trace(MDO<<"StaticNav::up"<<endl);
      return open?N::up_node(nav,N::pos()):open=N::up(nav);
    }
    template<typename Nav>
    inline bool down(Nav& nav) {
      _trace(MDO<<"StaticNav::down"<<endl);
      return open?N::down_node(nav,N::pos()):open=N::down(nav);
    }
  protected:
    bool open=false;
};

/**
* The StaticNavNode class is the top level static navigation object
*/
template<typename N>
struct StaticNavNode:public N {
  inline Modes mode() {return N::mode(*this);}
  inline bool up() {return N::up(*this);}
  inline bool down() {return N::down(*this);}
  inline bool left() {return N::left(*this);}
  inline bool right() {return N::right(*this);}
  inline bool enter() {return N::enter(*this);}
  inline bool esc() {return N::esc(*this);}
};

// template<typename Data,typename N>
// class StaticNavTree:public StaticNav<N> {
// };

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
template<typename N=Nil>
struct NavNode:public INavNode,public N {
  inline void printMenu(IMenuOut& out) override {N::print(out);}
};
/** @}*/
