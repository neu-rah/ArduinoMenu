/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename I,Idx max_depth>
struct Nav {
  template<typename N>
  struct Part:N {
    using This=Nav<I,max_depth>::Part<N>;
    using Data=I;
    Idx level=0;
    Idx path[max_depth]{0};
    Data& root;
    Modes editMode;
    inline Part(Data& o):root(o){}
    template<typename Out>
    inline void print(Out& out) {
      _trace(MDO<<"Nav::print"<<endl);
      root.printMenu(N::obj(),out,*this);}
    inline Idx pos() const {return path[level];}
    inline Modes mode() const {return editMode;}
    inline void setMode(Modes m) {editMode=m;}
    inline bool selected(Idx i) const {return path[level]==i;}
    inline void setPos(Idx n) {path[level]=n;}
    inline Idx head() const {return path[0];}
    inline operator PathRef() const {return (PathRef){level,path};}
    inline PathRef parent() const {return operator PathRef().parent();}
    inline void open() {
      assert(level<max_depth-1);
      if(!root.isMenu(*this,head())) setMode(Modes::Edit);
      path[++level]=0;
    }
    inline void close() {
      // assert(level>0);
      if(level>0) {
        path[level--]=0;
        setMode(Modes::Normal);
      }
    }
    // inline void up() {root.template cmd<typename N::Type,Cmd::Up>();}
    // inline void down() {root.template cmd<typename N::Type,Cmd::Down>();}
    // inline void cmd(Cmd c) {
    //   switch(cmd) {
    //     case Cmd::None: break;
    //     case Cmd::Up: up();break;
    //     case Cmd::Down: down();break;
    //   }
    // }
    inline size_t size() const {return root.size(*this);}
    inline void _up() {
      _trace(MDO<<"pos:"<<pos()<<" size:"<<size()<<endl);
      if(pos()+1<size()) setPos(pos()+1);}
    inline void _down() {if(pos()>0) setPos(pos()-1);}
    inline void _enter() {
      trace(MDO<<"enter->sending activate"<<endl);
      bool n=root.canNav(*this,This::head());
      bool r=root.activate(*this,head());
      trace(MDO<<"canNav:"<<n<<" activated:"<<r<<endl);
      if (!(n^r)) n?open():close();
    }
    inline void _esc() {close();}
    template<Cmd c>
    inline void _cmd() {
      switch(c) {
        case Cmd::Up:_up();break;
        case Cmd::Down:_down();break;
        case Cmd::Enter:_enter();break;
        case Cmd::Esc:_esc();break;
        default:break;
      }
    }

  };
};

template<Expr... I>
struct StaticNavRoot:Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>> {
  using Base=typename Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>>;
  using This=StaticNavRoot<I...>;
  using Base::Base;
};

struct INav {
  virtual inline void print(IOut& out)=0;
  virtual inline Idx pos() const=0;
  virtual inline Modes mode() const=0;
  virtual inline bool selected(Idx i) const =0;
};

template<Expr... N>
struct NavRoot:INav,Chain<N...,Drift>::template To<Obj<NavRoot<N...>>> {
  using Base=typename Chain<N...,Drift>::template To<Obj<NavRoot<N...>>>;
  using This=NavRoot<N...>;
  using Base::Base;
  inline void print(IOut& out) override {Base::print(out);}
  inline Idx pos() const override {return Base::pos();}
  inline Modes mode() const override {return Base::mode();}
  inline bool selected(Idx i) const override {return Base::selected(i);}
};
