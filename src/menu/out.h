/* -*- C++ -*- */
#pragma once

#include "base.h"
namespace AM5 {
  //////////////////////////////////////////////////////
  // output
  template<typename O=Nil>
  struct Void:public O {
    constexpr static inline bool isRange() {return false;}
    template<typename,typename,bool,size_t> static inline void fmtMenu() {}
    template<typename,typename,bool,size_t> static inline void fmtMenuBody() {}
    template<typename,typename,bool,size_t> static inline void fmtTitle() {}
    template<typename,typename,bool,size_t> static inline void fmtItem() {}
    template<typename,typename,bool,size_t> static inline void fmtIndex() {}
    template<typename,typename,bool,size_t> static inline void fmtCursor() {}
  };

  template<ostream& device=std::cout,typename O=Void<>>
  struct StdOut:public O {
    using This=StdOut<device,O>;
    template<typename T>
    static inline void raw(T o) {device<<o;}
    static inline void nl() {device<<std::endl;}
    static inline StdOut& endl(StdOut& o) {nl();return o;}
  };

  //static panel ------------------------------------------------
  // describes output geometry,
  // may be whole device, but must not exceed
  // it has origin coordinates to be displaced around
  template<idx_t x,idx_t y,idx_t w,idx_t h,typename O>
  struct StaticPanel:public O {
    constexpr static inline idx_t orgX() {return x;}
    constexpr static inline idx_t orgY() {return y;}
    constexpr static inline idx_t width() {return w;}
    constexpr static inline idx_t height() {return h;}
  };

  //its different than a scroll viewport
  //as it refers to the top line of the menu structure
  //minimize printing on line menus
  template<typename O>
  class RangePanel:public O {
    public:
      constexpr static inline bool isRange() {return true;}
      inline size_t top() const {return topLine;}
      inline void setTop(size_t n) {topLine=n;}
    protected:
      size_t topLine=0;
  };

};
