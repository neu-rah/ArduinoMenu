/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// some utilities and base definitions

namespace Menu {

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
    enum class Roles {Self,Menu,Panel,Title,Item,Pad,Accel,Cursor,Label,Mode,Value,Unit};

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
};//Menu
