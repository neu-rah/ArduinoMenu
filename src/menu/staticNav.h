/* -*- C++ -*- */
#pragma once

#include "api.h"

namespace Menu {

  template<typename I>
  struct IdNav {
    template<typename N>
    struct Part:N {
      using Data=I;
      Data& root;
      inline Part(Data& o):root(o){}
      template<typename Out,Idx i>
      inline void printItem(Out& out) {
        root.template print<typename N::Type,Out,Op::Printing,i>(N::obj(),out);
      }
      template<typename Out,Idx i>
      inline void print(Out& out) {
        trace(MDO<<"Nav::print"<<endl);
        root.template printMenu<typename N::Type,Out,Op::Printing,i>(N::obj(),out);
        if (out.partialDraw()) {
          trace(MDO<<"Nav::print cleanup!"<<endl);
          root.template printMenu<typename N::Type,Out,Op::ClearChanges,i>(N::obj(),out);
        }
      }
    };
  };

  template<typename I,Idx max_depth>
  struct Nav {
    template<typename N>
    struct Part:N {
      using This=Nav<I,max_depth>::Part<N>;
      using Data=I;
      Idx level=0;
      Idx path[max_depth]{0};
      Data& root;
      Mode editMode;
      inline Part(Data& o):root(o){}
      template<typename Out>
      inline void printItem(Out& out) {
        trace(MDO<<"Nav::printItem"<<endl);
        root.template print<typename N::Type,Out,Op::Printing>(N::obj(),out,*this);
      }
      template<typename Out>
      inline void print(Out& out) {
        trace(MDO<<"Nav::print"<<endl);
        auto printMenu=typename API::template PrintMenu<typename N::Type,Out,Op::Printing>(N::obj(),out);
        root.walkPath(printMenu,operator PathRef());
        // root.template printMenu<typename N::Type,Out,Op::Printing>(N::obj(),out,*this);
        if (out.partialDraw()) {
          trace(MDO<<"Nav::print cleanup!"<<endl);
          auto clear=typename API::template PrintMenu<typename N::Type,Out,Op::ClearChanges>(N::obj(),out);
          root.walkPath(clear,*this);
          // root.template printMenu<typename N::Type,Out,Op::ClearChanges>(N::obj(),out,*this);
        }
      }
      // template<typename Out>
      // inline void printParent(Out& out) {
      //   trace(MDO<<"Nav::printParent"<<endl);
      //   root.printMenu(N::obj(),out,parent());}
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
      inline size_t size() const {return root.size(*this);}
      inline size_t size(PathRef ref) const {
        auto sizeCall=API::Size();
        return root.walkPath(sizeCall,*this);
      }

      template<Cmd c>
      inline bool cmd() {
        _trace(MDO<<"Nav::cmd "<<c<<" path:"<<((PathRef)*this)<<endl);
        auto api=typename API::Cmd<c,This>(*this);
        // return root.template cmd<c,typename N::Type>(N::obj(),*this);
        return root.walkPath(api,*this);
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
        _trace(MDO<<"pos:"<<pos()<<" size:"<<size(parent())<<endl);
        if(((size_t)pos()+1)<size(parent())) setPos(pos()+1);}

      inline void _down() {if(pos()>0) setPos(pos()-1);}

      inline void _enter() {
        trace(MDO<<"enter->sending activate "<<(PathRef)*this<<endl);
        // bool n=root.canNav(*this);//TODO: check this on activate! => can not, we double check it
        auto actCall=API::Activate();
        ActRes r=root.walkPath(actCall,*this);
        // ActRes r=root.activate(*this);
        // trace(MDO<<"canNav:"<<n<<" activated:"<<r<<endl);
        // trace(MDO<<"!(n^r):"<<(!(n^r))<<endl);
        if(r==ActRes::Open) open();
        else if(r==ActRes::Close) close();
        // if (!(n^r)) n?open():close();
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
