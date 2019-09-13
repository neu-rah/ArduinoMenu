/* -*- C++ -*- */
#pragma once

#include "api.h"

////////////////////////////////////////////////////////////////////////////////
// panels and viewports
template<idx_t x,idx_t y,idx_t w,idx_t h,typename O>
struct StaticPanel:public O {
  constexpr static inline idx_t orgX() {return x;}
  constexpr static inline idx_t orgY() {return y;}
  constexpr static inline idx_t width() {return w;}
  constexpr static inline idx_t height() {return h;}

  constexpr static inline idx_t posX() {return x;}
  constexpr static inline idx_t posY() {return y;}
  constexpr static inline idx_t freeX() {return w;}
  constexpr static inline idx_t freeY() {return h;}
  constexpr static inline idx_t free() {return w*h;}
  static inline void useX(idx_t ux=1) {}
  static inline void useY(idx_t uy=1) {}
};
