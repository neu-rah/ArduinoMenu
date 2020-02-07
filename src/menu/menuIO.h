/* -*- C++ -*- */
#pragma once

#include "api.h"

//aux class for monometric text measure
template<int szX=1,int szY=1>
struct TextMeasure {
  template<typename O>
  struct Part:O {
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
  template<typename O> using Open=Part<O>;
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
  struct Part:O {
    using This=StreamOut<Dev,dev>::Part<O>;
    inline static void nl() {dev<<endl;}
    template<typename T> inline static void raw(T o) {dev<<o;}
  };
};

template<typename Dev,Dev& dev,typename O>
struct StreamIn:O {
  template<typename Nav>
  inline static bool cmd(Nav& nav) {
    char c;
    dev>>c;
    switch(c) {
      case '+': return nav.cmd(Cmd::Up);
      case '-': return nav.cmd(Cmd::Down);
      case '*': return nav.cmd(Cmd::Enter);
      case '/': return nav.cmd(Cmd::Esc);
      default:return false;
    }
  }
};

struct None:Nil {
  template<bool invY=false>
  inline static constexpr Cmd cmd() {return Cmd::None;}
};

template<Expr... O>
struct StaticMenuOut:Chain<O...,Void>::template To<Obj<StaticMenuOut<O...>>> {
  using Base=typename Chain<O...,Void>::template To<Obj<StaticMenuOut<O...>>>;
  using This=StaticMenuOut<O...>;
  using Base::Base;
};

struct IOut {
  virtual inline void nl()=0;
  virtual void raw(const char* o)=0;
  virtual void printMenu(IItem& it,INav& nav, Op=Op::Printing)=0;
  virtual void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Op op=Op::Printing,bool toPrint=true)=0;
  // virtual void print(INav& nav,IOut& out,Op op=Op::Printing,Roles role=Roles::Raw)=0;

  template<typename It,typename Nav,Op op=Op::Printing>
  inline void printMenu(It& it,Nav& nav) {printMenu(it,nav,op);}

  // template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  // inline void print(Nav& nav,Out& out)
  template<typename It,typename Nav,Op op=Op::Printing,bool toPrint=true>
  void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    printItem(it,nav,n,s,e,m,op,toPrint);
  }
};

template<Expr...  O>
struct MenuOut:IOut,Chain<O...,Void>::template To<Obj<MenuOut<O...>>> {
  using Base=typename Chain<O...,Void>::template To<Obj<MenuOut<O...>>>;
  using This=MenuOut<O...>;
  using Base::Base;
  inline void nl() override {Base::nl();}
  inline void raw(const char* o) override {Base::raw(o);}
  void printMenu(IItem& it,INav& nav, Op op=Op::Printing) override;
  void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Op op=Op::Printing,bool toPrint=true) override;
};
