/* -*- C++ -*- */
#pragma once

#include <menu.h>
#include <vector>

namespace Menu {
  template<typename Title,Title& title>
  struct StdVectorMenu {
    template<typename I>
    struct Part:I,vector<IItem*> {
      using vector<IItem*>::vector;
      using This=StdVectorMenu<Title,title>::Part<I>;
      using Base=I;

      inline void changed(bool o) {return title.changed(o);}

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

      inline bool activate(PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::activate "<<ref<<endl);
        return ref?vector<IItem*>::operator[](ref.head())->activate(ref.tail()):true;
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
        trace(MDO<<"StdVectorMenu::printMenu "<<op<<endl);
        if (!ref||ref.len==1&&(vector<IItem*>::operator[](ref.head())->parentPrint(ref.tail())))
          out.template printMenu<typename I::Type,Nav,op>(I::obj(),nav);
        else vector<IItem*>::operator[](ref.head())->template printMenu<Nav,Out,op>(nav,out,ref.tail());
      }

      template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
      inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self,bool fullPrint=true) {
        trace(MDO<<"StdVectorMenu::printItems fullPrint:"<<fullPrint<<" top:"<<top<<" freeY:"<<out.freeY()<<endl);
        if(ref) vector<IItem*>::operator[](ref.head())->printItems(nav,out,idx,top,ref.tail(),fullPrint);
        else {
          for(auto a:*this) {
            if (!out.freeY()) return;
            if(top) {
              trace(MDO<<"-----"<<endl);
              top--;idx++;continue;
            }
            trace(MDO<<"item "<<idx<<" changed "<<a->changed()<<endl);
            if (fullPrint||a->changed()||!out.partialDraw()) {
              trace(MDO<<"StdVectorMenu::printItems changed:"<<a->changed()<<" partialDraw:"<<out.partialDraw()<<endl);
              out.template printItem
                <decltype(*a),Nav,op,op==Op::Printing>
                (*a,nav,idx++,nav.selected(idx),a->enabled(),nav.mode());
            } else idx++;
          }
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        trace(MDO<<"StdVectorMenu::print "<<role<<endl);
        if(title.changed()) title.template print<Nav,Out,op,role>(nav,out,ref);
      }

      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,PathRef ref=self) {
        if (ref) return vector<IItem*>::operator[](ref.head())->template cmd<c,Nav>(nav,ref.tail());
        else {
          Idx p=nav.pos();
          bool res=Base::template cmd<c,Nav>(nav);
          if(p!=nav.pos()) {
            trace(MDO<<"changed "<<p<<"&"<<+nav.pos()<<endl);
            operator[](p)->changed(true);
            operator[](nav.pos())->changed(true);
            trace(MDO<<"<======>"<<endl);
          }
          return res;
        }
      }

    };
  };
};
