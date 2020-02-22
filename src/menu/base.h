/* -*- C++ -*- */
#pragma once

#include "chain.h"

#ifndef ARDUINO
  #include <limits>
#endif

namespace Menu {
  #ifdef ARDUINO
    using Idx=uint8_t;
    constexpr Idx idx_max=(1ul<<(sizeof(Idx)<<3))-1;
  #else
    using Idx=std::size_t;
    constexpr Idx idx_max=std::numeric_limits<Idx>::max();
  #endif

  //edit modes
  enum class Mode {Normal,Edit,Tune};

  //formating parts/roles
  enum class Roles:int {
    None=0<<0,
    Raw=1<<0,
    Panel=1<<1,
    Menu=1<<2,
    Title=1<<3,
    Body=1<<4,
    Item=1<<5,
    Index=1<<6,
    Cursor=1<<7,
    Name=1<<8,
    Mode=1<<9,
    Value=1<<10,
    Unit=1<<11
  };

  //navigation commands
  enum class Cmd:Idx {None=0,Enter=1,Esc=2,Up=4,Down=8,Left=16,Right=32};

  // Output Device Operations
  enum class Op {Measure,Printing,ClearChanges};

  inline Idx operator|(Roles a,Roles b) {return static_cast<Idx>(a)|static_cast<Idx>(b);}
  inline Idx operator|(Roles a,Idx b) {return static_cast<Idx>(a)&b;}
  inline Idx operator|(Idx a,Roles b) {return a&static_cast<Idx>(b);}
  inline Idx operator&(Roles a,Roles b) {return static_cast<Idx>(a)&static_cast<Idx>(b);}
  inline Idx operator&(Roles a,Idx b) {return static_cast<Idx>(a)&b;}
  inline Idx operator&(Idx a,Roles b) {return a&static_cast<Idx>(b);}

  inline bool is(Roles o,Idx p) {return static_cast<Idx>(o)==p;}
  inline bool has(Roles o,Idx p) {return static_cast<Idx>(o)|p;}

  inline Idx operator|(Cmd a,Cmd b) {return static_cast<Idx>(a)|static_cast<Idx>(b);}
  inline Idx operator|(Cmd a,Idx b) {return static_cast<Idx>(a)&b;}
  inline Idx operator|(Idx a,Cmd b) {return a&static_cast<Idx>(b);}
  inline Idx operator&(Cmd a,Cmd b) {return static_cast<Idx>(a)&static_cast<Idx>(b);}
  inline Idx operator&(Cmd a,Idx b) {return static_cast<Idx>(a)&b;}
  inline Idx operator&(Idx a,Cmd b) {return a&static_cast<Idx>(b);}

  inline bool is(Cmd o,Idx p) {return static_cast<Idx>(o)==p;}
  inline bool has(Cmd o,Idx p) {return static_cast<Idx>(o)|p;}

  struct Area {
    Idx width;
    Idx height;
  };

  #ifdef MENU_DEBUG
    constexpr const char* roleNames[]{
      "None","Raw","Panel","Menu","Title","Body","Item",
      "Index","Cursor","Name","Mode","Value","Unit"
    };

    constexpr const char* modeNames[]{"Normal","Edit","Tune"};
    constexpr const char* opNames[]{"Measure","Printing","ClearChanges",};

    template<typename O>
    constexpr inline O& operator<<(O& o,Roles r) {return o<<roleNames[(Idx)r];}

    template<typename O>
    constexpr inline O& operator<<(O& o,Mode r) {return o<<modeNames[(Idx)r];}

    template<typename O>
    inline O& operator<<(O& o,Op r) {o<<opNames[(Idx)r];return o;}

    template<typename O>
    inline O& operator<<(O& o,Cmd r) {
      switch(r){
        case Cmd::None:return o<<"None";
        // case Cmd::Activate:return o<<"Activate";
        case Cmd::Enter:return o<<"Enter";
        case Cmd::Esc:return o<<"Esc";
        case Cmd::Up:return o<<"Up";
        case Cmd::Down:return o<<"Down";
        case Cmd::Left:return o<<"Left";
        case Cmd::Right:return o<<"Right";
      }
      return o;
    }

  #endif

  ////////////////////////////////////////////////////////////////////////////////
  //implement CRTP
  // otherwise encapsulation becomes a code issue, making customizations hard
  template<typename T>
  struct Obj {
    using Type=T;
    inline Type& obj() const {return *(Type*)this;}
  };

  #include "debug.h"
  #define endl "\n\r"

  template<Roles role,typename O>
  struct As:O {
    using O::print;
    template<typename It,typename Out,Roles=role,bool toPrint=true>
    static inline void printItem(It& it,Out& out,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      // trace(MDO<<"As<"<<role<<">");
      out.template fmt<role,true>(n,s,e,m);
      O::template printItem<It,Out,role,toPrint>(it,out,n,s,e,m);
      out.template fmt<role,false>(n,s,e,m);
      // trace(MDO<<"</"<<role<<">"<<endl);
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
};
