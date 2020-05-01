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

      using vector<IItem*>::size;

      template<typename A,Idx i>
      inline IdMatch<A> _walkId(A& api) {
        for(auto o:*this)
          if (o->id(i)) return api.template call<This>(Base::obj());
          else {
            auto r=_walkId<A,i>(api);
            if(r.match) return r;
          }
        return IdMatch<A>();
      }
      // template<typename A>
      // APIRes walkPath(A& api,PathRef ref) {
      //   auto a=IAPICall<A>(api);
      //   return ref?
      //     vector<IItem*>::operator[](ref.head())->template walkPath<A>(a,ref.tail()):
      //     api.template call<This>(Base::obj());
      // }

      APIRes walkPath(IAPI& api,PathRef ref) {
        trace(MDO<<"StdVectorMenu::walkPath "<<api.named()<<" @"<<ref<<endl);
        return api.chk(Base::obj(),ref)?
          vector<IItem*>::operator[](ref.head())->walkPath(api,ref.tail()):
          api.call(Base::obj());
      }

      using Base::enabled;
      inline bool enabled(PathRef ref) {
        auto f=APICall::Enabled();
        return walkPath(f,ref);
      }
      using Base::enable;
      inline void enable(bool b,PathRef ref) {
        auto en=APICall::Enable<true>();
        auto dis=APICall::Enable<false>();
        if(b) walkPath(en,ref);
        else walkPath(dis,ref);
      }

      using Base::parentPrint;
      inline bool parentPrint(Idx n) {
        return vector<IItem*>::operator[](n)->parentPrint();
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out) {
        trace(MDO<<"StdVectorMenu::printMenu"<<endl);
        out.template printMenu<typename I::Type,Nav,op>
          (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()));
      }
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0) {
        trace(MDO<<"StaticMenu::printItems"<<endl);
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

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out) {
        trace(MDO<<"StdVectorMenu::print "<<role<<endl);
        // if(title.changed())
        title.template print<Nav,Out,op>(nav,out);
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

      inline ActRes activate() {return ActRes::Open;}
    };
  };
};
