/* -*- C++ -*- */
#pragma once

#include "sys/kernel/api.h"

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

      template<typename Out,Idx i>
      inline void print(Out& out) {
        trace(MDO<<"IdNav::print"<<endl);
        APICall::template PrintMenu
          <typename N::Type,Out,Op::Printing>
          (N::obj(),out).template walkId<Data,i>(Base::root);
      }
      template<Cmd c,Idx id>
      inline bool cmd() {
        trace(MDO<<"IdNav::cmd<"<<c<<","<<id<<">"<<endl);
        auto api=typename APICall::Cmd<c,typename N::Type,id>(N::obj());
        return Base::root.walkId(api,0);
      }
      template<Cmd c> inline bool cmd(Idx id) {
        trace(MDO<<"IdNav::cmd<(Cmd)"<<c<<">"<<endl);
        auto api=typename APICall::Cmd<c,typename N::Type>(N::obj());
        return Base::root.walkId(api,0,id);
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
      Mode editMode;
      inline Part(Data& o):Base(o){}

      template<typename Out>
      inline void printItem(Out& out) {
        trace(MDO<<"Nav::printItem"<<endl);
        Base::root.template print<typename N::Type,Out,Op::Printing>(N::obj(),out,*this);
      }
      template<typename Out>
      void print(Out& out,PathRef ref) {
        trace(MDO<<"Nav::print ref:"<<ref<<endl);
        APICall::template PrintMenu<typename N::Type,Out,Op::Printing>(N::obj(),out).walkPath(Base::root,ref,0);
        if (out.partialDraw()) {
          trace(MDO<<"Nav::print cleanup!"<<endl);
          APICall::template PrintMenu<typename N::Type,Out,Op::ClearChanges>(N::obj(),out).walkPath(Base::root,ref,0);
        }
      }
      template<typename Out>
      inline void print(Out& out) {print(out,operator PathRef());}
      inline Idx pos(Idx l) const {return path[l];}
      inline Idx pos() const {return pos(level);}
      inline Mode mode() const {return editMode;}
      inline void setMode(Mode m) {editMode=m;}
      // inline Idx focus() const {return path[level];}
      // inline Idx focus(Idx l) const {return path[l];}
      inline bool selected(Idx i) const {return path[level]==i;}
      inline bool selected(Idx i,Idx l) const {return path[l]==i;}
      inline void setPos(Idx n) {setPos(n,level);}
      inline void setPos(Idx n,Idx l) {path[l]=n;}
      inline Idx head() const {return path[0];}
      inline operator PathRef() const {return PathRef{(Idx)(level+1),path};}
      inline PathRef parent() const {return operator PathRef().parent();}
      inline void open() {
        assert(level<max_depth-1);
        path[++level]=0;
      }
      inline void close() {if(level>0) path[level--]=0;}
      inline size_t size() const {return Base::root.size(operator PathRef());}
      inline size_t size(PathRef ref) const {
        trace(MDO<<"Nav::size "<<ref<<endl);
        return Base::root.size(ref);
      }

      using Base::cmd;
      template<Cmd c>
      inline bool cmd(Idx aux=0) {
        trace(MDO<<"Nav::cmd "<<c<<" path:"<<((PathRef)*this)<<endl);
        //need to send esc event so that states get changed
        /*if(c==Cmd::Esc) {
          close();
          return true;
        } else*/
        return APICall::Cmd<c,typename N::Type>(N::obj(),aux).walkPath(Base::root,*this,0);
      }

      inline void up()    {cmd<Cmd::Up>();}
      inline void down()  {cmd<Cmd::Down>();}
      inline void enter() {cmd<Cmd::Enter>();}
      inline void esc()   {cmd<Cmd::Esc>();}
      inline void idx(Idx n) {cmd<Cmd::Index>(n);}
      inline void accel(Idx n) {cmd<Cmd::Accel>(n);}

      // template<Cmd c>
      // inline bool _cmd(Idx n=0) {
      //   trace(MDO<<"StaticNav::_cmd "<<c<<endl);
      //   switch(c) {
      //     case Cmd::Up:_up();break;
      //     case Cmd::Down:_down();break;
      //     case Cmd::Enter:_enter();break;
      //     case Cmd::Esc:_esc();break;
      //     case Cmd::Index:_idx(n);break;
      //     case Cmd::Accel:_accel(n);break;
      //     default:return false;
      //   }
      //   return true;
      // }

      inline Idx depth() const {return level;}
      // inline bool focus(Idx n) {return n<size()?(setPath(level,n),true):false;}
      // inline void setPath(Idx l,Idx n) {path[l]=n;}

      // inline void _idx(Idx n) {
      //   trace(MDO<<"nav index:"<<n<<endl);
      //   if(n) {
      //     if(focus(n)) enter();
      //   } else close();
      // }

      // inline void _accel(Idx n) {
      //   trace(MDO<<"nav index:"<<n<<endl);
      // }

      // inline void _up(/*Idx sz,bool wrap=false*/) {
      //   trace(MDO<<"pos:"<<pos()<<" size:"<<size(parent())<<endl);
      //   if(((size_t)pos()+1)<size(parent())) setPos(pos()+1);
      //   // if(pos()<sz-1) setPos(pos()+1);
      // }
      //
      // inline void _down(bool wrap=false) {if(pos()>0) setPos(pos()-1);}
      //
      // inline void _enter() {
      //   trace(MDO<<"enter->sending activate "<<(PathRef)*this<<endl);
      //   activate();
      // }

      inline void activate(Idx n) {
        path[level]=n;
        activate();
      }
      inline void activate() {
        APICall::template Activate
          <typename Base::Type>
          (Base::obj())
          .walkPath(Base::root,operator PathRef(),0);
      }

      // inline void _esc() {close();}

    };
  };

  template<Expr... I>
  struct StaticNavRoot:Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>> {
    using Base=typename Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>>;
    using This=StaticNavRoot<I...>;
    using Base::Base;
  };
};
