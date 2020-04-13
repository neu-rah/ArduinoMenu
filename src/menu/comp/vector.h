/* -*- C++ -*- */
#pragma once

#include <staticMenu.h>
#include <vector>

namespace Menu {
  template<typename Title,Title& title>
  struct StdVectorMenu {
    template<typename I>
    struct Part:I,vector<IItem*> {
      using vector<IItem*>::vector;
      using This=StdVectorMenu<Title,title>::Part<I>;
      using Base=I;

      // inline void changed(bool o) const {return title.changed(o);}

      inline bool enabled(PathRef ref=self) const {
        trace(MDO<<"StdVectorMenu::enabled "<<ref<<endl);
        return ref?
          vector<IItem*>::operator[](ref.head())->enabled(ref.tail()):
          Base::enabled();
      }
      inline void enable(bool b,PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::enable "<<b<<" "<<ref<<endl);
        if (ref) vector<IItem*>::operator[](ref.head())->enable(b,ref.tail());
        else Base::enable(b);
      }

      inline bool canNav(PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::canNav "<<ref<<endl);
        if (ref) return vector<IItem*>::operator[](ref.head())->canNav(ref.tail());
        return true;
      }

      inline size_t size(PathRef ref=self) const {
        trace(MDO<<"StdVectorMenu::size "<<ref<<endl);
        return ref?vector<IItem*>::operator[](ref.head())->size(ref.tail()):vector<IItem*>::size();
      }

      inline ActRes activate(PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::activate "<<ref<<endl);
        if(ref.len==1)
          return operator[](ref.head())->enabled()?operator[](ref.head())->activate():ActRes::Stay;
        return ref?operator[](ref.head())->activate(ref.tail()):ActRes::Open;
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
        trace(MDO<<"StdVectorMenu::printMenu "<<op<<" @"<<ref<<endl);
        if (!ref||(ref.len==1&&(vector<IItem*>::operator[](ref.head())->parentPrint(ref.tail()))))
          out.template printMenu
            <typename I::Type,Nav,op>
            (I::obj(),nav,!(out.partialDraw()&&out.isSame(&Base::obj())));
        else vector<IItem*>::operator[](ref.head())->template printMenu<Nav,Out,op>(nav,out,ref.tail());
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0,PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::printItems fullPrint:"<<fullPrint<<" top:"<<top<<" freeY:"<<out.freeY()<<endl);
        if(ref) vector<IItem*>::operator[](ref.head())->printItems(nav,out,idx,top,ref.tail());
        else {
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
                (*a,nav,idx++,nav.selected(idx),a->enabled(),nav.mode());
            } else idx++;
          }
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printTitle(Nav& nav,Out& out,bool fullPrint) {
        trace(MDO<<"StdVectorMenu::printTitle "<<op<<" fullDraw:"<<out.fullDraw()<<" changed:"<<title.changed()<<" out:"<<((long)&out)<<endl);
        if (op==Op::ClearChanges) title.changed(false);
        else {
          if (op==Op::Printing&&(fullPrint||out.fullDraw()||title.changed()))
          out.template printTitle<typename I::Type,Nav,op>(I::obj(),nav);
        else
          out.template printTitle<typename I::Type,Nav,op==Op::Printing?Op::Measure:op>(I::obj(),nav);
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::print "<<role<<endl);
        // if(title.changed())
        title.template print<Nav,Out,op>(nav,out,ref);
      }

      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::cmd "<<ref<<endl);
        if(ref.len==1&&!has(c,Cmd::Enter|Cmd::Esc)) {
          Idx p=nav.pos();
          bool res=vector<IItem*>::operator[](ref.head())->template cmd<c,Nav>(nav);
          if(p!=nav.pos()) {
            vector<IItem*>::operator[](p)->changed(true);
            vector<IItem*>::operator[](nav.pos())->changed(true);
          }
          return res;
        }
        return ref?
          vector<IItem*>::operator[](ref.head())->template cmd<c,Nav>(nav,ref.tail()):
          Base::template cmd<c,Nav>(nav);
      }

    };
  };
};
