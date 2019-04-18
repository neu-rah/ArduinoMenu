/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {

  template<typename O>
  class NavPosDef:public O {
    public:
      inline bool selected(size_t n) const {return at==n;}
      inline bool up() {
        if (at<O::size()-1) {at++;return true;}
        return O::up();
      }
      inline bool down() {
        if (at>0) {at--;return true;}
        return O::down();
      }
      inline size_t pos() const {return at;}
    protected:
      size_t at=0;
  };
};
