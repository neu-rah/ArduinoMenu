/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// some utilities and base definitions

#ifdef ARDUINO
  #include "arduino_assert.h"
#endif

namespace AM5 {

  struct Nil {};

  //type equality
  //from: https://stackoverflow.com/a/16924234/1329075
  // template<typename T, typename U> struct is_same {
  //   static inline constexpr bool value() {return false;}
  // };
  // template<typename T> struct is_same<T, T>{
  //   static inline constexpr bool value() {return true;}
  // };
  // template<typename T, typename U> bool eqTypes() { return is_same<T, U>::value(); }
  //
  // A<X> != A<Y> (as expected and as it should)
  // we should extend this to eqTemplates
  // however they must be disassembled to do so
  // then we could use eq<A,B> the we could use types A<O>,B<O>
  // .... not easy, we have too many variants

  //type nexting from a list of templates+defered terminal type
  #define Expr template<typename> class
  #define Term typename

  template<Expr... OO>
  struct Chain {
    template<Term _T,Expr _O, Expr... _OO>
    struct Links:public _O<Links<_T,_OO...>> {using _O<Links<_T,_OO...>>::_O;};

    template<Term _T,Expr _O>
    struct Links<_T,_O>:public _O<_T> {using _O<_T>::_O;};

    template<Term T>
    struct To:public Links<T,OO...> {};
    //provide a continuation
    template<Expr _O>
    using With=Chain<OO...,_O>;
  };

  #undef Expr
  #undef Term

  using idx_t = int;//size_t sucks for scrolling

  template<typename O>
  struct ID:public O {using O::O;};

  #define MENU_INJECT_PARTS false
  struct Item;
  template<typename> struct Prompt;
  struct MenuOut;
  #if (MENU_INJECT_PARTS==true)
    struct PrinterPart;
  #endif

  //////////////////////////////////////////////////////////
  // roles -----------------
  // menu structure can include this tags (in the form asTitle,asItem,asMenu,...)
  // output device fmt translators hook to this tags to generate addicional content
    enum class Roles {Self,AM5,Panel,Title,Item,Pad,Accel,Cursor,Label,Value,Unit};

  //hook out and fmt callbacks for role tags included on menu structure
  //they provide a direct access to specific output driver
  template<Roles role,typename O,void (MenuOut::*f)(bool)>
  struct Role:public O {
    using O::O;
    Role(O& o):O(o) {}
    inline void out(MenuOut&) const;
    inline void fmt(MenuOut& o,bool io) const;
  };

  //////////////////////////////////////////////////////////////////
  // code ------------------------------------------
  template<Roles role,typename O,void (MenuOut::*f)(bool)>
  void Role<role,O,f>::out(MenuOut&o) const {
    fmt(o,true);
    O::out(o);
    fmt(o,false);
  }

  template<Roles role,typename O,void (MenuOut::*f)(bool)>
  inline void Role<role,O,f>::fmt(MenuOut& o,bool io) const {
    (static_cast<MenuOut&>(o).*f)(io);
  }
};//AM5
