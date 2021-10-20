/* -*- C++ -*- */
#pragma once

// #include "sys/kernel/path.h"
#include "menu/out.h"

namespace Menu {

  //API calls
  //agensts to call member function on post provided objects
  APIDEF(get,GetValue);
  APIDEF(valueIs,ValueIs);
  APIDEF(isDefault,IsDefault);
  APIDEF(setDefault,SetDefault);
  APIDEF(styles,Styles);
  APIDEF(enabled,Enabled);
  APIDEF(enable,Enable);
  // APIDEF(relax,Relax);
  APIDEF(poll,Poll);

  APIDEF(sz,Sz);
  APIDEF(onPrintTitleTo,PrintTitleTo);
  APIDEF(onPrintBodyTo,PrintBodyTo);
  APIDEF(onPrintItemTo,PrintItemTo);
  APIDEF(esc,Esc);
  APIDEF(up,Up);
  APIDEF(down,Down);

  // APIDEF(printTo,PrintTo);
  struct PrintTo {
    // constexpr static const char* named="PrintTo";
    template<typename T,typename Nav,typename Out>
    inline auto operator()(T& o,Nav& nav,Out& out,int n=0,bool d=true)
      ->decltype(o.template printTo<Nav,Out>(nav,out,n,d)) 
      {return o.template printTo<Nav,Out>(nav,out,n,d);}
  };

  // APIDEF(onPrintMenuTo,PrintMenuTo);
  struct PrintMenuTo {
    // constexpr static const char* named="PrintMenuTo";
    // _trace(MDO& operator<<(MDO& o) const {return o<<named;});
    template<typename T,typename Nav,typename Out>
    inline auto operator()(T& o,Nav& nav,Out& out,int n)
      ->decltype(o.template onPrintMenuTo<Nav,Out>(nav,out,n)) 
      {return o.template onPrintMenuTo<Nav,Out>(nav,out,n);}
  };
  // _trace(MDO& operator<<(MDO& o,const PrintMenuTo& api) {return api.operator<<(o);});


  // APIDEF(enter,Enter);
  struct Enter {
    // constexpr static const char* named="Enter";
    // _trace(MDO& operator<<(MDO& o) const {return o<<named;})
    template<typename T,typename Nav>
    inline auto operator()(T& o,Nav& nav)
      ->decltype(o.template enter<Nav>(nav)) 
      {return o.template enter<Nav>(nav);}
  };
  // _trace(MDO& operator<<(MDO& o,const Enter& api) {return api.operator<<(o);});

  template<Cmd c>
  struct CmdTo {
    // constexpr static const char* named="CmdTo";
    // _trace(MDO& operator<<(MDO& o) const {return o<<named<<"<"<<c<<">";});
    template<typename T,typename Nav>
    inline auto operator()(T& o,Nav& nav,int code)
      ->decltype(o.template cmd<Nav,c>(nav,code)) 
      {return o.template cmd<Nav,c>(nav,code);}
  };
  // _trace(
  //   template<Cmd c>
  //   MDO& operator<<(MDO& o,const CmdTo<c>& api) {return api.operator<<(o);}
  // );

  // #ifdef DEBUG
  //   template<typename API,typename... Args>
  //   MDO& operator<<(MDO& o,const Map<API,Args...>& api) {return api.operator<<(o);}
  // #endif

  // Item API ----------------------------
  template<typename O=Nil>
  struct Empty:O {
    using Target=void;
    using Base=O;
    using IsTag=No;
    using Base::Base;
    using Head=Empty<O>;
    using Tail=Empty<O>;
    Empty(){}
    Empty(const Empty&)=delete;
    Empty& operator=(const Empty&)=delete;
    inline static void setDefault() {}
    // template<typename Next,typename... Args>
    // static Next build(Args... args) {return Next(args...);}
    Tail& head() {return *this;}
    Tail& tail() {return *this;}
    // static void names() {_trace(MDO::print("."));}
    IItem* operator[](size_t i) {assert(false);}
    inline static constexpr bool get() {return true;}
    inline static void begin() {}
    // inline static void relax() {}
    inline static constexpr bool canWrap() {return false;}
    template<typename T> constexpr static bool valueIs(T&) {return false;}
    inline static constexpr bool isTop() {return false;}
    inline static constexpr Style styles() {return Style::None;}
    inline static constexpr bool is(Idx mask) {return ((Idx)Base::obj().styles())==mask;}
    inline static constexpr bool has(Idx mask) {return ((Idx)Base::obj().styles())&mask;}
    inline static constexpr bool changed() {return false;}
    inline static constexpr bool enabled() {return true;}
    inline static void enable(bool b) {}
    template<typename Dev> inline static void poll(Dev&) {
      // _trace(
      //   static TinyTimeUtils::Tick<500> dots;
      //   if(dots) Serial.write('.')
      // );
    }
    template<typename Nav,typename Out,bool=true> 
    static void printTo(Nav& nav,Out&,int n=0,bool sel=false) {}
    template<typename Nav,typename Out> void onPrintMenuTo(Nav& nav,Out& out,Idx level,Idx selIdx) {out.printMenu(nav,O::obj(),level+1,selIdx);}
    template<typename Nav,typename Out> void onPrintTitleTo(Nav& nav,Out& out) {out.printTitle(nav,O::obj());}
    template<typename Nav,typename Out> void onPrintBodyTo(Nav& nav,Out& out,Idx selIdx,Idx n)
      {printTo(nav,out,selIdx,n==selIdx);}
    template<typename Nav,typename Out> void onPrintItemTo(Nav& nav,Out& out,Idx level,int n,bool sel) 
      {
        // _trace(clog<<"Empty::onPrintItemTo->printTo"<<endl;clog.flush());
        Base::obj().template printTo<Nav,Out>(nav,out,n,sel);}
    template<typename T> static constexpr bool chkId(T n) {return false;}
    static constexpr Idx sz() {return 1;}
    static constexpr Idx _sz() {return 1;}
    template<typename Nav> constexpr static bool up(Nav&) {return true;}
    template<typename Nav> constexpr static bool down(Nav&) {return true;}
    template<typename Nav> constexpr static bool enter(Nav&) {return false;}
    template<typename Nav> constexpr static bool esc(Nav&) {return false;}
    template<typename Nav> constexpr static bool left(Nav& nav) {return enter(nav);}
    template<typename Nav> constexpr static bool right(Nav& nav) {return esc(nav);}
    template<typename Nav> constexpr static bool key(Nav&,Key) {return false;}
    inline static constexpr bool action() {return true;}
    template<typename Nav,Cmd c>
    bool cmd(Nav& nav,int code=0) {
      // _trace(clog<<"Empty::cmd->obj()..."<<endl);
      switch(c) {
        case Cmd::Enter: return O::obj().enter(nav);
        case Cmd::Esc: return O::obj().esc(nav);
        case Cmd::Up: return O::obj().up(nav);
        case Cmd::Down: return O::obj().down(nav);
        default: return false;
      }
    }

    template<typename API,typename... Args>
    auto map(Args... args) 
      ->decltype(API().operator()(*this,args...))
      {return API().operator()(*this,args...);}
      
    template<typename Target,Idx o,Idx... oo>
    struct Walk {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args) 
        ->decltype(API().operator()(target,args...)) 
        {return API().operator()(target,args...);}
    };
    struct RefWalk {
     RefWalk(Idx l,Idx*) {assert(!l);}
     template<typename Target,typename API,typename... Args>
      auto step(Idx at,Target& target,Args... args) 
        ->decltype(API().operator()(target,args...))
        {return API().operator()(target,args...);}
    };
    // template<typename Nav,typename Item>
    // void printItem(Nav& nav,Item& i,Idx n=0,bool sel=false) {
    //   _trace(clog<<"Empty::printItem"<<endl);
    //   i.onPrintItemTo(nav,Base::obj(),n,sel);
    // }
};

  template<template<typename> class Base,template<typename> class... I>
  using ItemDefBase=Composition<Base,I...,Empty>;

  //IItem class
  //The virtual Item API interface
  class IItem {
    public:
      static void names() {}
      virtual IItem* operator[](size_t i) {assert(false);return this;}
      virtual Style styles() const {return Style::None;}
      virtual bool enabled() const {return true;}
      virtual void enable(bool) {}
      virtual bool cmd(INav&,Cmd,int) {return false;}
      virtual void printTo(INav& nav,IOut&,int=0,bool=false){}
      virtual void onPrintMenuTo(INav& nav,IOut&,int) {}
      virtual void onPrintTitleTo(INav& nav,IOut& out) {out.printTitle(nav,*this);}
      virtual void onPrintBodyTo(INav& nav,IOut& out,Idx,Idx) {}
      virtual void onPrintItemTo(INav& nav,IOut& out,int n=0,bool sel=false) {}
      template<typename Nav,Cmd c> bool cmd(INav& nav,int code=0) {return cmd(nav,c,code);}
      virtual Idx sz() const {return 1;}
      template<typename Nav,typename Out,bool=true> 
      static void printTo(Nav& nav,Out& out,int n=0,bool sel=false)
        {printTo(nav,out,n);}
  };

  //Prompt class
  //encapsulate static item definition for use on dynamic containers
  class Prompt {
    public:
      template<typename I>
      struct Part:IItem, I {
      using Base=I;
      using This=Part<Base>;
      using Base::Base;
      using Base::cmd;
      bool cmd(INav& nav,Cmd c,int code) override {
        switch(c) {
          case Cmd::Up: return Base::up(nav);
          case Cmd::Down: return Base::down(nav);
          case Cmd::Left: return Base::left(nav);
          case Cmd::Right: return Base::right(nav);
          case Cmd::Enter: return Base::enter(nav);
          case Cmd::Esc: return Base::esc(nav);
          case Cmd::KeyCode: return Base::key(nav,code);
          default: return false;
        }
      }

      IItem* operator[](size_t i) override {return Base::operator[](i);}
      Style styles() const override {return Base::styles();}
      bool enabled() const override {return Base::enabled();}
      void enable(bool b) {Base::enable(b);}
      void printTo(INav& nav,IOut& out,int n=0,bool sel=false) override
        {Base::template printTo<INav,IOut>(nav,out,n,sel);}
      void onPrintMenuTo(INav& nav,IOut& out,int n) override {out.printMenu(nav,Base::obj(),n);}
      void onPrintTitleTo(INav& nav,IOut& out) override {Base::onPrintTitleTo(nav,out);}
      void onPrintBodyTo(INav& nav,IOut& out,Idx selIdx,Idx n) override 
        {Base::onPrintBodyTo(nav,out,selIdx,n);}
      void onPrintItemTo(INav& nav,IOut& out,int n=0,bool sel=false) override 
        {Base::template onPrintItemTo<INav,IOut>(nav,out,n,sel);}
      Idx sz() const override {return Base::sz();}
    };
  };

};//Menu namespace
