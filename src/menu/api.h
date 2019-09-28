/* -*- C++ -*- */
#pragma once
#include "base.h"

//a reference to menu item that works also for static structures
struct Ref {
  Idx len;
  Idx* path;
  inline Idx head() const {return path[0];}
  inline Ref tail() const {return {len-1,&path[1]};}
};

struct Nil {};

////////////////////////////////////////////////////////////////////////////////
template<typename N=Nil> struct Drift:N {
  template<typename It,typename Nav,typename Out>
  inline static void print(It&,Nav&,Out&,Ref,Idx) {}
};

////////////////////////////////////////////////////////////////////////////////
template<typename O=Nil> struct Void:O {};

////////////////////////////////////////////////////////////////////////////////
template<typename I=Nil> struct Empty:I {};
