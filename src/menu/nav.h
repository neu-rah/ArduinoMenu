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

/*
* The Drift class. Is the minimalist navigation element
*/
template<typename N=Nil> struct Drift:public N {
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
  constexpr static inline Modes mode() {return Modes::Normal;}
  template<typename Nav> constexpr static inline Modes _mode(Nav&) {return Modes::Normal;}
  template<typename Nav> constexpr static inline Modes mode(Nav& nav) {return nav._mode();}
  template<typename Nav> constexpr static inline bool _up(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _down(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _left(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _right(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _enter(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _esc(Nav& nav) {return false;}
  // template<typename Nav> constexpr static inline bool up(Nav& nav) {return nav._up(nav);}
  // template<typename Nav> constexpr static inline bool down(Nav& nav) {return nav._down(nav);}
  // template<typename Nav> constexpr static inline bool left(Nav& nav) {return nav._left(nav);}
  // template<typename Nav> constexpr static inline bool right(Nav& nav) {return nav._right(nav);}
  // template<typename Nav> constexpr static inline bool enter(Nav& nav) {return nav._enter(nav);}
  // template<typename Nav> constexpr static inline bool esc(Nav& nav) {return nav._esc(nav);}
};

/*
* The NavBase class. Provides common navigation functionality
* @brief this is a brief
*/
template<typename Out,typename N=Drift<>>
class NavBase:public N {
  public:
    inline void enterMenuRender() {onMenu=true;}
    inline void exitMenuRender() {onMenu=false;}
    inline idx_t top() const {return out.top();}
    inline void setTop(idx_t n) {out.setTop(n);}
    inline idx_t height() const {return out.height();}
    // inline void setFocus(Item*i=NULL) {focus=i;}
    // inline void clearFocus() {setFocus();}
    // inline Item& getFocus() const {return *focus;}
    // inline bool hasFocus() {return focus!=NULL;}
  protected:
    Out out;
    bool onMenu=false;
};

/*
* The StaticNab class. Is bound to a specific menu data
*/
template<typename Out,typename Data,typename N=Drift<>>
class StaticNav:public NavBase<Out,N> {
  public:
    using Base=NavBase<Out,N>;
    using This=StaticNav<Out,Data,N>;
    inline void setTarget(Data d) {data=d;}
    template<typename Nav>
    inline void printMenu(Nav& nav) {
      // Base::enterMenuRender();
      // Base::out.newView();
      Base::out.template printMenu<Nav,Out,Data>(nav,Base::out,data);
      // Base::exitMenuRender();
    }
    inline idx_t size() {return data.size();}
    inline void enable(idx_t n,bool o) {data.enable(n,o);}
    inline bool enabled(idx_t n) {return data.enabled(n);}
    // inline Item& operator[](idx_t n) {return data.operator[](n);}
    inline NavAgent activate() {return data.activate();}
    inline NavAgent activate(idx_t n) {return data.activateItem(n);}
    inline Modes _mode() const {return data.mode();}
  protected:
    Data data;
};

/**
* The NavRoot class encapsulates navigation components and implements/redirects navigation interface
*/
template<typename N>
struct NavRoot:public N {
  using This=NavRoot<N>;
  inline bool up() {return N::template _up<This>(*this);}
  inline bool down() {return N::template _down<This>(*this);}
  inline bool left() {return N::template _left<This>(*this);}
  inline bool right() {return N::template _right<This>(*this);}
  inline bool enter() {
    trace(MDO<<"NavRoot::enter"<<endl);
    return N::template _enter<This>(*this);}
  inline bool esc() {return N::template _esc<This>(*this);}
  inline Modes mode() {
    _trace(MDO<<"NavRoot::mode"<<endl);
    return N::template _mode<This>(*this);}
  // inline NavAgent activate() {return N::activate();}
  inline void printMenu() {
    N::enterMenuRender();
    N::out.newView();
    N::template printMenu<This>(*this);
    N::exitMenuRender();
  }
};

/**
* The DynamicNav class. Can point o other target menu
*/
template<typename Out,typename Data,typename N=Drift<>>
class DynamicNav:public NavNode,public NavBase<Out,N> {
  public:
    using Base=NavBase<Out,N>;
    using This=DynamicNav<Out,Data,N>;
    DynamicNav(Data& o):data(&o) {}
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {This::printMenu(*this);}
    template<typename Nav>
    inline void printMenu(Nav& nav) {
      // Base::enterMenuRender();
      // Base::out.newView();
      Base::out.template printMenu<Nav,Out,Data>(nav,Base::out,*data);
      // Base::exitMenuRender();
    }
    /**
    * @brief get the element size
    * @return idx_t (number of collection members)
    * @details non collection elements will return 0
    */
    inline idx_t size() {return data->size();}
    inline bool selected(idx_t i) const override {return N::selected(i);}
    inline bool enabled(idx_t i) const override {return data->enabled(i);}
    inline void enable(idx_t n,bool o) {data->enable(n,o);}
    inline bool up() override {return N::template _up<This>(*this);}
    inline bool down() override {return N::template _down<This>(*this);}
    inline bool left() override {return N::template _left<This>(*this);}
    inline bool right() override {return N::template _right<This>(*this);}
    inline bool enter() override {return N::template _enter<This>(*this);}
    inline bool esc() override {return N::template _esc<This>(*this);}
    // inline Item& operator[](idx_t n) {return data->operator[](n);}
    inline NavAgent activate() {return data->activate();}
    inline NavAgent activate(idx_t n) {return data->activateItem(n);}
    inline Modes mode() const override {return N::mode();}
  protected:
    Data* data;
};

/**
* The NavPos class holds selected item index, is a primitive navigation functionality
*/
template<typename N=Drift<>>
class NavPos:public N {
  public:
    using N::N;
    using This=NavPos<N>;
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
    inline idx_t pos() {return at;}
    template<typename Nav>
    inline bool _enter(Nav& nav) {
      trace(MDO<<"ItemNav::_enter"<<endl);
      return (!nav.enabled(nav.pos()))&&(nav.activate(at)||N::_enter(nav));
    }
    // template<typename Nav>
    // inline bool _enter(Nav& nav) {
    //   _trace(MDO<<"NavPos::_enter "<<at<<endl);
    //   // if (nav.hasFocus()) nav.clearFocus();
    //   // else if (nav.enabled(at)&&nav.activate()) nav.setFocus(&nav.operator[](at));
    //   // else return false;
    //   // return true;
    //   // return false;
    // }
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
      if (focus) {
        if (focus.enter()) return true;
        focus=Empty<>::activate();//blur if enter return false
      } else {
        if (!nav.enabled(nav.pos())) return false;
        focus=nav.activate(nav.pos());
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
    template<typename Nav>
    inline Modes _mode(Nav& nav) const {
      _trace(MDO<<"ItemNav::_mode"<<endl);
      return focus.result()?focus.mode():Modes::Normal;}
  protected:
    NavAgent focus;
};

/** @}*/
