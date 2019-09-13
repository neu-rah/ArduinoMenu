/* -*- C++ -*- */
#pragma once

#include "base.h"

////////////////////////////////////////////////////////////////////////////////
//navigation terminal
template<typename N>
struct Drift:N {
  static inline void up() {}
  static inline void down() {}
  static inline void enter() {}
  static inline void esc() {}
  constexpr static inline Modes mode() {return Modes::Normal;}
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
};

////////////////////////////////////////////////////////////////////////////////
//output terminal
template<typename O>
struct Void:O {
  template<typename T> inline static void raw(T&) {}
  inline static void nl() {}
  inline static idx_t top() {return 0;}
  template<typename Out>
  static inline void clrLine(idx_t n) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtPanel (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtMenu  (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtTitle (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtBody  (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtItem  (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtIndex (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtCursor(idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtName  (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtMode  (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtValue (idx_t) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtUnit  (idx_t) {}
};

////////////////////////////////////////////////////////////////////////////////
//menu structure terminal
template<typename I>
struct Empty:I {
  template<typename O,typename N=Drift<>>
  inline static void printTo() {}//print full item
  template<typename O,typename N=Drift<>>
  inline static void printItem(size_t i) {}//print a sub-item by index
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu() {
    _trace(MDO<<"Empty::printMenu");
    It::template printTo<Out,Nav>();
    It::template printMenu<Out,Nav>();
  }//print full menu
  inline static idx_t size() {return 0;}
};
