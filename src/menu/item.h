/* -*- C++ -*- */
#pragma once
#include "api.h"

template<const char** text,typename I=Empty<>>
struct StaticText:I {
  using I::size;
  template<typename Out> inline static void print() {Out::raw(text[0]);}
  // template<typename Out> inline static void print(Out& out) {Out::raw(text[0]);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav) {print<Out>();}
};

template<typename I,typename... II>
struct StaticData:StaticData<I> {
  using This=StaticData<I,II...>;
  using Head=I;
  using Base=StaticData<I>;
  using Tail=StaticData<II...>;
  using Base::size;
  inline static constexpr Idx size() {return Tail::size()+1;}
  inline static constexpr Idx size(Ref ref) {
    return ref.len?size(ref,ref.head()):size();
  }
  inline static constexpr Idx size(Ref ref,Idx n) {
    return
      n?Tail::size(ref,n-1):
        ref.len?
          I::size(ref.tail(),ref.tail().head()):
          size();
  }

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Idx n) {
    trace(MDO<<"StaticData::printItem<I,II...> "<<n<<endl);
    if (!Out::freeY()) return;
    StaticData<I>::template printItem<I,Nav,Out>(*this,nav,n);
    reinterpret_cast<Tail*>(this)->Tail::template printItem<It,Nav,Out>(it,nav,n+1);
    // Tail::template printItem<Tail,Nav,Out>(it,nav,out,n+1);
  }
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav) {
    trace(MDO<<"StaticData::printItems<I,II...>"<<endl);
    Out::posTop(nav);

    Out::template fmt<P,true,It,Out,Nav>(0,nav);
    printItem<This,Nav,Out>(*this,nav,Out::top());
    Out::template fmt<P,false,It,Out,Nav>(0,nav);
  }
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    trace(MDO<<"StaticData::printMenu<I,II...> "<<n<<endl);
    if (n)
      // reinterpret_cast<Tail*>(this)->Tail::template printMenu<Tail,Nav,Out>(*reinterpret_cast<Tail*>(this),nav,ref,n-1);
      reinterpret_cast<Tail*>(this)->Tail::template printMenu<It,Nav,Out>(it,nav,ref,n-1);
    else if (ref.len) {
      trace(
        MDO<<"path walk... head:"
          <<ref.head()
          <<" len:"<<ref.len
          <<" n:"<<n
          <<endl);
      reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
    } else {
      trace(MDO<<"self printMenu<I,II...> out"<<endl);
      Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
    }
  }
  using I::cmd;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    _trace(MDO<<"StaticData<I,II...>::"<<c<<" #"<<n<<" ref.len:"<<ref.len<<endl);
    if (n) {
      _trace(MDO<<"StaticData<I,II...>::"<<c<<" walk..."<<n<<endl);
      // return reinterpret_cast<Tail*>(this)->Tail::template cmd<c,Tail,Nav>(*reinterpret_cast<Tail*>(this),nav,ref,n-1);
      return reinterpret_cast<Tail*>(this)->Tail::template cmd<c,It,Nav>(it,nav,ref,n-1);
    } else if (ref.len) {
      _trace(MDO<<"StaticData<I,II...>::"<<c<<" step... "<<ref.head()<<endl);
      return
        reinterpret_cast<I*>(this)->I::template cmd<c,It,Nav>
          (it,nav,ref.tail(),ref.head());
    } else {
      _trace(MDO<<"StaticData<I,II...>::"<<c<<" run..."<<n<<endl);
      if (c==Cmds::Enter) {
        _trace(
          MDO<<"open()..."
            <<"head:"<<ref.head()
            <<" len:"<<ref.len
            <<" n:"<<n
            <<endl);
        nav.open();
      }
      I::template cmd<c,It,Nav>(it,nav);
      return true;
    }
  }
};

template<typename I>
struct StaticData<I>:I {
  using This=StaticData<I>;
  inline static constexpr Idx size() {return 1;}
  inline static constexpr Idx size(Ref ref) {
    return ref.len?size(ref,ref.head()):size();
  }
  inline static constexpr Idx size(Ref ref,Idx n) {
    return
      n?0:
        ref.len?
          I::size(ref.tail(),ref.head()):
          I::size()+1;
  }

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Idx n) {
    trace(MDO<<"StaticData::printItem<I> "<<n<<endl);
    Out::clrLine(Out::posY());
    it.template fmt<Roles::Prompt,true ,It,Out,Nav>(n,nav);
    it.template fmt<Roles::Index, true ,It,Out,Nav>(n,nav);
    it.template fmt<Roles::Index, false,It,Out,Nav>(n,nav);
    it.template fmt<Roles::Cursor,true ,It,Out,Nav>(n,nav);
    it.template fmt<Roles::Cursor,false ,It,Out,Nav>(n,nav);
    reinterpret_cast<I*>(this)->template print<I,Nav,Out,Roles::Prompt>(*reinterpret_cast<I*>(this),nav);
    // reinterpret_cast<I*>(this)->template print<Out>(out);
    it.template fmt<Roles::Prompt,false,It,Out,Nav>(n,nav);
  }
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav) {
    trace(MDO<<"StaticData::printItems<I>"<<endl);
    Out::posTop(nav);

    it.template fmt<P,true,It,Out,Nav>(0,nav);
    printItem<This,Nav,Out>(*this,nav,Out::top());
    it.template fmt<P,false,It,Out,Nav>(0,nav);
  }

  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    trace(MDO<<"StaticData::printMenu<I>"<<endl);
    if (n) return;//printMenu<It,Nav,Out>(it,nav,out,ref,n-1);
    else if (ref.len) {
      trace(MDO<<"path walk..."<<endl);
      reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
    } else {
      trace(MDO<<"self printMenu<I> out"<<endl);
      Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
    }
  }
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    _trace(MDO<<"StaticData<I>::"<<c<<" #"<<n<<endl);
    if (n) return false;
    // if (ref.len)
    //   return reinterpret_cast<I*>(this)->I::template cmd<c,I,Nav>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
    // else
      // return reinterpret_cast<I*>(this)->I::template cmd<c,I,Nav>(*reinterpret_cast<I*>(this),nav);
    I::template cmd<c,It,Nav>(it,nav);
  }
};

template<typename T,typename B>
struct StaticMenu:B {
  using This=StaticMenu<T,B>;
  using Title=T;
  using Body=B;
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav) {
    Title::template print<Out>();
  }
  using B::cmd;
  using B::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    trace(MDO<<"StaticMenu::printMenu "<<B::size()<<endl);
    if (ref.len) B::template printMenu<It,Nav,Out>(it,nav,ref,n);
    else Out::template printMenu<This,Nav,Out>(*this,nav);
  }
  // template<Cmds c,typename It,typename Nav>
  // inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
  //   _trace(MDO<<"StaticMenu::"<<c<<" #"<<n<<endl);
  //   if (n) {
  //     cmd<c,It,Nav>(it,nav,ref,n-1);
  //   } else if (ref.len) cmd<c,This,Nav>(*this,nav,ref.tail(),ref.head());
  //   else {
  //     _trace(MDO<<"StaticMenu::"<<c<<"!!!"<<endl);
  //     if (c==Cmds::Enter) {
  //       _trace(
  //         MDO<<"open()..."
  //           <<"head:"<<ref.head()
  //           <<" len:"<<ref.len
  //           <<" n:"<<n
  //           <<endl);
  //       nav.open();
  //     }
  //     reinterpret_cast<B*>(this)->B::template cmd<c,B,Nav>(*reinterpret_cast<B*>(this),nav);
  //     return true;
  //   }
  // }
};
