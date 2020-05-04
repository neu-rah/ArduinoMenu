/* -*- C++ -*- */
#pragma once

#include "staticNav.h"

namespace Menu {
  struct INav {
    virtual void print(IOut& out)=0;
    virtual Idx pos() const=0;
    virtual Mode mode() const=0;
    virtual bool selected(Idx i) const =0;
    virtual bool cmd(Cmd,Idx=0)=0;
    virtual bool _cmd(Cmd,Idx=0)=0;
    virtual void setMode(Mode m)=0;


    template<Cmd c> inline bool _cmd(Idx n=0) {return _cmd(c,n);}
    template<typename In> inline bool doInput(In& in) {return in.cmd(*this);}
    template<Cmd c> inline bool cmd() {return cmd(c);}
};

  template<Expr... N>
  struct NavRoot:INav,Chain<N...,Drift>::template To<Obj<NavRoot<N...>>> {
    using Base=typename Chain<N...,Drift>::template To<Obj<NavRoot<N...>>>;
    using This=NavRoot<N...>;
    using Base::Base;
    using Base::_cmd;
    using INav::cmd;
    using INav::doInput;
    inline void print(IOut& out) override {Base::print(out);}
    inline Idx pos() const override {return Base::pos();}
    inline Mode mode() const override {return Base::mode();}
    inline bool selected(Idx i) const override {return Base::selected(i);}
    inline bool cmd(Cmd c,Idx n=0) override {
      switch(c) {
        case Cmd::None: return Base::template cmd<Cmd::None>(n);break;
        case Cmd::Enter: return Base::template cmd<Cmd::Enter>(n);break;
        case Cmd::Esc: return Base::template cmd<Cmd::Esc>(n);break;
        case Cmd::Up: return Base::template cmd<Cmd::Up>(n);break;
        case Cmd::Down: return Base::template cmd<Cmd::Down>(n);break;
        case Cmd::Left: return Base::template cmd<Cmd::Left>(n);break;
        case Cmd::Right: return Base::template cmd<Cmd::Right>(n);break;
      }
      assert(false);
    }
    bool _cmd(Cmd c,Idx n=0) override {
      switch(c) {
        case Cmd::None: return Base::template _cmd<Cmd::None>(n);break;
        case Cmd::Enter: return Base::template _cmd<Cmd::Enter>(n);break;
        case Cmd::Esc: return Base::template _cmd<Cmd::Esc>(n);break;
        case Cmd::Up: return Base::template _cmd<Cmd::Up>(n);break;
        case Cmd::Down: return Base::template _cmd<Cmd::Down>(n);break;
        case Cmd::Left: return Base::template _cmd<Cmd::Left>(n);break;
        case Cmd::Right: return Base::template _cmd<Cmd::Right>(n);break;
      }
      assert(false);
    }
    void setMode(Mode m) override {Base::setMode(m);}
  };
};
