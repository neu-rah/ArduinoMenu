/* -*- C++ -*- */
#pragma once

namespace AM5 {

  template<typename F,typename P>
  struct RootDef {
    using Root=RootDef<F,P>;
    using Framework=F;
    using Platform=P;
    using Idx=typename Platform::Idx;
    using RoleType=typename Framework::RoleType;

    enum Roles:RoleType {
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

    class Item;

    //the output interface
    struct Fmt:public Root {
      using Root=Root;
      inline void out(Item& i);
      //role format tag, and typelevel continuation
      //we can wrap, translate or abort printing here
      template<Roles,typename Next>
      inline void out() {Next::out(*this);}
    };

    // item interface
    struct Item:public Root {
      using Idx=typename Root::Idx;
      inline virtual void out(Fmt& o) const {}
      inline virtual Idx size() const {return 0;}
      inline virtual Item& operator[](Idx) {return *this;};
    };

    template<typename O=Item>
    struct Prompt:public O {
      using O::O;
    };

  };

  template<typename F,typename P>
  void RootDef<F,P>::Fmt::out(Item& i) {i.out(*this);}

  //menu output with root device
  template<typename Device,Device& dev,typename O>
  struct RawOut:public O {
    using This=RawOut<Device,dev,O>;
    using Root=typename O::Root;
    using Item=typename Root::Item;
    template<typename T>
    static inline void raw(T o) {dev<<o;}
    inline void out(Item& i) {i.out(*this);}
    // template<typename T> static inline void out(T o) {dev<<o;}
  };



  // template<typename O,typename F,typename P>
  // inline O& operator<<(O& o,RootDef<F,P>& i) {O::out(i);return o;}
  //
  // template<typename O,typename T>
  // inline O& operator<<(O& o,T& i) {return o<<i;}

  /*
  Out<Comp<Root>>
  call:
  Out::operator<<(Item&)->Comp(Out&)->[Out::out<N>]->Root()

  this way compositions are output device agnostic
  and still depend on a Framwork/Platform

  */

};//AM5
