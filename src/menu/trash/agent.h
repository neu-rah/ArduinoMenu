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
  virtual ActRes activate()=0;
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
  virtual void print(Out& out,Tag role=Tag::Raw,bool toPrint=true) {
    switch(role) {
      case Tag::Raw: toPrint?agent().template print<Tag::Raw,true>(out):agent().template print<Tag::Raw,false>(out);
      case Tag::Panel: toPrint?agent().template print<Tag::Panel,true>(out):agent().template print<Tag::Panel,false>(out);
      case Tag::Menu: toPrint?agent().template print<Tag::Menu,true>(out):agent().template print<Tag::Menu,false>(out);
      case Tag::Title: toPrint?agent().template print<Tag::Title,true>(out):agent().template print<Tag::Title,false>(out);
      case Tag::Body: toPrint?agent().template print<Tag::Body,true>(out):agent().template print<Tag::Body,false>(out);
      case Tag::Item: toPrint?agent().template print<Tag::Item,true>(out):agent().template print<Tag::Item,false>(out);
      case Tag::Index: toPrint?agent().template print<Tag::Index,true>(out):agent().template print<Tag::Index,false>(out);
      case Tag::Cursor: toPrint?agent().template print<Tag::Cursor,true>(out):agent().template print<Tag::Cursor,false>(out);
      case Tag::Name: toPrint?agent().template print<Tag::Name,true>(out):agent().template print<Tag::Name,false>(out);
      case Tag::Mode: toPrint?agent().template print<Tag::Mode,true>(out):agent().template print<Tag::Mode,false>(out);
      case Tag::Value: toPrint?agent().template print<Tag::Value,true>(out):agent().template print<Tag::Value,false>(out);
      case Tag::Unit: toPrint?agent().template print<Tag::Unit,true>(out):agent().template print<Tag::Unit,false>(out);
    }
  }
  virtual void printMenu(bool) {
    trace(MDO<<"PrintAgent::printMenu"<<endl);
    agent().print(out);}
  static inline void printMenu(bool,It& it,Nav&,Out& out,Ref,Idx) {
    trace(MDO<<"Empty::printMenu"<<endl);
    it.print(out);}
  template<Tag role=Tag::Raw,bool toPrint=true>
  static inline void printItem(It& it,Out& out,Idx=0,bool=false,bool=true,Mode=Mode::Normal) {
    trace(MDO<<"Nav::printItem"<<endl);
    it.template print<Out,role,toPrint>(out);
  }
};
