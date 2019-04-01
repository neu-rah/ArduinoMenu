/* -*- C++ -*- */
#pragma once

namespace Menu {
  struct Item;
  struct MenuOut;

  //////////////////////////////////////////////////////////
  // roles -----------------
  enum class Roles {Self,Menu,Panel,Title,Item,Pad,Accel,Cursor,Label,Mode,Value,Unit};

  template<Roles role,typename O,void (MenuOut::*f)(bool)>
  struct Role:public O {
    using O::O;
    inline void out(MenuOut&);
    inline void fmt(MenuOut& o,bool io);
  };

  //////////////////////////////////////////////////////////////////
  // code ------------------------------------------
  template<Roles role,typename O,void (MenuOut::*f)(bool)>
  void Role<role,O,f>::out(MenuOut&o) {
    fmt(o,true);
    O::out(o);
    fmt(o,false);
  }

  template<Roles role,typename O,void (MenuOut::*f)(bool)>
  inline void Role<role,O,f>::fmt(MenuOut& o,bool io) {
    (static_cast<MenuOut&>(o).*f)(io);
  }
};//Menu
