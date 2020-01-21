/* -*- C++ -*- */
#pragma once

#include "base.h"

//implement CRTP
template<typename T>
struct Obj {
  using Type=T;
  inline Type& obj() const {return *(Type*)this;}
};

//basic output, just ignore the output
template<typename O>
struct Void:O {
  template<typename T>
  inline static void raw(T) {}
  template<typename I>
  inline static void printItem(I& i) {i.print();}
  inline static void newView() {}
  inline static constexpr bool partialDraw() {return false;}
  template<Roles role,bool io,bool toPrint>
  inline static void fmt(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  inline static constexpr bool isSame(void*) {return false;}
  inline static constexpr Idx top() {return 0;}
  template<typename Nav> inline static constexpr bool posTop(Nav&) {return false;}
  template<typename It,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(It& it,Idx idx=0,Idx top=0) {
    for(auto o:it) o->print(O::obj());
  }
  //to detect target menu change when nothing else changed
  inline static void lastDrawn(void*) {}
  constexpr static inline Idx orgX() {return 0;}
  constexpr static inline Idx orgY() {return 0;}
  constexpr static inline Idx width() {return idx_max;}
  constexpr static inline Idx height() {return idx_max;}
  inline static void setCursor(int x,int y) {}
  inline static constexpr int ascent() {return 0;}
};

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
  template<typename O>
  using Template=As<O>;
};

////////////////////////////////////////////////////////////////////////////////
template<typename N>
struct Drift:N {
  template<typename Out> inline static void print(Out& out) {}
  inline static Idx pos() {return 0;}
};

template<typename I,Idx max_depth>
struct Nav {
  template<typename N>
  struct As:N {
    using This=Nav<I,max_depth>::As<N>;
    using Data=I;
    Idx path[]{0};
    Idx level=0;
    Data root;
    inline Idx pos() const {return path[level];}
  };
};

template<typename I>
struct Empty:I {
  inline static Idx len() {return 0;}
  template<typename Out,Op=Op::Printing> inline void print(Out&) {}
  template<typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Out& out,Idx idx=0,Idx top=0) {
    out.printItems();
  }
};
