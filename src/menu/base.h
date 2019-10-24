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
enum class Roles:Idx {Panel,Menu,Title,Body,Prompt,Index,Cursor,Name,Mode,Value,Unit,Raw};
enum class Cmds:Idx {None=0,Activate=1,Enter=2,Esc=4,Up=8,Down=16,Left=32,Right=64};

//a reference to menu item that works also for static structures
struct Ref {
  Idx len;
  Idx* path;
  inline Idx head() const {return path[0];}
  inline Ref tail() const {return {(Idx)len-1,&path[1]};}
};

#ifdef MENU_DEBUG
  constexpr char* roleNames[]{
    "Panel","Menu","Title","Body","Prompt","Index",
    "Cursor","Name","Mode","Value","Unit","Raw"
  };

  template<typename O>
  constexpr inline O& operator<<(O& o,Roles r) {return o<<roleNames[(Idx)r];}

  template<typename O>
  inline O& operator<<(O& o,Cmds r) {
    switch(r){
      case Cmds::None:return o<<"None";
      case Cmds::Activate:return o<<"Activate";
      case Cmds::Enter:return o<<"Enter";
      case Cmds::Esc:return o<<"Esc";
      case Cmds::Up:return o<<"Up";
      case Cmds::Down:return o<<"Down";
      case Cmds::Left:return o<<"Left";
      case Cmds::Right:return o<<"Right";
    }
  }

#endif

#include "debug.h"
#define endl "\r\n"

template<Roles Part>
struct AllowRole {
  template<Roles P> inline static constexpr bool allowed() {return Part==P;}
};

template<Roles Part>
struct DenyRole {
  template<Roles P> inline static constexpr bool allowed() {return Part!=P;}
};
