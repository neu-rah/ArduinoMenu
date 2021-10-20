/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief static (type defined/compile time) menu
* no structure data will be stored, all beavior is implemented by code instead of data
* making this structure as thin as user encoded menus (pure functional, no data)
* just as hand coded menus for small devices (low ram)
*/

#include "../parts/src/parts.h"
using namespace Parts;

#include "../item.h"

namespace Menu {
  template<typename O,typename... OO>
  struct StaticList:Node<O,StaticList<OO...>> {
    // named("StaticList<O,OO...>");
    using Tail=StaticList<OO...>;
    using Base=Node<O,Tail>;
    using This=StaticList<O,OO...>;
    using Base::Base;
    // template<typename... Args>
    // StaticList(Args... args):Base(args...,Base::template build<Tail,Args...>(args...)) {}
    template<typename Nav,typename Out>
    void onPrintBodyTo(Nav& nav,Out& out,Idx selIdx,Idx n)  {
      out.template printItem<Nav,This>(nav,*this,n,n==selIdx);
      Base::tail().template onPrintBodyTo<Nav,Out>(nav,out,selIdx,n+1);
    }
    static constexpr Idx len() {return Base::_sz();}
    void begin() {
      Base::head().begin();
      Base::tail().begin();
    }
};

  template<typename O>
  struct StaticList<O>:Node<O,Nil> {
    // named("StaticList<O>");
    using Base=Node<O,Nil>;
    using This=StaticList<O>;
    using Base::Base;
    template<typename... Args>
    StaticList(Args... args):Base(args...) {}
    template<typename Nav,typename Out>
    void onPrintMenuTo(Nav& nav,Out& out,Idx level,Idx selIdx) {out.printMenu(nav,*this,level+1,selIdx);}
    template<typename Nav,typename Out>
    void onPrintBodyTo(Nav& nav,Out& out,Idx selIdx,Idx n) {out.printItem(nav,*this,n,selIdx==n);}
    static constexpr Idx len() {return Base::_sz();}
  };

  template<typename O,typename... OO>
  using StaticMenuBody=StaticList<O,OO...>;

  template<typename Title,typename Body>
  struct StaticMenu:IsMenu::Part<Body> {
    Title title;
    // named("StaticMenu<Title,Body...>");
    using Base=typename IsMenu::Part<Body>;
    using This=StaticMenu<Title,Body>;
    using Base::Base;
    // template<typename... Args>
    // StaticMenu(Args... args):Title(args...),Base(Title::template build<Base,Args...>(args...)) {}
    template<typename Nav,typename Out,bool delegate=true>
    void printTo(Nav& nav,Out& out,int n=0,bool sel=false) {
      title.template printTo<Nav,Out>(nav,out,n,sel);
    }
    template<typename Nav,typename Out>
    void onPrintMenuTo(Nav& nav,Out& out,Idx level,Idx selIdx) 
      {out.template printMenu<Nav,This>(nav,*this,level+1,selIdx);}
    template<typename Nav,typename Out>
    void onPrintTitleTo(Nav& nav,Out& out) 
      {out.printTitle(nav,title);}
    template<typename Nav,typename Out>
    void onPrintItemTo(Nav& nav,Out& out,Idx level,Idx n=0,bool sel=0) {
      //TODO: allow combination of the types (mask) instead of the combination of results!
      // so that this is possoble `(has<Style::PadDraw&&Style::ParentDraw>(Base::styles())`
      // need to change `has` & `is` from Style to decltype(Style)? C++ should know it better...
      if(has<Style::PadDraw>(Base::styles())&has<Style::ParentDraw>(Base::styles())) {
        out.padOn();
        out.template printMenu<Nav,This>(nav,*this,nav.pos());//TODO: nav pos() need to b corrected & menus need a preview print
        out.padOff();
      } else title.printTo(nav,out,n,sel);
}

    // #define keyWalker(f,n)
    // template<int... path>
    // bool f() {
    //   using Call=typename This::template Walk<bool,typename This::n>;
    //   return Call().template walk<This,path...>(*this);
    // }

    template<typename Nav> bool enter(Nav& nav) {return true;}
    template<typename Nav,Cmd c> bool cmd(Nav& nav,int code=0) {
      PathRef parent(nav.level,nav.path);
      Style ps=parent.template walk<typename Nav::MenuType,Styles>(nav.data);
      PathRef child(1,nav.at());
      Style s=child.template walk<This,Styles>(*this);
      if(has<Style::WrapNav>(ps)) s=s^Style::WrapNav;
      return nav.template doNav<c>(sz(),s);
    }

    // keyWalker(enter,Enter);
    // keyWalker(esc,Esc);
    // keyWalker(up,Up);
    // keyWalker(down,Down);

    template<typename Nav,typename Out,int... path>
    void printMenuTo(Nav& nav,Out& out,int sel) {
      // _trace(clog<<"StaticMenu::onPrintMenuTo"<<endl;clog.flush());
      Path<path...>::template walk<This,PrintMenuTo,Out,int>(*this,out,sel);
    }
    static constexpr Idx sz() {return Base::_sz();}
  };

};
