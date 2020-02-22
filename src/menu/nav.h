/* -*- C++ -*- */
#pragma once

#include "staticNav.h"

namespace Menu {
  struct INav {
    virtual inline void print(IOut& out)=0;
    virtual inline Idx pos() const=0;
    virtual inline Mode mode() const=0;
    virtual inline bool selected(Idx i) const =0;
    virtual inline bool _cmd(Cmd)=0;

    template<Cmd c>
    inline bool _cmd() {return _cmd(c);}
  };

  template<Expr... N>
  struct NavRoot:INav,Chain<N...,Drift>::template To<Obj<NavRoot<N...>>> {
    using Base=typename Chain<N...,Drift>::template To<Obj<NavRoot<N...>>>;
    using This=NavRoot<N...>;
    using Base::Base;
    using Base::_cmd;
    inline void print(IOut& out) override {Base::print(out);}
    inline Idx pos() const override {return Base::pos();}
    inline Mode mode() const override {return Base::mode();}
    inline bool selected(Idx i) const override {return Base::selected(i);}
    bool _cmd(Cmd c) override {
      switch(c) {
        case Cmd::None: return Base::template _cmd<Cmd::None>();break;
        case Cmd::Enter: return Base::template _cmd<Cmd::Enter>();break;
        case Cmd::Esc: return Base::template _cmd<Cmd::Esc>();break;
        case Cmd::Up: return Base::template _cmd<Cmd::Up>();break;
        case Cmd::Down: return Base::template _cmd<Cmd::Down>();break;
        case Cmd::Left: return Base::template _cmd<Cmd::Left>();break;
        case Cmd::Right: return Base::template _cmd<Cmd::Right>();break;
      }
      assert(false);
    }
  };
};
