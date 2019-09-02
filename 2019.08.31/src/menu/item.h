/* -*- C++ -*- */
#pragma once
/**
* @file item.h
* @author Rui Azevedo
* @brief ArduinoMenu data/structure representation and base components
*/


#include "base.h"

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
  virtual Modes mode(void*) const {return Modes::Normal;}
};

/**
* The EmptyCmds is for items that do not handle nav cmds
* they can however react to activation and return a true or false version
*/
template<bool res=false>
struct EmptyCmds:public CmdAgent {
  bool canNav() const override {return false;}
  bool result() const override {return res;};
  bool up (void*)  override {return false;}
  bool down (void*)  override {return false;}
  bool enter (void*)  override {return false;}
  bool esc (void*)  override {return false;}
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

//////////////////////////////////////////////
/**
* The Empty class is the data terminal
*/
template<typename I>
struct Empty:public I {
  template<typename N,typename O,typename H>
  static inline void print(N&,O&,H&) {}
  template<typename N,typename O,typename H>
  static inline void printItem(N&,O&,H&,idx_t) {}
  // constexpr static inline bool isRange() {return false;}
  constexpr static inline idx_t size() {return 0;}
  /// is this item enabled?
  constexpr static inline bool enabled() {return true;}
  /// get enabled status of collection indexed item
  constexpr static inline bool enabled(idx_t) {return true;}
  /// set enabled status of indexed collection member
  static inline void enable(idx_t,bool) {}
  /// returns a dumb agent to be used by navigation
  static inline NavAgent activate();
  /// activate collection item by index
  static inline NavAgent activateItem(idx_t);
  /// the dumb navigation agent, meaning this item does not handle navigation
  static EmptyCmds<false> cmds;
  constexpr static inline bool up() {return false;}
  template<typename Nav> constexpr static inline bool up   (Nav& nav,idx_t at=0) {return false;}
  constexpr static inline bool down() {return false;}
  template<typename Nav> constexpr static inline bool down (Nav& nav,idx_t at=0) {return false;}
  template<typename Nav> constexpr static inline bool left (Nav& nav,idx_t at=0) {return false;}
  template<typename Nav> constexpr static inline bool right(Nav& nav,idx_t at=0) {return false;}
  constexpr static inline bool enter() {return false;}
  template<typename Nav> constexpr static inline bool enter(Nav& nav,idx_t at=0) {return false;}
  constexpr static inline bool esc() {return false;}
  template<typename Nav> constexpr static inline bool esc  (Nav& nav,idx_t at=0) {return false;}
  constexpr static inline Modes mode() {return Modes::Normal;}
  template<typename Out> static inline void printMenu(Out& out) {}
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
    template<typename Nav> inline bool enter(Nav& nav) {return act();}
    inline NavAgent activate() {
      if (act()) return {this,&cmds};
      else return Empty<>::activate();
    }
  protected:
    static EmptyCmds<true> cmds;
};
/** @}*/

/**
* The StaticText class is a data composition to hold simple text data (const char*)
*/
template<const char**text,typename I=Empty<>>
struct StaticText:public I {
  template<typename N,typename O,typename H>
  inline void print(N& n,O& o,H& i) {
    o.raw(text[0]);
    I::template print<N,O,H>(n,o,i);
  }
};

/**
* The StaticMenu class is a data composition to hold a set of data items
* this data enumeration can't be changed either in order or in size
* as its structure will NOT be stored on the final program (compile time structure)
* data items can however be dynamic or of mixed types
*/
template<typename I,typename... II>
class StaticMenu:lpp::List<I,II...> {
  public:
    // using This=StaticMenu<I,II...>;
    // using Next=StaticMenu<II...>;
    // friend Next;
    template<typename Prompt>
    static inline idx_t selIdx(Prompt& i,idx_t n) {return n?i.next.selIdx(i,n-1):selIdx(i);}
    constexpr static inline idx_t size() {return Next::size()+1;}
    inline void enable(idx_t n,bool o) {
      if (n) next.enable(n-1,o);
      else I::enable(n,o);
    }
    inline bool enabled(idx_t n) const {
      trace(MDO<<"StaticMenu<I,II...>::enabled"<<endl);
      return n?next.enabled(n-1):I::enabled(0);
    }
    template<typename N,typename O,typename H>
    inline void printItem(N& n,O& o,H& i,idx_t c) {
      trace(MDO<<"StaticMenu<...>::printItem "<<c<<endl);
      c>0?i.next.printItem(n,o,next,c-1):I::print(n,o,i);
    }
    inline NavAgent activateItem(idx_t n) {
      trace(MDO<<"StaticMenu<I,II...>::activateItem "<<n<<endl);
      return n?next.activateItem(n-1):This::activate();
    }
    template<typename Nav>
    inline bool enter(Nav& nav,idx_t at=0) {
      return at?next.enter(nav,at-1):I::enter(nav);
    }
    template<typename Out>
    inline void printMenu_node(Out& out,idx_t n) {
      _trace(MDO<<"StaticMenu::printMenu_node "<<(int)n<<endl);
      if(n) next.printMenu_node(out,n-1); else This::printMenu_node(out,0);
    }
    template<typename Nav>
    inline bool enter_node(Nav& nav,idx_t n) {
      return n?next.enter_node(nav,n-1):This::enter_node(nav,0);
    }
    template<typename Nav>
    inline bool up_node(Nav& nav,idx_t n) {
      return n?next.up_node(nav,n-1):This::up_node(nav,0);
    }
    template<typename Nav>
    inline bool down_node(Nav& nav,idx_t n) {
      return n?next.down_node(nav,n-1):This::down_node(nav,0);
    }
  protected:
    Next next;
};

/*
* this is the StaticMenu terminal specialization (last item)
*/
// template<typename I>
// struct StaticMenu<I>:public I {
//   using This=StaticMenu<I>;
//   constexpr static inline idx_t size() {return 1;}
//   inline void enable(idx_t n,bool o) {
//     if(!n) I::enable(n,o);
//   }
//   inline bool enabled(idx_t n) const {
//     trace(MDO<<"StaticMenu<I>::enabled"<<endl);
//     return n?true:I::enabled(0);
//   }
//   template<typename N,typename O,typename H>
//   inline void print(N& n,O& o,H& i) {/*I::print(n,o,i);*/}
//   template<typename N,typename O,typename H>
//   static inline void printItem(N& n,O& o,H& i,idx_t c) {
//     trace(MDO<<"StaticMenu::printItem "<<c<<endl);
//     i.I::print(n,o,i);
//   }
//   inline NavAgent activateItem(idx_t n) {
//     trace(MDO<<"StaticMenu<I>::activateItem "<<n<<endl);
//     return This::activate();
//   }
//   template<typename Out>
//   inline void printMenu_node(Out& out,idx_t n) {
//     _trace(MDO<<"StaticMenu<I>::printMenu_node"<<endl);
//     I::printMenu(out);
//   }
//   template<typename Nav> inline bool enter_node(Nav& nav,idx_t n) {I::enter(*this);}
//   template<typename Nav> inline bool up_node(Nav& nav,idx_t n) {I::up();}
//   template<typename Nav> inline bool down_node(Nav& nav,idx_t n) {I::down();}
// };

/**
* SelfNav class allows item to also be a navigation system
* needed fot static tree navigation? will see
*/
// template<typename I,typename N=NavPosBase<>>
// struct SelfNav:public I,public N {
//   template<typename O>
//   inline void printMenu(O& out) {out.print(*this,out,*this);}
// };

/**
* The StaticItem is the top level static composition that encapsulates data structures compositions
* it should be compatible with the virtual interface
*/
template<typename I>
struct StaticItem:public I {
  using This=StaticItem<I>;
  template<typename N,typename O,typename H>
  inline void print(N& n,O& o,H& i) {
    I::template print<N,O,This>(n,o,i);
  }
  static inline constexpr bool enter() {return false;}//no, we do not accept navigation
  static inline constexpr bool esc() {return false;}
  static inline constexpr bool up() {return false;}
  static inline constexpr bool down() {return false;}
};

/**
* The Item class is the virtual data interface definition
*/
struct Item {
  virtual inline void print(INavNode&,IMenuOut&) {}
};

/**
* The Prompt class is the top level virtual data implementation
* it should encapsulate a data compsition and abide to the virtual interface
*/
template<typename I>
struct Prompt:public Item, public I {
  inline void print(INavNode& nav,IMenuOut& out) override {I::print(nav,out,(I&)*this);}
  template<typename N,typename O,typename H>
  inline void print(N& nav,O& out,H& i) {I::print(nav,out,i);}
};

/**
* The AsUnit class signals the format system to handle inner content as an unit (normaly append text after a value)
*/
template<typename I>
struct AsUnit:public I {
  using I::I;
  using This=AsUnit<I>;
  template<typename N,typename O,typename H>
  inline void print(N& nav,O& out,H& i) {
    out.template fmtUnit<true,N,O,This>(nav,out,*this,0);
    I::print(nav,out,i);
    out.template fmtUnit<false,N,O,This>(nav,out,*this,0);
  }
};

/**
* The AsMode class signals the format system to handle inner content as a edit mode cursor.
*/
template<typename I>
struct AsMode:public I {
  using I::I;
  using This=AsMode<I>;
  template<typename N,typename O,typename H>
  inline void print(N& nav,O& out,H& i) {
    out.template fmtMode<true,N,O,This>(nav,out,*this,0);
    I::print(nav,out,i);
    out.template fmtMode<false,N,O,This>(nav,out,*this,0);
  }
};

/**
* The AsValue class signals the format system to handle inner content as a value.
*/
template<typename I>
struct AsValue:public I {
  using I::I;
  using This=AsValue<I>;
  template<typename N,typename O,typename H>
  inline void print(N& nav,O& out,H& i) {
    out.template fmtValue<true,N,O,This>(nav,out,*this,0);
    I::print(nav,out,i);
    out.template fmtValue<false,N,O,This>(nav,out,*this,0);
  }
};
