/* -*- C++ -*- */
#pragma once

namespace AM5 {

  enum Roles:uint16_t {
    noRole=0,
    panelRole=1,
    titleRole=2,
    bodyRole=4,
    itemRole=8,
    accelRole=16,
    cursorRole=32,
    modeRole=64,
    valueRole=128,
    unitRole=256,
    menuRole=512,
  };

  //the output interface
  struct Fmt {
    template<typename I>
    inline operator<<(I &o) {o.out(*this);}
    //generic raw output
    template<typename T>
    static inline void out() {}
    //role format tag
    template<typename T,Roles>
    static inline void out() {out<T>();}
  };

  template<typename Device,Device& dev,typename O=Fmt>
  struct RawOut:public O {
    template<typename T>
    static inline void out(T& o) {dev<<o;}
  };

  // template<typename O,typename Raw=RawOutType>
  // struct OutDef:public O {
  //   using This=OutDef<Raw,O>;
  //   using RawOut=Raw;
  // };

  template<typename F,typename P>
  struct RootDef {
    using Root=RootDef<F,P>;
    using Framework=F;
    using Platform=P;
    using Idx=typename Platform::Idx;
  };

  template<typename O>
  struct Item:public O {
    using Idx=typename O::Root::Idx;
    inline virtual Idx size() const {return 0;}
    inline virtual Item& operator[](size_t n) const =0;
  };

  /*
  Out<Comp<Root>>
  call:
  Out::operator<<(Item&)->Comp(out(Out&))->Root()

  this way compositions are output device agnostic
  and still depend on a Framwork/Platform

  */

};//AM5
