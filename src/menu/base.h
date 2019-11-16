/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  using Idx=uint8_t;
  constexpr Idx idx_max=(1ul<<(sizeof(Idx)<<3))-1;
#else
  #include <limits>
  using Idx=std::size_t;
  constexpr Idx idx_max=std::numeric_limits<Idx>::max();
#endif

enum class Modes {Normal,Edit,Tune};
enum class Roles:Idx {Raw,Panel,Menu,Title,Body,Item,Index,Cursor,Name,Mode,Value,Unit};
enum class Cmds:Idx {None=0,/*Activate=1,*/Enter=2,Esc=4,Up=8,Down=16,Left=32,Right=64};

template<Roles role,typename O>
struct As:O {
  template<typename Out>
  inline static void print(Out& out) {O::template print<Out>(out);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav,Out& out) {
    it.template fmt<role,true,Nav,Out>(nav,out);
    O::template print<It,Nav,Out>(it,nav,out);
    it.template fmt<role,false,Nav,Out>(nav,out);
  }
};

template<typename O> using AsPanel=As<Roles::Panel,O>;
template<typename O> using AsMenu=As<Roles::Menu,O>;
template<typename O> using AsTitle=As<Roles::Title,O>;
template<typename O> using AsBody=As<Roles::Body,O>;
template<typename O> using AsItem=As<Roles::Item,O>;
template<typename O> using AsIndex=As<Roles::Index,O>;
template<typename O> using AsCursor=As<Roles::Cursor,O>;
template<typename O> using AsName=As<Roles::Name,O>;
template<typename O> using AsMode=As<Roles::Mode,O>;
template<typename O> using AsValue=As<Roles::Value,O>;
template<typename O> using AsUnit=As<Roles::Unit,O>;
template<typename O> using AsRaw=As<Roles::Raw,O>;

#ifdef MENU_DEBUG
  constexpr const char* roleNames[]{
    "Panel","Menu","Title","Body","Prompt","Index",
    "Cursor","Name","Mode","Value","Unit","Raw"
  };

  template<typename O>
  constexpr inline O& operator<<(O& o,Roles r) {return o<<roleNames[(Idx)r];}

  template<typename O>
  inline O& operator<<(O& o,Cmds r) {
    switch(r){
      case Cmds::None:return o<<"None";
      // case Cmds::Activate:return o<<"Activate";
      case Cmds::Enter:return o<<"Enter";
      case Cmds::Esc:return o<<"Esc";
      case Cmds::Up:return o<<"Up";
      case Cmds::Down:return o<<"Down";
      case Cmds::Left:return o<<"Left";
      case Cmds::Right:return o<<"Right";
    }
  }

#endif

//a reference to menu item that works also for static structures
//and dynamic access
struct Ref {
  Idx len;
  const Idx* path;
  inline Idx head() const {return path[0];}
  inline Ref tail() const {return {(Idx)(len-1),&path[1]};}
  inline operator Idx() const {return len;}
  inline operator bool() const {return len;}
  inline Ref parent() const {
    return len?(Ref){len-1,path}:*this;
  }
};

// template<typename Nav,Nav& nav>
// struct NavRef:Ref {
//   inline NavRef():Ref{nav.level+1,nav.path} {}
//   inline NavRef<Nav,nav> parent() const {return len?(NavRef<Nav,nav>){len-1,path}:*this;}
//   inline Nav& root() const {return nav;}
// };

#include "debug.h"
#define endl "\n\r"
