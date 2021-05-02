/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief std container menu
*/

#include <utility>
#include "menu/item.h"

namespace Menu {
  template<typename Data>
  struct StdMenu {
    template<typename O>
    struct Part:O {
      Data data;
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      template<typename ...T>
      Part(T... t):data{ std::forward<T>(t)... } {}
      decltype(data.begin()) begin() { return data.begin(); }
      decltype(data.end()) end() { return data.end(); }
      decltype(data.size()) size() const { return data.size(); }
      static constexpr Style styles() {return Style::IsMenu|Base::styles();}
      Idx sz() const {return size();}
      decltype(data.operator[](0)) operator[](size_t i) {return data.operator[](i); }

      static constexpr bool canNav() {return true;}
      static constexpr bool parentDraw() {return false;}

      template<typename Target,Idx o,Idx... oo>
      struct Walk {
        Target& target;
        Walk(Target& t):target(t) {}
        template<typename API,typename... Args>
        auto walk(Args... args) 
          ->decltype(
            Walk<IItem,oo...>(*target[0]).template walk<API,Args...>(args...)
          ){
          return Walk<IItem,oo...>(*target[o]).template walk<API,Args...>(args...);
        }
      };

      template<typename Target,Idx o>
      struct Walk<Target,o> {
        Target& target;
        Walk(Target& t):target(t) {}
        template<typename API,typename... Args>
        auto walk(Args... args) 
          ->decltype(
            API().operator()(*(target[o]),args...)
          ){
          return API().operator()(*(target[o]),args...);
        }
      };

      struct RefWalk {
        Idx pathLen;
        const Idx* path;
        RefWalk(Idx l,Idx* p):pathLen(l),path(p) {}
        RefWalk(const RefWalk& p):pathLen(p.len()),path(p.path) {}
        Idx len() const {return pathLen;}
        Idx head() {return path[0];}
        RefWalk tail() {return RefWalk(len()-1,(Idx*)&path[1]);}
        template<typename Target,typename API,typename... Args>
        auto step(Idx at,Target& target,Args... args) 
          ->decltype(API().operator()(*target[0],args...))
          {
          return len()?
            RefWalk(pathLen-1,(Idx*)&path[1])
              .template step<decltype(*target[at]),API,Args...>(head(),*target[at],args...):
            API().operator()(*target[at],args...);
        }
      };

      template<typename Nav> static bool enter(Nav&) {return true;}

      // using Base::cmdTo;
      // NavRes cmdTo(Cmd c,Idx l,Idx *path) {
      //   switch(c) {
      //     case Cmd::Enter:return PathRef(l,path).walk<This,CmdTo<Cmd::Enter>>(*this);
      //     case Cmd::Esc:return PathRef(l,path).walk<This,CmdTo<Cmd::Esc>>(*this);
      //     case Cmd::Up:return PathRef(l,path).walk<This,CmdTo<Cmd::Up>>(*this);
      //     case Cmd::Down:return PathRef(l,path).walk<This,CmdTo<Cmd::Down>>(*this);
      //     default: assert(false);
      //   }
      // }
      template<typename Nav,typename Out,int... path>
      void printMenuTo(Nav& nav,Out& out,int sel) {
        Walk<This,path...>(*this).template walk<PrintMenuTo,Out,int>(out,sel);
      }

      template<typename Nav,typename Out>
      void printMenuTo(Nav& nav,Out& out,int sel) {
        out.printMenu(nav,*this,sel);
      }

      template<typename Nav,typename Out> void onPrintTitleTo(Nav& nav,Out& out) {Base::onPrintTitleTo(nav,out);}
      template<typename Nav,typename Out> void onPrintBodyTo(Nav& nav,Out& out,Idx selIdx,Idx n) {
        // Base::printTo(out);
        int idx=0;
        for(auto i:data) {
          out.printItem(nav,*i,idx,idx==selIdx);
          idx++;
        }
      }
      template<typename Nav,typename Out> void printItemTo(Nav& nav,Out& out) {out.printItem(Base::obj());}
    };
  };

};//Menu namespace