/* -*- C++ -*- */
#pragma once

// #include "../menu.h"
#include "base.h"
#include "out.h"

namespace AM5 {

  //can we config like this?
  // using Config=AM5::Config<
  //   wrapMenus::yes,
  //   canExit::yes,
  //   useFields::yes,
  //   useActions::yes,
  //   itemBase<AM5::Nil>,
  //   outBase<AM5::Nil>,
  //   manageIdle::no,
  //   navStyle<AM5::FlatNav>,
  //   titleWrapChars<'[',']'>,
  //   textCursors<' ','>','-'>
  // } myConfig;

  //not sure if i implement This
  //it will grow the vtable footprint a bit
  //or maybe not as it is only requested for containers
  //and we only need 1 virtual to get an iterator
  // template<typename I>
  // struct ItemIterator {
  //   size_t at=0;
  //   virtual inline I operator*() {return data[at];}
  //   virtual inline I operator==(const It o) const {return at==o.at;}
  //   virtual inline I operator!=(const It o) const {return !operator==(o);}
  //   virtual inline I operator++() {return data[++at];}
  //   virtual inline I operator++(I) {return data[at++];}
  // };

  //events should be flagged start/end
  //this should be used only for drawing custom items
  //still is preferable to extend a class
  //using this can encourage users to allocate/deallocate resources
  //and that does not play well with async request
  //as for web
  //for compatinilioty we should have them
  // make them optional thou!
  // enum class Events {
  //   enterEvent,
  //   exitEvent,
  //   updateEvent,
  //   focusEvent,
  //   blurEvent,
  //   ...
  // };

  enum class Styles {
    CanNav,//nav commands should be send to this item
    Wrap,//this menu wraps (numFields can look at this too)
    Action,//can receive enter
  };

  ///////////////////////////////////////////////////////////////
  // navigatiojn commands -----------------------------------
  // menu items that wish to receive navigation commands should emit
  // the respective command pallete object
  // and empty pallete is emited for items that do not receive
  // commands...
  // navigations requests `activate` from the item
  // however any menu wished to receive enter
  // sop we should switch this to enter instead

  //represents an item that can receive navigation commands
  struct CmdAgent {
    virtual bool canNav() const =0;
    virtual bool up(void* o)=0;
    virtual bool down(void* o)=0;
    virtual bool enter(void* o)=0;
    virtual bool esc(void* o)=0;
    virtual bool result() const=0;
  };

  //for items that do not handle nav cmds
  template<bool res=false>
  struct EmptyCmds:public CmdAgent {
    bool canNav() const override {return false;}
    bool result() const override {return res;};
    bool up (void* o)  override {return false;}
    bool down (void* o)  override {return false;}
    bool enter (void* o)  override {return false;}
    bool esc (void* o)  override {return false;}
  };

  //nav commands of specific item
  template<typename O,bool res=true>
  struct ItemCmd:public CmdAgent {
    bool canNav () const override {return true;}
    bool result() const override {return res;};
    bool up(void* o) override {return ((O*)o)->up();}
    bool down(void* o) override {return ((O*)o)->down();}
    bool enter(void* o) override {return ((O*)o)->enter();}
    bool esc(void* o) override {return ((O*)o)->esc();}
  };

  struct NavAgent;

  ///////////////////////////////////////////////////////////////
  // menu items -----------------------------------

  struct Item {
    //footprint:
    // 4 bytes for each virtual function * #virtual tables
    // the # of vtables is equal to the # of unique Prompt<...> compositions
    virtual void out(MenuOut& o) const {}
    virtual size_t size() const {return 1;}
    virtual Item& operator[](size_t)=0;// const {return *this;}
    virtual NavAgent activate()=0;// {assert(false);return CmdAgent();};
  };

  //static composition blocks -----------------------
  struct Empty {
    inline Empty() {}
    inline Empty(Empty&) {}
    static inline void out(MenuOut&) {}
    static inline size_t size() {return 1;}
    inline Item& operator[](size_t n) {return *reinterpret_cast<Item*>(this);}
    static inline NavAgent activate();
    static inline bool up() {return false;}
    static inline bool down() {return false;}
    static inline bool enter() {return false;}
    static inline bool esc() {return false;}
    static EmptyCmds<false> cmds;
  };

  struct NavAgent {
    void* obj;
    CmdAgent* run;//we will derive this one, it will know the void final type
    inline NavAgent():obj(NULL),run(Empty::activate().run) {}
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
  };

  inline bool doNothing() {return false;}

  inline NavAgent Empty::activate() {return {NULL,&cmds}; }

  template<typename O>
  class NavHandler:public O {
    public:
      using O::O;
      using This=NavHandler<O>;
      inline NavAgent activate() {return {this,&cmds};}
    protected:
      static ItemCmd<This> cmds;
  };

  using ActionHandler=bool (*)();
  template<typename O,ActionHandler act=doNothing>
  class Action:public O {
    public:
      using This=Action<O,act>;
      using O::O;
      inline NavAgent activate() {
        if (act()) return {this,&cmds};
        else return Empty::activate();
      }
    protected:
      static EmptyCmds<true> cmds;
  };

  //adapt specific types as menu items
  //provide virtual overrides for them
  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    using This=Prompt<O>;
    inline void out(MenuOut& o) const override {O::out(o);}
    size_t size() const override {return O::size();}
    Item& operator[](size_t n) override {return O::operator[](n);}
    inline NavAgent activate() override {return O::activate();}
    //not used yet --
    template<template<typename> class T>
    inline void stack(MenuOut& o) const {Prompt<T<O>>(*this).out(o);}
  };

  template<typename O>
  struct Text:public O {
    const char* text;
    inline Text(const char* text):text(text) {}
    inline Text(const Text<O>& t):text(t.text) {}
    inline void out(MenuOut &o) const {
      o<<text;
      O::out(o);
    }
  };

  //this provides only a size hint for navigation
  //or an iterator if its the case (TODO)
  template<size_t n,typename O=Empty>
  struct EmptyMenu:public O {
  public:
    using O::O;
    // inline EmptyMenu(const char*title):O(title) {}
    static inline size_t size() {return n;}
  };

  template<size_t n,typename O>
  class StaticMenu:public O {
  protected:
    Item* data[n];
  public:
    template<typename... OO>
    inline StaticMenu(OO... oo):data{oo...} {}
    template<typename... OO>
    inline StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
    static inline size_t size() {return n;}
    inline Item& operator[](size_t i) {return *data[i];}
  };

};//AM5
