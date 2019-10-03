/* -*- C++ -*- */
#pragma once
#include "api.h"

template<const char** text,typename I=Empty<>>
struct StaticText:I {
  template<typename Out> inline static void print() {Out::raw(text[0]);}
  template<typename Out> inline static void print(Out& out) {out.raw(text[0]);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav,Out& out) {print(out);}
};

template<typename I,typename... II>
struct StaticData:StaticData<I> {
  using This=StaticData<I,II...>;
  using Head=I;
  using Tail=StaticData<II...>;
  inline static constexpr Idx size() {return Tail::size()+1;}

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Out& out,Idx n) {
    trace(MDO<<"StaticData::printItem<I,II...> "<<n<<endl);
    if (!out.freeY()) return;
    StaticData<I>::template printItem<I,Nav,Out>(*this,nav,out,n);
    reinterpret_cast<Tail*>(this)->Tail::template printItem<Tail,Nav,Out>(*reinterpret_cast<Tail*>(this),nav,out,n+1);
    // Tail::template printItem<Tail,Nav,Out>(it,nav,out,n+1);
  }
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav,Out& out) {
    trace(MDO<<"StaticData::printItems<I,II...>"<<endl);
    out.posTop(nav);

    out.template fmt<P,true,It,Out,Nav>(0,nav,out);
    printItem<This,Nav,Out>(*this,nav,out,out.top());
    out.template fmt<P,false,It,Out,Nav>(0,nav,out);
  }
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(MDO<<"StaticData::printMenu<I,II...> "<<n<<endl);
    if (n) printMenu<It,Nav,Out>(it,nav,out,ref,n-1);
    else if (ref.len) {
      _trace(
        MDO<<"path walk... head:"
          <<ref.head()
          <<" len:"<<ref.len
          <<" n:"<<n
          <<endl);
      reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,out,ref.tail(),ref.head());
    } else {
      _trace(MDO<<"self printMenu<I,II...> out"<<endl);
      out.template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,out,/*ref,*/n);
    }
  }
};

template<typename I>
struct StaticData<I>:I {
  using This=StaticData<I>;
  inline static constexpr Idx size() {return 1;}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Out& out,Idx n) {
    trace(MDO<<"StaticData::printItem<I> "<<n<<endl);
    out.clrLine(out.posY());
    it.template fmt<Roles::Prompt,true ,It,Out,Nav>(n,nav,out);
    it.template fmt<Roles::Index, true ,It,Out,Nav>(n,nav,out);
    it.template fmt<Roles::Index, false,It,Out,Nav>(n,nav,out);
    it.template fmt<Roles::Cursor,true ,It,Out,Nav>(n,nav,out);
    it.template fmt<Roles::Cursor,false ,It,Out,Nav>(n,nav,out);
    reinterpret_cast<I*>(this)->template print<I,Nav,Out,Roles::Prompt>(*reinterpret_cast<I*>(this),nav,out/*,n*/);
    // reinterpret_cast<I*>(this)->template print<Out>(out);
    it.template fmt<Roles::Prompt,false,It,Out,Nav>(n,nav,out);
  }
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav,Out& out) {
    trace(MDO<<"StaticData::printItems<I>"<<endl);
    out.posTop(nav);

    it.template fmt<P,true,It,Out,Nav>(0,nav,out);
    printItem<This,Nav,Out>(*this,nav,out,out.top());
    it.template fmt<P,false,It,Out,Nav>(0,nav,out);
  }

  // template<typename It,typename Nav,typename Out>
  // inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
  //   trace(MDO<<"StaticData::printMenu<I>"<<endl);
  //   if(n) return;
  //   else if(ref.len)
  // }
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(MDO<<"StaticData::printMenu<I>"<<endl);
    if (n) return;//printMenu<It,Nav,Out>(it,nav,out,ref,n-1);
    else if (ref.len) {
      _trace(MDO<<"path walk..."<<endl);
      reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,out,ref.tail(),ref.head());
    } else {
      trace(MDO<<"self printMenu<I> out"<<endl);
      out.template printMenu<This,Nav,Out>(*this,nav,out,ref,n);
    }
  }
};

template<typename T,typename B>
struct StaticMenu:B {
  using This=StaticMenu<T,B>;
  using Title=T;
  using Body=B;
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav,Out& out/*,Idx n*/) {
    Title::print(out);
  }
  using B::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(MDO<<"StaticMenu::printMenu"<<endl);
    if (n) {
      _trace(MDO<<"scan "<<n<<endl);
      printMenu<It,Nav,Out>(it,nav,out,ref,n-1);
    } else if (ref.len) {
      _trace(MDO<<"path walk..."<<endl);
      B::template printMenu<It,Nav,Out>(it,nav,out,ref.tail(),ref.head());
    } else {
      _trace(MDO<<"self print"<<endl);
      out.template printMenu<This,Nav,Out>(*this,nav,out,n);
    }
  }
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    trace(MDO<<"StaticMenu::"<<c<<endl);
    if (n) cmd<c,It,Nav>(it,nav,ref,n-1);
    else if (ref.len) cmd<c,It,Nav>(it,nav,ref.tail(),ref.head());
    else {
      trace(MDO<<"StaticMenu::"<<c<<"!!!"<<endl);
      if (c==Cmds::Enter) nav.open();
      B::template cmd<c,It,Nav>(it,nav);
      return true;
    }
  }
};
