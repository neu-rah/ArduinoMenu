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

    template<Roles m,typename O>
    struct Role:public O {
      using O::O;
      using OutDef=typename O::OutDef;
      using RawOut=typename OutDef::RawOut;
      // Role(O& o):O(o) {}
      // static constexpr Roles mask=m;
      inline RawOut& out(RawOut& o) const {}
    };

    template<typename O>
    struct Role<itemRole,O>:public O {
      using O::O;
      using OutDef=typename O::OutDef;
      using RawOut=typename OutDef::RawOut;
      // Role(O& o):O(o) {}
      // static constexpr Roles mask=m;
      inline RawOut& out(RawOut& o) const {}
    };

    template<typename O> using asPanel=Role<panelRole,O>;
    template<typename O> using asTitle=Role<titleRole,O>;
    template<typename O> using asBody=Role<bodyRole,O>;
    template<typename O> using asItem=Role<itemRole,O>;
    template<typename O> using asAccel=Role<accelRole,O>;
    template<typename O> using asCursor=Role<cursorRole,O>;
    template<typename O> using asMode=Role<modeRole,O>;
    template<typename O> using asUnit=Role<unitRole,O>;
    template<typename O> using asMenu=Role<menuRole,O>;

    class Item;

    //the output interface
    struct Fmt:public Root {
      using Root=Root;
      inline void out(Item& i);
      //role format tag, and typelevel continuation
      //we can wrap, translate or abort printing here
      template<Roles,typename Next>
      inline void out() {Next::out(*this);}
      template<typename T>
      static inline void raw(T o) {}
    };

    // item interface
    struct Item {
      using Idx=typename Root::Idx;
      inline virtual void out(Fmt& o) const {Serial<<"Item::out(Fmt)"<<endl;}
      inline virtual Idx size() const {return 0;}
      inline virtual Item& operator[](Idx) {return *this;};
    };

    template<typename O>
    struct Prompt:public Item,private O {
      using O::O;
      template<typename... OO>
      inline Prompt(const char*title,OO... oo):O(title,oo...) {}
      inline Prompt(const char*title):O(title) {}
      inline void out(Fmt& o) const override {
        Serial<<"Prompt::out(Fmt)"<<endl;
        O::out(o);
      }
      inline Idx size() const override {return O::size();}
      inline Item& operator[](Idx n) override {return O::operator[](n);}
    // private: using Item& O::operator[](Idx);
    };

    static inline void out(Fmt& o) {
      Serial<<"Root::out(Fmt)"<<endl;
      return o;
    }
    static inline Idx size() {return 0;}
    inline Item& operator[](Idx n) {return *(Prompt<Root>*)this;}
  };

  // template<typename F,typename P>
  // using Root=RootDef<F,P>;
  template<typename R>
  using RootItem=typename R::Item;
  template<typename R>
  using RootFmt=typename R::Fmt;

  template<typename F,typename P>
  void RootDef<F,P>::Fmt::out(Item& i) {i.out(*this);}

  template<typename R>
  inline RootFmt<R>& operator<<(RootFmt<R>& o, RootItem<R>& i) {
    o.out(i);
    return o;
  }

  template<typename T,typename R>
  inline RootFmt<R>& operator<<(RootFmt<R>& o, T& i) {
    o.raw(i);
    return o;
  }

  template<typename O,typename R>
  inline O& operator<<(O& o,RootItem<R>& i) {O::out(i);return o;}

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
