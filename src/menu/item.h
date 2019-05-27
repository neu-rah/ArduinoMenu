/* -*- C++ -*- */
#pragma once
/**
* @file item.h
* @author Rui Azevedo
* @brief Base menu item implementations
*/

/** \defgroup Items Menu items
 *  @{
 */

#include "base.h"

/********************************************************
* navigation commands -----------------------------------
* menu items that wish to receive navigation commands should emit
* the respective command pallette object
* an empty pallette is emited for items that do not receive
* commands...
* navigations requests `activate` from the item to receive them
********************************************************/

/** \defgroup Agents Command and navigation agents
 *  @{
 */

/**
* The CmdAgent class represents an item that might receive navigation commands
*/
struct CmdAgent {
  virtual bool canNav() const =0;
  virtual bool up(void* o)=0;
  virtual bool down(void* o)=0;
  virtual bool enter(void* o)=0;
  virtual bool esc(void* o)=0;
  virtual bool result() const=0;
  virtual Modes mode(void *) const {return Modes::Normal;};
};

/**
* The EmptyCmds is for items that do not handle nav cmds
* they can however react to activation and return a true or false version
*/
template<bool res=false>
struct EmptyCmds:public CmdAgent {
  bool canNav() const override {return false;}
  bool result() const override {return res;};
  bool up (void* o)  override {return false;}
  bool down (void* o)  override {return false;}
  bool enter (void* o)  override {return false;}
  bool esc (void* o)  override {return false;}
};

/**
* The ItemCmd class provides access to navigation functions of a specific item
* system generated this types automatically and maps to object functions
*/
template<typename I,bool res=true>
struct ItemCmd:public CmdAgent {
  bool canNav () const override {return true;}
  bool result() const override {return res;};
  bool up(void* o) override {return ((I*)o)->up();}
  bool down(void* o) override {return ((I*)o)->down();}
  bool enter(void* o) override {return ((I*)o)->enter();}
  bool esc(void* o) override {return ((I*)o)->esc();}
  Modes mode(void* o) const override {return ((I*)o)->mode();};
};

/** @}*/

/**
* The Item class, defines menu item interface
*/
struct Item {
  virtual inline void print(NavNode&,MenuOut& out) {}
  virtual inline void printItem(NavNode&,MenuOut& out,idx_t n) {}
  virtual inline void enable(idx_t,bool) {}
  virtual inline bool enabled(idx_t) const {return true;}
  virtual inline NavAgent activate()=0;
  virtual inline NavAgent activateItem(idx_t)=0;
};

/**
* The Empty class is the static base for menu item elements.
* It provides minimalist or inexistent implementations.
*/
template<typename I=Nil>
struct Empty:public I {
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
  // constexpr static inline NavAgent activate() {return false;}
  /// activate collection item (handle enter/select)
  // constexpr static inline NavAgent activateItem(idx_t) {return false;}
  // inline Item& operator[](idx_t) {return *(Item*)this;}
  /// returns a dumb agent to be used by navigation
  static inline NavAgent activate();
  /// activate collection item by index
  static inline NavAgent activateItem(idx_t);
  /// the dumb navigation agent, meaning this item does not handle navigation
  static EmptyCmds<false> cmds;
  constexpr static inline Modes mode() {return Modes::Normal;}
  constexpr static inline bool parentDraw() {return false;}
};

/** \defgroup Agents Command and navigation agents
 *  @{
 */

/**
* The NavAgent class allow navigation system access to specific item
* navigation functions.
*/
struct NavAgent {
  void* obj;
  CmdAgent* run;//we will derive this one, it will know the void final type
  inline NavAgent():obj(NULL),run(Empty<>::activate().run) {}
  inline NavAgent(void* o,CmdAgent* r):obj(o),run(r) {}
  inline NavAgent(const NavAgent& o):obj(o.obj),run(o.run) {}
  inline NavAgent operator=(NavAgent&& o) {obj=o.obj;run=o.run;return o;}
  inline operator bool() const {return run->canNav();}
  inline bool canNav() const {return run->canNav();}
  inline bool up() {return run->up(obj);}
  inline bool down() {return run->down(obj);}
  inline bool enter() {return run->enter(obj);}
  inline bool esc() {return run->esc(obj);}
  inline bool result() const {return run->result();};
  inline Modes mode() const {return run->mode(obj);};
};

/**
* @brief means the item has no associated action
* @return always false, the item will not handle navigation
* @details this is a comodity
*/
inline bool doNothing() {return false;}

/**
* The NavHandler class allow menu item to receive navigation commands.
* Navigation functions are mapped automatically
*/
template<typename I>
class NavHandler:public I {
  public:
    using I::I;
    using This=NavHandler<I>;
    inline NavAgent activate() {return {this,&cmds};}
  protected:
    static ItemCmd<This> cmds;
};

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

/**
* The Action class associates an actikon function with a menu item.
*/
template<typename I,ActionHandler act=doNothing>
class Action:public I {
  public:
    using I::I;
    using This=Action<I,act>;
    inline NavAgent activate() {
      if (act()) return {this,&cmds};
      else return Empty<>::activate();
    }
  protected:
    static EmptyCmds<true> cmds;
};

/** @}*/

/**
* The StaticText class provides code stored constant strings.
*/
template<const char**text,typename I=Empty<>>
struct StaticText:public I {
  using I::I;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.template raw(text[0]);
    I::print(nav,out);
  }
};

// old Action object
// using ActionHandler=bool (*)();
// static inline bool doNothing() {return false;}
// template<typename I,ActionHandler act=doNothing>
// struct Action:public I {
//   using This=Action<I,act>;
//   using I::I;
//   inline NavAgent activate() {return act();}
// };

/**
* The StaticMenu class represents a compile time menu structure.
*/
template<typename I,typename... II>
class StaticMenu:public StaticMenu<I> {
  public:
    using StaticMenu<I>::StaticMenu;
    using This=StaticMenu<I>;
    using Next=StaticMenu<II...>;
    constexpr inline idx_t size() {return next.size()+1;}
    template<typename Nav,typename Out>
    inline void printItem(Nav& nav,Out& out,idx_t n) {
      n?next.template printItem<Nav,Out>(nav,out,n-1):I::print(nav,out);
    }
    inline void enable(idx_t n,bool o) {
      if (n) next.enable(n-1,o);
      else I::enable(n,o);
    }
    inline bool enabled(idx_t n) const {
      trace(MDO<<"StaticMenu<I,II...>::enabled"<<endl);
      return n?next.enabled(n-1):I::enabled(0);
    }
    inline NavAgent activateItem(idx_t n) {return n?next.activateItem(n-1):This::activate();}
    // inline Item& operator[](idx_t n) {
    //   trace(MDO<<"StaticMenu<I,II...>::operator[] "<<n<<endl);
    //   return n?next.operator[](n-1):*reinterpret_cast<Item*>(this);
    // }
  protected:
    Next next;
};

/**
* The StaticMenu specialization class represents the last element of a compile-time
* menu content chain.
*/
template<typename I>
struct StaticMenu<I>:public I {
  using I::I;
  using This=StaticMenu<I>;
  constexpr static inline idx_t size() {return 1;}
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {}
  template<typename Nav,typename Out>
  inline void printItem(Nav& nav,Out& out,idx_t) {I::print(nav,out);}
  inline bool enabled(idx_t n) const {
    trace(MDO<<"StaticMenu<I>::enabled"<<endl);
    return n?true:I::enabled(0);
  }
  inline void enable(idx_t n,bool o) {
    if(!n) I::enable(n,o);
  }
  // inline NavAgent activate() {return I::act();}
  // inline Item& operator[](idx_t n) {
  //   trace(MDO<<"StaticMenu<I>::operator[] "<<n<<endl);
  //   return *reinterpret_cast<Item*>(this);
  // }
};

/**
* The Prompt class represents a generic virtual menu item.
* This class allows Prompt pointers to be stored on a list as they all implement the same interface.
* This will adapts a composition to the virtual interface.
*/
template<typename I>
struct Prompt:public Item,public I {
  using I::I;
  inline void printItem(NavNode& nav,MenuOut& out,idx_t n) override {
    I::template printItem<NavNode,MenuOut>(nav,out,n);
  }
  inline void print(NavNode& nav,MenuOut& out) override {
    I::print(nav,out);
  }
  inline void enable(idx_t n,bool b) override {I::enable(n,b);}
  virtual inline bool enabled(idx_t n) const override {return I::enabled(n);}
  inline NavAgent activate() override {return I::activate();}
  inline NavAgent activateItem(idx_t n) override {return I::activateItem(n);}
};

/**
* The AsUnit class signals the format system to handle inner content as an unit (normaly append text after a value)
*/
template<typename I>
struct AsUnit:public I {
  using I::I;
  using This=AsUnit<I>;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.template fmtUnit<true,Nav,Out,This>(nav,out,*this,0);
    I::print(nav,out);
    out.template fmtUnit<false,Nav,Out,This>(nav,out,*this,0);
  }
};

/**
* The AsMode class signals the format system to handle inner content as a edit mode cursor.
*/
template<typename I>
struct AsMode:public I {
  using I::I;
  using This=AsMode<I>;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.template fmtMode<true,Nav,Out,This>(nav,out,*this,0);
    I::print(nav,out);
    out.template fmtMode<false,Nav,Out,This>(nav,out,*this,0);
  }
};

/**
* The AsValue class signals the format system to handle inner content as a value.
*/
template<typename I>
struct AsValue:public I {
  using I::I;
  using This=AsValue<I>;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.template fmtValue<true,Nav,Out,This>(nav,out,*this,0);
    I::print(nav,out);
    out.template fmtValue<false,Nav,Out,This>(nav,out,*this,0);
  }
};

/** @}*/
