/* -*- C++ -*- */
#pragma once

#include "chain.h"

#ifdef ARDUINO
  #ifdef ARDUINO_AVR_ATTINY13
    #include <avr/pgmspace.h>
    struct __FlashStringHelper;
    #define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
    #include "extra/Serial13/serial13.h"
    extern SwSerial Serial;
  #endif
#else
  #include <cstddef>
  #include <limits>
  #include <iostream>
  using namespace std;
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
  enum class Tag:int {
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

  inline Idx operator|(Tag a,Tag b) {return static_cast<Idx>(a)|static_cast<Idx>(b);}
  inline Idx operator|(Tag a,Idx b) {return static_cast<Idx>(a)&b;}
  inline Idx operator|(Idx a,Tag b) {return a&static_cast<Idx>(b);}
  inline Idx operator&(Tag a,Tag b) {return static_cast<Idx>(a)&static_cast<Idx>(b);}
  inline Idx operator&(Tag a,Idx b) {return static_cast<Idx>(a)&b;}
  inline Idx operator&(Idx a,Tag b) {return a&static_cast<Idx>(b);}

  inline bool is(Tag o,Idx p) {return static_cast<Idx>(o)==p;}
  inline bool has(Tag o,Idx p) {return static_cast<Idx>(o)|p;}

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
    constexpr inline O& operator<<(O& o,Tag r) {return o<<roleNames[(Idx)r];}

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
  // #ifdef ARDUINO
    #define endl "\n\r"
  // #else
  //   #define endl std::endl
  // #endif

};
