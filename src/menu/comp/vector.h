/* -*- C++ -*- */
#pragma once

#include <staticMenu.h>
#include <vector>

namespace Menu {
  template<typename Title,Title& title>
  struct StdVectorMenu {
    template<typename I>
    struct Part:SetWalker::template Part<I>,vector<IItem*> {
      using vector<IItem*>::vector;
      using This=Part<I>;
      using Base=typename SetWalker::template Part<I>;

      using vector<IItem*>::size;
      inline size_t sizeItem(Idx n) const {
        trace(MDO<<"StdVectorMenu::size of "<<n<<endl);
        return vector<IItem*>::operator[](n)->size();
      }

      // template<typename A,Idx i>
      // inline IdMatch<A> _walkId(A& api) {
      //   for(auto o:*this)
      //     if (o->id(i)) return api.template call<This>(Base::obj());
      //     else {
      //       auto r=_walkId<A,i>(api);
      //       if(r.match) return r;
      //     }
      //   return IdMatch<A>();
      // }
      using Base::walkPath;
      template<typename A>
      APIRes walkPath(const A& api,PathRef ref,Idx level,Idx n) {
        trace(MDO<<"StdVectorMenu::walkPath"<<endl);
        return api.step(*vector<IItem*>::operator[](n),ref,level);
      }

      using Base::enable;
      inline void enable(bool b,Idx n) {
        trace(MDO<<"StdVectorMenu::enable "<<b<<" @"<<n<<endl);
        vector<IItem*>::operator[](n)->enable(b);
      }

      using Base::enabled;
      inline bool enabledItem(Idx n) const {
        trace(MDO<<"StdVectorMenu::enabledItem @"<<n<<endl);
        return vector<IItem*>::operator[](n)->enabled();
      }

      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux) {
        Idx p=nav.pos();
        bool res=Base::template cmd<c,Nav>(nav,level,aux);
        if(p!=nav.pos()) {
          Base::changed(p,true);
          Base::changed(nav.pos(),true);
        }
        return res;
      }

      template<Cmd c,typename Nav>
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        return vector<IItem*>::operator[](n)->template cmdItem<c,Nav>(nav,level,aux,n);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,Idx level,Idx=0) {
        trace(MDO<<"StdVectorMenu::printMenu"<<endl);
        out.template printMenu<typename I::Type,Nav,op>
          (Base::obj(),nav,level,out.fullDraw()||!out.isSame(&Base::obj()));
      }
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printItems(Nav& nav,Out& out,Idx level,bool fullPrint,Idx idx=0,Idx top=0) {
        trace(MDO<<"StdVectorMenu::printItems"<<endl);
        for(auto a:*this) {
          if (!out.freeY()) return;
          if(top) {
            trace(MDO<<"-----"<<endl);
            top--;idx++;continue;
          }
          trace(MDO<<"item "<<idx<<" changed "<<a->changed()<<endl);
          if (op==Op::Printing&&(fullPrint||out.fullDraw()||a->changed())) {
            out.template printItem
              <decltype(*a),Nav,op>
              (*a,nav,level,idx++,nav.selected(idx),a->enabled(),nav.mode());
          } else idx++;
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level) {
        trace(MDO<<"StdVectorMenu::print "<<role<<endl);
        // if(title.changed())
        title.template print<Nav,Out,op>(nav,out,level);
        if (delegate) I::template print<Nav,Out,op>(nav,out,level);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printTitle(Nav& nav,Out& out,Idx level,bool fullPrint) {
        trace(MDO<<"StdVectorMenu::printTitle "<<op<<" fullDraw:"<<out.fullDraw()<<" changed:"<<title.changed()<<" out:"<<((long)&out)<<endl);
        if (op==Op::ClearChanges) title.changed(false);
        else {
          if (op==Op::Printing&&(fullPrint||out.fullDraw()||title.changed()))
          out.template printTitle<typename I::Type,Nav,op>(I::obj(),nav,level);
        else
          out.template printTitle<typename I::Type,Nav,op==Op::Printing?Op::Measure:op>(I::obj(),nav,level);
        }
      }

      template<typename Nav>
      inline void activate(Nav& nav,Idx) {
        trace(MDO<<"StdVectorMenu::activate!"<<endl);
        nav.open();
      }
      template<typename Nav>
      inline void activate(Nav& nav,Idx level,Idx n) {
        trace(MDO<<"StdVectorMenu::activate n:"<<n<<endl);
        vector<IItem*>::operator[](n)->template activate<Nav>(nav,level);
      }
    };
  };
};
