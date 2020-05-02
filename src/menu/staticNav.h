/* -*- C++ -*- */
#pragma once

#include "api.h"

namespace Menu {

  template<typename I>
  struct StaticNavBase {
    template<typename N>
    struct Part:N {
      using Data=I;
      Data& root;
      inline Part(Data& o):root(o){}
    };
  };

  template<typename I>
  struct IdNav {
    template<typename N>
    struct Part:StaticNavBase<I>::template Part<N> {
      using Base=typename StaticNavBase<I>::template Part<N>;
      using Data=I;
      inline Part(Data& o):Base(o){}

      template<typename A>
      inline APIRes walkId(A& api,Idx i) {
        trace(MDO<<"IdNav::walkId<"<<api.name<<"> "<<i<<endl;);
        return Base::root.walkId(api,i);
      }

      template<typename A,Idx i>
      APIRes walkId(A& api) {
        trace(MDO<<"IdNav::walkId<"<<api.name<<","<<i<<">"<<endl;);
        return Base::root.template walkId<A,i>(api);
      }

      template<typename Out,Idx i>
      inline void print(Out& out) {
        trace(MDO<<"IdNav::print"<<endl);
        auto printMenu=typename APICall::template PrintMenu<typename N::Type,Out,Op::Printing>(N::obj(),out);
        Base::root.template walkId<decltype(printMenu),i>(printMenu);
      }
      template<Cmd c,Idx id>
      inline bool cmd() {
        trace(MDO<<"IdNav::cmd<"<<c<<","<<id<<">"<<endl);
        auto api=typename APICall::Cmd<c,typename N::Type,id>(N::obj());
        return Base::root.walkId(api);
      }
      template<Cmd c> inline bool cmd(Idx id) {
        trace(MDO<<"IdNav::cmd<(Cmd)"<<c<<">"<<endl);
        auto api=typename APICall::Cmd<c,typename N::Type>(N::obj());
        return Base::root.walkId(api,id);
      }
    };
  };

  template<typename I,Idx max_depth>
  struct Nav {
    template<typename N>
    struct Part:IdNav<I>::template Part<N> {
      using This=Nav<I,max_depth>::Part<N>;
      using Base=typename IdNav<I>::template Part<N>;
      using Data=I;
      Idx level=0;
      Idx path[max_depth]{0};
      // Data& root;
      Mode editMode;
      inline Part(Data& o):Base(o){}

      template<typename Out>
      inline void printItem(Out& out) {
        trace(MDO<<"Nav::printItem"<<endl);
        Base::root.template print<typename N::Type,Out,Op::Printing>(N::obj(),out,*this);
      }
      template<typename Out>
      inline void print(Out& out) {
        trace(MDO<<"Nav::print"<<endl);
        auto printMenu=typename APICall::template PrintMenu<typename N::Type,Out,Op::Printing>(N::obj(),out);
        Base::root.template walkPath<decltype(printMenu)>(printMenu,operator PathRef());
        if (out.partialDraw()) {
          trace(MDO<<"Nav::print cleanup!"<<endl);
          auto clear=typename APICall::template PrintMenu<typename N::Type,Out,Op::ClearChanges>(N::obj(),out);
          Base::root.walkPath(clear,N::obj());//sometimes we need INav here... what now
        }
      }
      inline Idx pos() const {return path[level];}
      inline Mode mode() const {return editMode;}
      inline void setMode(Mode m) {editMode=m;}
      inline bool selected(Idx i) const {return path[level]==i;}
      inline void setPos(Idx n) {path[level]=n;}
      inline Idx head() const {return path[0];}
      inline operator PathRef() const {return PathRef{(Idx)(level+1),path};}
      inline PathRef parent() const {return operator PathRef().parent();}
      inline void open() {
        assert(level<max_depth-1);
        path[++level]=0;
      }
      inline void close() {if(level>0) path[level--]=0;
      }
      inline size_t size() const {return Base::root.size(*this);}
      inline size_t size(PathRef ref) const {
        auto sizeCall=APICall::Size();
        return Base::root.walkPath(sizeCall,N::obj());
      }

      using Base::cmd;
      template<Cmd c>
      inline bool cmd() {
        trace(MDO<<"Nav::cmd "<<c<<" path:"<<((PathRef)*this)<<endl);
        auto api=typename APICall::Cmd<c,typename N::Type>(N::obj());
        // return Base::root.template cmd<c,typename N::Type>(N::obj(),*this);
        return Base::root.walkPath(api,N::obj());
      }

      inline void up()    {cmd<Cmd::Up>();}
      inline void down()  {cmd<Cmd::Down>();}
      inline void enter() {cmd<Cmd::Enter>();}
      inline void esc()   {cmd<Cmd::Esc>();}

      template<Cmd c>
      inline bool _cmd() {
        switch(c) {
          case Cmd::Up:_up();break;
          case Cmd::Down:_down();break;
          case Cmd::Enter:_enter();break;
          case Cmd::Esc:_esc();break;
          default:return false;
        }
        return true;
      }

      inline void _up() {
        trace(MDO<<"pos:"<<pos()<<" size:"<<size(parent())<<endl);
        if(((size_t)pos()+1)<size()) setPos(pos()+1);}

      inline void _down() {if(pos()>0) setPos(pos()-1);}

      inline void _enter() {
        trace(MDO<<"enter->sending activate "<<(PathRef)*this<<endl);
        auto actCall=APICall::Activate();
        ActRes r=Base::root.walkPath(actCall,N::obj());
        if(r==ActRes::Open) open();
        else if(r==ActRes::Close) close();
      }

      inline void _esc() {close();}

    };
  };

  template<Expr... I>
  struct StaticNavRoot:Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>> {
    using Base=typename Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>>;
    using This=StaticNavRoot<I...>;
    using Base::Base;
  };
};
