/* -*- C++ -*- */
#pragma once
#include "api.h"

template<const char** text,typename I=Empty<>>
struct StaticText:I {
  template<typename Out> inline static void print() {Out::raw(text[0]);}
  template<typename Out> inline static void print(Out& out) {out.raw(text[0]);}

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It&,Nav&,Out& out,Ref,Idx) {print(out);}
};

template<typename I,typename... II>
struct StaticData:I {
  using This=StaticData<I,II...>;
  using Head=I;
  using Tail=StaticData<II...>;
  inline static constexpr Idx size() {return Tail::size()+1;}

  template<typename It,typename Nav>
  inline bool enter(It&,Nav&,Ref ref,Idx n) {}

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void printItem(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(
      MDO<<"StaticData::printItem<I,II>"
        <<" ref.len:"<<ref.len
        <<" n:"<<n<<endl
    );
    if (n) Tail::template printItem<It,Nav,Out,P>(it,nav,out,ref,n-1);
    else I::template print<It,Nav,Out,P>(it,nav,out,ref,n);
  }

  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(
      MDO<<"StaticData::printMenu<I,II>"
        <<" ref.len:"<<ref.len
        <<" n:"<<n<<endl
    );
    if (n) Tail::template printMenu<It,Nav,Out>(it,nav,out,ref,n-1);
    else if (ref.len) I::template printMenu<It,Nav,Out>(it,nav,out,ref.tail(),ref.head());
    else out.printMenu(it,nav,out,ref,n);
  }
};

template<typename I>
struct StaticData<I>:I {
  using This=StaticData<I>;
  inline static constexpr Idx size() {return 1;}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void printItem(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(
      MDO<<"StaticData::printItem<I>"
        <<" ref.len:"<<ref.len
        <<" n:"<<n<<endl
    );
    // if (n) return;
    // else if (ref.len) I::template printMenu<It,Nav,Out>(it,nav,out,ref.tail(),ref.head());
    // else it.print(it,nav,out,ref,n);
  }
};

template<typename T,typename B>
struct StaticMenu:B {
  using Title=T;
  using Body=B;
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(
      MDO<<"StaticMenu::print"
        <<" ref.len:"<<ref.len
        <<" n:"<<n<<endl
    );
    if (P==Roles::Title) Title::print(out);
    else Body::template print<It,Nav,Out>(it,nav,out,ref,n);
  }
  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(
      MDO<<"StaticMenu::printMenu"
        <<" ref.len:"<<ref.len
        <<" n:"<<n<<endl
    );
    Body::template printMenu<It,Nav,Out>(it,nav,out,ref,n);
  }
  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav,Out& out) {
    _trace(MDO<<"StaticMenu::printMenu."<<endl);
    out.printMenu(it,nav,out);
  }
};
