/* -*- C++ -*- */
#pragma once

#include "api.h"

//aux class for monometric text measure
template<int szX=1,int szY=1>
struct TextMeasure {
  template<typename O>
  struct As:O {
    template<typename T>
    static inline Idx measure(T o) {
      #ifdef ARDUINO
        return String(o).length();
      #else
        return _str(o);
      #endif
    }
    inline static constexpr int maxCharWidth() {return 1;}
    inline static constexpr int maxCharHeight() {return 1;}
    int textWidth(const char*s) const {return measure(s);}
    protected:
      #ifndef ARDUINO
      static inline Idx _str(const char*o){return std::string(o).length();}
      template<typename T>
      static inline Idx _str(T i){return std::string(std::to_string(i)).length();}
      #endif
  };
  template<typename O> using Open=As<O>;
};

template<Expr... O>
struct MenuOut:Chain<O...,Void>::template To<Obj<MenuOut<O...>>> {
  using Base=typename Chain<O...,Void>::template To<Obj<MenuOut<O...>>>;
  using This=MenuOut<O...>;
  using Base::Base;
};

template<typename O>
struct CanMeasure:O {
  // using O::raw;
  // template<typename T,typename Out,bool toPrint>
  // inline void print(T o,Out& out,Roles role=Roles::Raw) {
  //   if (toPrint) O::template print<T,Out,toPrint>(o,out,role);
  //   else out.useX(O::measure(o));
  // }
};

template<typename O>
struct PartialDraw:O {
  inline static constexpr bool partialDraw() {return true;}
};

template<typename Dev,Dev& dev>
struct StreamOut {
  template<typename O>
  struct As:O {
    using This=StreamOut<Dev,dev>::As<O>;
    inline static void nl() {dev<<endl;}
    template<typename T> inline static void raw(T o) {dev<<o;}
  };
  template<typename O> using Open=As<O>;
};

template<typename Dev,Dev& dev,typename O>
struct StreamIn:O {
  template<bool invY=false>
  inline static Cmd cmd() {
    char c;
    dev>>c;
    switch(c) {
      case '+': return Cmd::Up;
      case '-': return Cmd::Down;
      case '*': return Cmd::Enter;
      case '/': return Cmd::Esc;
      default:return Cmd::None;
    }
  }
};
