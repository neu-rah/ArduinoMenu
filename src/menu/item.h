/* -*- C++ -*- */
#pragma once

#include "api.h"

template<const char** text,typename I=Empty<>>
struct StaticText:I,lambda::StaticText<text> {
  using This=StaticText<text,I>;
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    O::raw(text[0]);
    I::template printTo<O,P,N>();
  }
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu() {
  //   // It::template printTo<Out,Nav>();
  //   I::template printMenu<It,Out,Nav>();
  // }
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu() {Out::template printMenu<This,Out,Nav>(This());}
};

// using StaticText=lambda::Curry<_StaticText,2>;//using this invalidates the default argument!

template<typename O>
struct SubMenu:O {
  // template<typename Nav> inline static bool enter(Nav& nav) {
  //   _trace(MDO<<"SubMenu::enter(nav)"<<endl);
  //   nav.open();
  //   return true;
  //   // return nav._enter();
  // }
  template<typename Nav>
  inline static bool enter(Nav& nav,Ref,idx_t) {
    _trace(MDO<<"SubMenu::enter(nav,ref,idx_t)"<<endl);
    nav.open();
    return nav._enter();}
  template<typename Nav>
  inline static bool enter(Nav& nav) {
    _trace(MDO<<"SubMenu::enter(inav)"<<endl);
    return nav._enter();}
};

template<typename I,typename... II>
struct StaticMenuData:StaticMenuData<I> {
  using This=StaticMenuData<I,II...>;
  using Head=I;
  using Tail=StaticMenuData<II...>;
  inline void enable(idx_t n,bool o) {
    if (n) enable(n-1,o);
    else I::enable(n,o);
  }
  inline bool enabled(idx_t n) const {
    return n?Tail::enabled(n-1):I::enabled(0);
  }
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printItem(size_t i) {
    i?Tail::template printItem<O,P,N>(i-1):I::template printTo<O,P,N>();
  }
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu() {
  //   _trace(MDO<<"StaticMenuData<I,II>::printMenu"<<endl);
  //   Out::template printMenu<This,Out,Nav>(This());
  // }
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline void printMenu(Nav& nav,Ref ref,idx_t n) {
  //   _trace(
  //     MDO<<"StaticMenuData<I,II>::printMenu"<<endl
  //       <<"maxDepth:"<<nav.maxDepth()<<endl
  //       <<"level:"<<nav.level<<endl
  //       <<"ref.len:"<<ref.len<<endl
  //       <<"ref.head():"<<ref.head()<<endl
  //       <<"pos():"<<nav.pos()<<endl
  //       <<"n:"<<n<<endl
  //   );
  //   if (n) Tail::template printMenu<Out,Nav>(nav,ref,n-1);
  //   else if (ref.len)
  //     I::template printMenu<I,Out,Nav>(nav,ref.tail(),ref.head());
  //     else Out::template printMenu<This,Out,Nav>(*this,nav,Out());
  // }
  inline static constexpr idx_t size() {return Tail::size()+1;}
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  inline static void fmtItem(idx_t at,idx_t n,const Nav& nav,Out& out) {
    if (at) Tail::template fmtItem<r,io,It,Out,Nav>(at-1,n,nav,out);
    else Out::template fmt<r,io,It,Out,Nav>(n,nav,out);
  }
  template<typename Nav> inline static bool enter(Nav& nav,Ref ref,idx_t n) {
    trace(
      MDO <<endl<<"StaicMenuData<I,II>::enter"<<endl
          <<"maxDepth:"<<nav.maxDepth()<<endl
          <<"level:"<<nav.level<<endl
          <<"ref.len:"<<ref.len<<endl
          <<"ref.head():"<<ref.head()<<endl
          <<"pos():"<<nav.pos()<<endl
          <<"n:"<<n<<endl
    );
    if (n) return Tail::template enter<Nav>(nav,ref,n-1);
    else {
      _trace(MDO<<"=>");
      I::template printTo<MDD>();
      return I::template enter<Nav>(nav);
    }
    // return n?
    //   Tail::template enter<Nav>(nav,ref,n-1):
    //   I::template enter<Nav>(nav);
  }
};

template<typename I>
struct StaticMenuData<I>:I {
  using This=StaticMenuData<I>;
  template<typename O,Roles P,typename N=Drift<>>
  inline static void printTo() {}
  template<typename O,Roles P,typename N=Drift<>>
  inline static void printItem(size_t i) {I::template printTo<O,P,N>();}
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu(Nav& nav,Ref ref,idx_t n) {
  //   _trace(MDO<<"StaticMenuData<I>::printMenu"<<endl);
  //   Out::template printMenu<This,Out,Nav>(nav);
  // }
  inline static constexpr idx_t size() {return 1;}
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  inline static void fmtItem(idx_t at,idx_t n,const Nav& nav,Out& out) {
    Out::template fmt<r,io,It,Out,Nav>(n,nav,out);
  }
  // template<typename Nav> inline static bool enter(Nav& nav,idx_t n) {
  //   return n?false:I::template enter<Nav>(nav);
  // }
  template<typename Nav> inline static bool enter(Nav& nav,Ref ref,idx_t n) {
    trace(MDO<<"StaticMenuData<I>::enter"<<endl);
    return n?
      false:
      I::template enter<Nav>(nav,ref.tail(),ref.head());
  }
};

template<typename Title,typename Data>
struct StaticMenu: Data {
  using This=StaticMenu<Title,Data>;
  template<typename O,Roles P,typename N=Drift<>>
  inline static void printTo() {Title::template printTo<O,P,N>();}
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu(Nav& nav,Ref ref,idx_t n) {
  //   _trace(MDO<<"StaticMenu::printMenu"<<endl);
  //   Out::template printMenu<It,Nav>(This(),nav);
  // }
};

//this is the top level of static items
template<typename I>
struct Item:I {
  // template<typename Out,typename Nav=Drift<>>
  // inline void printMenu(Nav& nav) {
  //   _trace(MDO<<"Item::printMenu"<<endl);
  //   if (nav.level) printMenu<Out,Nav>(nav,nav.operator Ref());
  //   else I::template printMenu<I,Out,Nav>(*this,nav,Out());
  // }
  // template<typename Out,typename Nav=Drift<>>
  // inline static void printMenu(Nav& nav,Ref ref) {
  //   printMenu<Out,Nav>(nav,ref,ref.len?ref.head():nav.pos());
  // }
  //
  // template<typename Out,typename Nav=Drift<>>
  // inline static void printMenu(Nav& nav,Ref ref,idx_t n) {
  //   I::template printMenu<I,Out,Nav>(nav,ref,ref.len?ref.head():nav.pos());
  // }
  //
  template<typename Nav>
  inline static bool enter(Nav& nav) {return enter<Nav>(nav,nav.operator Ref());}
  template<typename Nav>
  inline static bool enter(Nav& nav,Ref ref) {return enter<Nav>(nav,ref,ref.len?ref.head():nav.pos());}
  template<typename Nav>
  inline static bool enter(Nav& nav,Ref ref,idx_t n) {
    trace(
      MDO<<endl<<"Item::enter"<<endl
        <<"maxDepth:"<<nav.maxDepth()<<endl
        <<"level:"<<nav.level<<endl
        <<"ref.len:"<<ref.len<<endl
        <<"ref.head():"<<ref.head()<<endl
        <<"pos():"<<nav.pos()<<endl
        <<"n:"<<n<<endl
    );
    return I::template enter<Nav>(nav,ref,n);
  }
};
