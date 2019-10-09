/* -*- C++ -*- */
#pragma once
#include <lpp.h>
#include "api.h"

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

/**
* The Action class associates an actikon function with a menu item.
*/
template<typename I,ActionHandler act>
struct Action:I {
  using I::I;
  using This=Action<I,act>;
  using I::cmd;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav) {return act();}
};

template<const char** text,typename I=Empty<>>
struct StaticText:I {
  using I::size;
  template<typename Out> inline static void print() {Out::raw(text[0]);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav) {print<Out>();}
};

enum FName {StaticPrint};
template<FName f>
using FuncId=lambda::StaticValue<FName,f>;

template<typename I,typename... II>
struct StaticData:lambda::List<lambda::As<I>,lambda::As<II>...> {
  using This=StaticData<I,II...>;
  template<Idx n,Idx... nn>
  struct Agent {
    using Type=typename lambda::Expr<lambda::Index,This,lpp::N<n>>::App::Type::template Agent<nn...>;
  };
  template<Idx n> struct Agent<n> {
    using Type=typename lambda::Expr<lambda::Index,This,lpp::N<n>>::App::Type;
    template<typename Out>
    inline void print() {I::template print<Out>();}
  };
  template<Idx n,Idx... nn>
  inline Agent<n,nn...> agent() {return Agent<n,nn...>();}
  template<Idx... ii>
  using TypeOf=typename Agent<ii...>::Type;
  // template<size_t n>
  // constexpr This::TypeOf<n>* item() const {return this;}
  // constexpr static auto item()->This::TypeOf<n>* {return this;}
};

// template<typename I,typename... II>
// struct StaticData:StaticData<I> {
//   using This=StaticData<I,II...>;
//   using Head=I;
//   using Base=StaticData<I>;
//   using Tail=StaticData<II...>;
//   using Base::size;
//   template<Idx n,Idx... nn>
//   struct TypeOf {
//     using App=typename lpp::Index<lpp::List<I,II...>,lpp::N<n>>;//::Type::template TypeOf<nn...>::Type;
//   };
//   template<Idx n> struct TypeOf<n> {
//     using App=typename lpp::Index<lpp::List<I,II...>,lpp::N<n>>;
//   };
//   inline static constexpr Idx size() {return Tail::size()+1;}
//   inline static constexpr Idx size(Ref ref) {
//     return ref.len?size(ref,ref.head()):size();
//   }
//   inline static constexpr Idx size(Ref ref,Idx n) {
//     return
//       n?Tail::size(ref,n-1):
//         ref.len?
//           I::size(ref.tail(),ref.tail().head()):
//           size();
//   }
//
//   template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
//   inline void printItem(It& it,Nav& nav,Idx n,Idx p=0) {
//     if (p) reinterpret_cast<Tail*>(this)->Tail::template printItem<It,Nav,Out,P>(it,nav,n,p-1);
//     if (!Out::freeY()) return;
//     StaticData<I>::template printItem<I,Nav,Out>(*this,nav,n);
//     reinterpret_cast<Tail*>(this)->Tail::template printItem<It,Nav,Out>(it,nav,n+1);
//   }
//
//   template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
//   inline void printItems(It& it,Nav& nav) {
//     Out::posTop(nav);
//     if (!Out::freeY()) return;
//     Out::clrLine(Out::posY());
//     Out::template fmt<P,true,It,Out,Nav>(0,nav);
//     printItem<This,Nav,Out>(*this,nav,Out::top(),Out::top());
//     Out::template fmt<P,false,It,Out,Nav>(0,nav);
//   }
//
//   template<typename It,typename Nav,typename Out>
//   inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
//     if (n)
//       reinterpret_cast<Tail*>(this)->Tail::template printMenu<It,Nav,Out>(it,nav,ref,n-1);
//     else if (ref.len) reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
//     else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
//   }
//
//   using I::cmd;
//   template<Cmds c,typename It,typename Nav>
//   inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
//     assert(c!=Cmds::Esc);
//     if (n) {
//       return reinterpret_cast<Tail*>(this)->Tail::template cmd<c,It,Nav>(it,nav,ref,n-1);
//     } else if (ref.len) {
//       return reinterpret_cast<I*>(this)->I::template cmd<c,It,Nav>(it,nav,ref.tail(),nav.pos());
//     } else {
//       assert(c!=Cmds::Esc);
//       if (c==Cmds::Enter) {
//         //TODO: check enabled!
//         return I::template cmd<c,It,Nav>(it,nav);
//       }
//       Empty<>::template cmd<c,It,Nav>(it,nav);
//       return true;
//     }
//   }
// };
//
// template<typename I>
// struct StaticData<I>:I {
//   using This=StaticData<I>;
//   // template<Idx n,Idx... nn>
//   // struct TypeOf {
//   //   using Type=typename lpp::Index<lpp::List<I>,lpp::N<n>>;//::Type::template TypeOf<nn...>::Type;
//   // };
//   inline static constexpr Idx size() {return 1;}
//   inline static constexpr Idx size(Ref ref) {
//     return ref.len?size(ref,ref.head()):size();
//   }
//   inline static constexpr Idx size(Ref ref,Idx n) {
//     return
//       n?0:
//         ref.len?
//           I::size(ref.tail(),ref.head()):
//           I::size()+1;
//   }
//
//   template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
//   inline void printItem(It& it,Nav& nav,Idx n,Idx p=0) {
//     if (!Out::freeY()) return;
//     Out::clrLine(Out::posY());
//     it.template fmt<Roles::Prompt,true ,It,Out,Nav>(n,nav);
//     it.template fmt<Roles::Index, true ,It,Out,Nav>(n,nav);
//     it.template fmt<Roles::Index, false,It,Out,Nav>(n,nav);
//     it.template fmt<Roles::Cursor,true ,It,Out,Nav>(n,nav);
//     it.template fmt<Roles::Cursor,false ,It,Out,Nav>(n,nav);
//     reinterpret_cast<I*>(this)->template print<I,Nav,Out,Roles::Prompt>(*reinterpret_cast<I*>(this),nav);
//     it.template fmt<Roles::Prompt,false,It,Out,Nav>(n,nav);
//   }
//   template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
//   inline void printItems(It& it,Nav& nav) {
//     Out::posTop(nav);
//     it.template fmt<P,true,It,Out,Nav>(0,nav);
//     printItem<This,Nav,Out>(*this,nav,Out::top(),Out::top());
//     it.template fmt<P,false,It,Out,Nav>(0,nav);
//   }
//
//   template<typename It,typename Nav,typename Out>
//   inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
//     if (n) return;
//     else if (ref.len) reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
//     else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
//   }
//   template<Cmds c,typename It,typename Nav>
//   inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
//     assert(c!=Cmds::Esc);
//     if (c==Cmds::Enter) {
//       //TODO: check enabled!
//       return I::template cmd<c,It,Nav>(it,nav);
//     }
//     Empty<>::template cmd<c,It,Nav>(it,nav);
//     return true;
//     // assert(c!=Cmds::Esc);
//     // if (n) return false;
//     // return I::template cmd<c,It,Nav>(it,nav);
//   }
// };

template<typename T,typename B>
struct StaticMenu:B {
  using This=StaticMenu<T,B>;
  using Title=T;
  using Body=B;
  template<typename Out> inline static void print() {T::template print<Out>();}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav) {
    Title::template print<Out>();
  }
  // using B::cmd;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav) {
    assert(c!=Cmds::Esc);
    nav.open();
    return true;
  }

  // using B::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    if (ref.len) B::template printMenu<It,Nav,Out>(it,nav,ref,n);
    else Out::template printMenu<This,Nav,Out>(*this,nav);
  }
};
