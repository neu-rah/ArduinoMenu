#pragma once

// #include "sys/kernel/base.h"

namespace Menu {
  template<typename N=Nil>
  struct Drift:N {
    Drift(){}
    Drift(const Drift&)=delete;
    Drift& operator=(const Drift&)=delete;
    template<typename Out>
    static inline void printTo(Out& out) {}
    template<typename In>
    inline bool doInput(In& in) {return in.cmd(N::obj());}
    template<Cmd c> inline static bool cmd() {return false;}
    template<typename Dev> inline static void poll(Dev&) {}
  };

  template<typename Data,Idx max_depth=8>
  struct StaticNav {
    template<typename O=Drift<>>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      using MenuType=Data;
      Data data;
      Idx path[max_depth]{0};
      Idx level=0;
      bool _editing=false;
      bool _tunning=false;
      bool wasEdited=false;
      bool edited() const {return wasEdited;}
      void edited(bool e) {wasEdited=e;}
      // Part(Data& data):data(data) {}
      Part(const Part&)=delete;
      Part& operator=(const Part&)=delete;
      template<typename Dev>
      inline void poll(Dev& dev) {root().poll(dev);}
      bool focus() {return _editing;}
      void focus(bool f) {_editing=f;}
      inline void begin() {data.begin();}
      inline Data& root() {return data;}
      void relax() {if(_editing) PathRef(level,path).walk<Data,Relax>(data);}
      template<Idx o,Idx oo,Idx... ooo>
      void go(Idx n=0) {path[n]=o;go<oo,ooo...>(n+1);}
      template<Idx o>
      void go(Idx n=0) {quitEdit();path[n]=o;level=n;}
      void goRoot() {while(level) esc();}
      void quitEdit() {focus(false);}
      bool tune() const {return _tunning;}
      void tune(bool t) {_tunning=t;}
      Idx* at() {return &path[level];}
      Idx pos() const {return path[level];}
      void pos(Idx at) {path[level]=at;}
      //get position of item or parent when parent printing
      Idx ppos() {
        auto s=PathRef(level,path);
        Idx l=level?(has<Style::ParentDraw>(s.walk<Data,Styles>(data))?level-1:level):0;
        return path[l];
      }
      template<typename Out>
      inline void printTo(Out& out) {
        out.newView();
        Style s=PathRef(level,path).walk<Data,Styles>(data);
        Idx l=level?(has<Style::ParentDraw>(s)?level-1:level):0;
        PathRef(l,path).walk<Data,PrintMenuTo,This&,Out&>(data,*this,out,path[l]);
      }
      using Base::cmd;
      template<Cmd c>
      inline bool cmd(int code=0) {
        //TODO: check changes...
        auto ref=PathRef(level+(focus()?1:0),path);
        bool r=ref.template walk<Data,CmdTo<c>,This&,int>(data,*this,code);
        //enter/esc result reflect open/close will, but from now on it must reflect change->redraw
        //up/down wil alway refer change->draw
        return r||c==Cmd::Enter||c==Cmd::Esc;
      }
      inline bool up() {return cmd<Cmd::Up>();}
      inline bool down() {return cmd<Cmd::Down>();}
      inline bool enter() {return cmd<Cmd::Enter>();}
      inline bool esc() {return cmd<Cmd::Esc>();}
      template<Cmd c>
      bool doNav(Idx len,Style s) {
        switch(c) {
          case Cmd::Up: {
            if(pos()<(len-1)) {
              pos(pos()+1);
              return true;
            } else if(has<Style::WrapNav>(s)) {
              pos(0);
              return true;
            }
            return false;
          }
          case Cmd::Down: 
            if(pos()>0) {
              pos(pos()-1);
              return true;
            } else if(has<Style::WrapNav>(s)) {
              pos(len-1);
              return true;
            }
            return false;
          case Cmd::Enter: {
            auto ref=PathRef(level+1,path);
            bool r=ref.template walk<Data,Enter,typename This::Type&>(data,This::obj());
            bool m=has<Style::IsMenu>(s);
            bool x=r^m;
            // r m x
            // 0 0 0 close
            // 0 1 1 -
            // 1 0 1 -
            // 1 1 0 open
            return x?
              (has<Style::IsField>(s)?(focus(true),true):true)
              :r?open():close();
          }; break;
          case Cmd::Esc: return close();
          default: return false;
        }
        return true;
      }
      bool childEnabled() {return PathRef(level+1,path).template walk<Data,Enabled>(data);}
      Style styles() {return PathRef(level,path).template walk<Data,Styles>(data);}
      bool open(Idx i=0) {
        if(level>=max_depth||!childEnabled()) return false;
        level++;//enter new level
        path[level]=i;//pos. at fst item
        return true;
      }
      bool close() {
        if(focus()) {
          focus(false);
          return true;
        } else if(level) {
          level--;
          return true;
        }
        return true;
      }
    };
  };

  class INav {
    public:
      virtual bool focus() {return false;}
      virtual bool tune() {return false;}
  };

  // class Nav {
  //   public:
  //     template<typename N>
  //     struct Part:INav,N {};
  // };

  template<template<typename> class Base,template<typename> class... I>
  using NavDefBase=Composition<Base,I...,Drift>;

};//Menu namespace