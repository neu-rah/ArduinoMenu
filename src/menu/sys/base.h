/* -*- C++ -*- */
#pragma once

#include "../parts/src/parts.h"
using namespace Parts;

namespace Menu {

  struct PartsCfg:Parts::DefaultCfg {
    #ifdef Arduino
      using Idx=byte;
    #else
      using Idx=uint16_t;//array size type
    #endif
  };
  using Idx=typename PartsCfg::Idx;
  template<Idx... path>
  using Path=Parts::Config<PartsCfg>::Path<path...>;
  using PathRef=Parts::Config<PartsCfg>::PathRef;
  template<typename Fst,typename Snd>
  using Pair=Parts::Config<PartsCfg>::Pair<Fst,Snd>;
  template<typename Fst,typename Snd>
  using StaticList=Parts::Config<PartsCfg>::StaticList<Fst,Snd>;

  using Key=int;//key type
  using ConstText=const char*;

  struct Yes {static constexpr bool value(){return true;}};
  struct No {static constexpr bool value(){return false;}};

  //commands
  enum class Cmd:Idx {Enter,Esc,Up,Down,Left,Right,Key};

  //output format
  using fmt_t=Idx;
  enum class Fmt:fmt_t {
    Panel,Menu,Title,Body,Item,Pad,
    Index,Cursor,Field,
    Accel,Label,EditMode,EditCursor,Value,Unit
  };

  //attributes
  enum class Style:Idx {
    None=0<<0,
    WrapNav=1<<0,
    ParentDraw=1<<1,
    IsField=1<<2,
    IsMenu=1<<3
  };

  Style operator&(Style a,Style b) {return (Style)(((Idx)a)&((Idx)b));}
  Style operator|(Style a,Style b) {return (Style)(((Idx)a)|((Idx)b));}
  Style operator^(Style a,Style b) {return (Style)(((Idx)a)^((Idx)b));}
  Style operator~(Style a) {return (Style)~((Idx)a);}
  // operator bool(Style o) {return (bool)o;}
  template<Style mask>
  bool is(Style s) {return ((Idx)s)==(Idx)mask;}
  template<Style mask>
  bool has(Style s) {return ((Idx)s)&(Idx)mask;}

  //navigation wrap (nav behavior)
  struct WrapNav {
    template<typename O>
    struct Part:O {
      using O::O;
      static constexpr Style styles() {return O::styles()|Style::WrapNav;}
    };
  };

  //style: item should be drawn by the parent when active
  struct ParentDraw {
    template<typename O>
    struct Part:O {
      using O::O;
      static constexpr Style styles() {return O::styles()|Style::ParentDraw;}
    };
  };

  // struct SelfDraw {
  //   template<typename O>
  //   struct Part:O {
  //     using O::O;
  //     static constexpr Style styles() {return O::styles()&~Style::ParentDraw;}
  //   };
  // };

  //style: item is a menu
  struct IsMenu {
    template<typename O>
    struct Part:O {
      using O::O;
      static constexpr Style styles() {return O::styles()|Style::IsMenu;}
    };
  };

  //makes a `block` of multiple parts that can be skipped as a single element
  //because `::Base` will point to the next item after the block
  template<template<typename> class... OO>
  struct Union {
    template<typename O>
    struct Part:Chain<OO...>::template To<O> {
      using Base=O;
    };
  };

  template<template<typename> class... OO>
  struct MakePart {
    template<typename O>
    struct Part:Chain<OO...>::template To<O> {
      using Base=typename Chain<OO...>::template To<O>;
      using This=Part<O>;
      using Base::Base;
    };
  };

  // enum class NavRes:Idx {
  //   None,//keep nav state
  //   Pass,//pass command to parent
  //   // Focus,//open as focus only (no level)
  //   Open,//open nav level (sub-menu)
  //   Close,//close nav level
  //   Draw//keep state and redraw
  // };

  // template<typename O>
  // O& operator<<(O& out,NavRes n) {
  //   switch (n) {
  //     case NavRes::None: return out<<"None";break;
  //     case NavRes::Pass: return out<<"Pass";break;
  //     case NavRes::Open: return out<<"Open";break;
  //     case NavRes::Close: return out<<"Close";break;
  //     case NavRes::Draw: return out<<"Draw";break;
  //     default: return out;
  //   }
  // }

  //interfaces
  class IItem;
  class IOut;
  class INav;

  //relational op's
  // struct RelOp {
  //   template<typename O>
  //   inline bool operator!=(const O o) const {return !operator==(o);}
  //   template<typename O>
  //   inline bool operator>=(const O o) const {return !operator<(o);}
  //   template<typename O>
  //   inline bool operator>(const O o) const {return !(operator==(o)||O::operator<(o));}
  // };

  // template<size_t n> 
  // struct Num:RelOp {
  //   template<size_t m>
  //   inline bool operator==(const Num<m>) const {return n==m;}
  //   template<size_t m>
  //   inline bool operator<(const Num<m>) const {return n<m;}
  // };

};

// #include "menu/sys/kernel/menuDebug.h"
// #ifdef RS_DEBUG
// namespace Parts {
//   template<typename T>
//   MDO& operator<<(MDO& mdo,T data) {
//     MDO::print(data);
//     return mdo;
//   }

//   MDO& operator<<(MDO& o,const Style s) {
//     if(s==Style::None) o<<"|None";
//     else {
//       if(has<Style::IsMenu>(s)) o<<"|IsMenu";
//       if(has<Style::WrapNav>(s)) o<<"|WrapNav";
//       if(has<Style::ParentDraw>(s)) o<<"|ParentDraw";
//     }
//     o<<"|";
//     // else if(has<Style::IsMenu>(s)) o<<"IsMenu";
//     return o;
//   }
//   MDO& operator<<(MDO& o,const Cmd c) {
//     switch(c) {
//       case Cmd::Enter: o<<"Enter";break;
//       case Cmd::Esc: o<<"Esc";break;
//       case Cmd::Up: o<<"Up";break;
//       case Cmd::Down: o<<"Down";break;
//       case Cmd::Left: o<<"Left";break;
//       case Cmd::Right: o<<"Right";break;
//       case Cmd::Key: o<<"Key";break;
//     }
//     return o;
//   }

// };
// #endif


