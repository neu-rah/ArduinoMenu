/* -*- C++ -*- */
#pragma once

#include "api.h"

////////////////////////////////////////////////////////////////
template<typename It,typename Nav,typename Out> struct PrintAgent;
template<typename It,typename Nav>struct NavAgent;

struct Rookie {
  virtual Idx size()=0;
  virtual bool enabled()=0;
  virtual bool canNav()=0;
  virtual bool isMenu()=0;
  virtual bool activate()=0;
  virtual bool parentDraw()=0;
  virtual bool changed()=0;
  virtual void changed(bool)=0;
  virtual void changed(Idx,bool)=0;
};

template<typename It>
struct Agent:public Rookie {
  It& it;
  inline Agent(It& it):it(it) {}
  inline It& agent() {return it;}
  inline It agent() const {return it;}
};
template<typename It,typename Nav>
struct NavAgent:Agent<It> {
  Nav nav;
  inline NavAgent(Agent<It>& ita):Agent<It>(ita),nav(nav) {}
  using Agent<It>::agent;
  virtual void cmd(Nav& nav,Cmds c) {
    switch(c) {
      case Cmds::None: break;
      case Cmds::Enter: agent().enter();break;
      case Cmds::Esc: agent().esc();break;
      case Cmds::Up: agent().up();break;
      case Cmds::Down: agent().down();break;
      case Cmds::Left: agent().left();break;
      case Cmds::Right: agent().right();break;
    }
  }
};
template<typename It,typename Nav,typename Out>
struct PrintAgent:NavAgent<It,Nav> {
  Out& out;
  inline PrintAgent(Agent<It>& ita,Nav& nav,Out& out):NavAgent<It,Nav>(ita,nav),out(out) {}
  inline PrintAgent(NavAgent<It,Nav>& nava,Out& out):NavAgent<It,Nav>(nava),out(out) {}
  using Agent<It>::agent;
  virtual void print(Out& out,Roles role=Roles::Raw,bool toPrint=true) {
    switch(role) {
      case Roles::Raw: toPrint?agent().template print<Roles::Raw,true>(out):agent().template print<Roles::Raw,false>(out);
      case Roles::Panel: toPrint?agent().template print<Roles::Panel,true>(out):agent().template print<Roles::Panel,false>(out);
      case Roles::Menu: toPrint?agent().template print<Roles::Menu,true>(out):agent().template print<Roles::Menu,false>(out);
      case Roles::Title: toPrint?agent().template print<Roles::Title,true>(out):agent().template print<Roles::Title,false>(out);
      case Roles::Body: toPrint?agent().template print<Roles::Body,true>(out):agent().template print<Roles::Body,false>(out);
      case Roles::Item: toPrint?agent().template print<Roles::Item,true>(out):agent().template print<Roles::Item,false>(out);
      case Roles::Index: toPrint?agent().template print<Roles::Index,true>(out):agent().template print<Roles::Index,false>(out);
      case Roles::Cursor: toPrint?agent().template print<Roles::Cursor,true>(out):agent().template print<Roles::Cursor,false>(out);
      case Roles::Name: toPrint?agent().template print<Roles::Name,true>(out):agent().template print<Roles::Name,false>(out);
      case Roles::Mode: toPrint?agent().template print<Roles::Mode,true>(out):agent().template print<Roles::Mode,false>(out);
      case Roles::Value: toPrint?agent().template print<Roles::Value,true>(out):agent().template print<Roles::Value,false>(out);
      case Roles::Unit: toPrint?agent().template print<Roles::Unit,true>(out):agent().template print<Roles::Unit,false>(out);
    }
  }
  virtual void printMenu(bool) {
    trace(MDO<<"PrintAgent::printMenu"<<endl);
    agent().print(out);}
  static inline void printMenu(bool,It& it,Nav&,Out& out,Ref,Idx) {
    trace(MDO<<"Empty::printMenu"<<endl);
    it.print(out);}
  template<Roles role=Roles::Raw,bool toPrint=true>
  static inline void printItem(It& it,Out& out,Idx=0,bool=false,bool=true,Mode=Mode::Normal) {
    trace(MDO<<"Nav::printItem"<<endl);
    it.template print<Out,role,toPrint>(out);
  }
};
