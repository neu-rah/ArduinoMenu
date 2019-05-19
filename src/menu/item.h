/* -*- C++ -*- */
#pragma once
/**
* @file item.h
* @author Rui Azevedo
* @brief Base menu item implementations
*/

#include "base.h"

/**
* The Empty class is the static base for menu item elements.
* It provides minimalist or inexistent implementations.
*/
template<typename O=Nil>
struct Empty:public O {
  /// collection size, single elements will return 0
  constexpr static inline idx_t size() {return 0;}
  /// print this element to output, some extra info from navigation might be used, such as index
  template<typename Nav,typename Out>
  static inline void print(Nav&,Out&) {}
  /// print an item from the collection
  template<typename Nav,typename Out>
  static inline void printItem(Nav& nav,Out& out,idx_t) {}
  /// is this item enabled?
  constexpr static inline bool enabled() {return true;}
  /// get enabled status of collection indexed item
  constexpr static inline bool enabled(idx_t) {return true;}
  /// set enabled status of indexed collection member
  static inline void enable(idx_t,bool) {}
  /// activate this item (handle enter/select)
  constexpr static inline bool activate() {return false;}
  /// activate collection item (handle enter/select)
  constexpr static inline bool activate(idx_t) {return false;}
  // inline Item& operator[](idx_t) {return *(Item*)this;}
};

/**
* The StaticText class provides code stored constant strings.
*/
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

/**
* The StaticMenu class represents a compile time menu structure.
*/
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

/**
* The StaticMenu specialization class represents the last element of a compile-time
* menu content chain.
*/
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

/**
* The Prompt class represents a generic virtual menu item.
* This class allows Prompt pointers to be stored on a list as they all implement the same interface.
* This will adapts a composition to the virtual interface.
*/
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
