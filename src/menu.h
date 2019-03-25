/* -*- C++ -*- */
#pragma once

namespace AM5 {

  template<typename F,typename P>
  struct RootDef {
    inline RootDef() {}
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

    template<typename O,Roles m>
    struct Role:public O {
      using O::O;
      // using OutDef=typename O::OutDef;
      // using RawOut=typename OutDef::RawOut;
      // Role(O& o):O(o) {}
      // static constexpr Roles mask=m;
      // inline RawOut& out(RawOut& o) const {}
    };

    // template<typename O>
    // struct Role<O,itemRole>:public O {
    //   using O::O;
    //   // using OutDef=typename O::OutDef;
    //   // using RawOut=typename OutDef::RawOut;
    //   // Role(O& o):O(o) {}
    //   // static constexpr Roles mask=m;
    //   // inline RawOut& out(RawOut& o) const {}
    // };

    template<typename O> using asPanel  =Role<O,O::Roles::panelRole>;
    template<typename O> using asTitle  =Role<O,O::Roles::titleRole>;
    template<typename O> using asBody   =Role<O,O::Roles::bodyRole>;
    template<typename O> using asItem   =Role<O,O::Roles::itemRole>;
    template<typename O> using asAccel  =Role<O,O::Roles::accelRole>;
    template<typename O> using asCursor =Role<O,O::Roles::cursorRole>;
    template<typename O> using asMode   =Role<O,O::Roles::modeRole>;
    template<typename O> using asUnit   =Role<O,O::Roles::unitRole>;
    template<typename O> using asMenu   =Role<O,O::Roles::menuRole>;

    struct Item;

    //the output interface
    struct Fmt {

      using Root=RootDef<F,P>;

      size_t out(Item& i);

      //role format tag, and typelevel continuation
      //we can wrap, translate or abort printing here
      template<Roles,typename Next>
      inline size_t out() {
        Serial<<"Fmt::out<Roles,Next>()"<<endl;
        Next::out(*this);
      }
      inline virtual size_t raw(const char[]) {return 0;}
      inline virtual size_t raw(char) {return 0;}
      inline virtual size_t raw(unsigned char, int = DEC) {return 0;}
      inline virtual size_t raw(int, int = DEC) {return 0;}
      inline virtual size_t raw(unsigned int, int = DEC) {return 0;}
      inline virtual size_t raw(long, int = DEC) {return 0;}
      inline virtual size_t raw(unsigned long, int = DEC) {return 0;}
      inline virtual size_t raw(double, int = 2) {return 0;}
      inline virtual size_t raw(const __FlashStringHelper *) {return 0;}
      inline virtual size_t raw(const String &) {return 0;}
      inline virtual size_t raw(const Printable&) {return 0;}
    };

    struct Item {
      inline virtual size_t out(Fmt& o) const {return 0;}
      inline virtual Idx size() const {return 0;}
      inline virtual Item& operator[](Idx) {return *this;};
    };

    static inline size_t out(Fmt& o) {return 0;}
    static inline Idx size() {return 0;}
    inline Item& operator[](Idx n) {return *(Item*)this;}

    template<typename O>
    struct Prompt:public Item,private O {
      using O::O;
      using Root=typename O::Root;
      using Item=typename Root::Item;
      using Fmt=typename Root::Fmt;
      using Idx=typename Root::Idx;
      inline Prompt():O() {}
      template<typename... OO>
      inline Prompt(const char*title,OO... oo):O(title,oo...) {}
      inline Prompt(const char*title):O(title) {}
      inline size_t out(Fmt& o) const override {return O::out(o);}
      inline Idx size() const override {return O::size();}
      inline Item& operator[](Idx n) override {return O::operator[](n);}
    };

  };//RootDef

  template<typename F,typename P>
  using RootItem=typename RootDef<F,P>::Item;
  template<typename F,typename P>
  using RootFmt=typename RootDef<F,P>::Fmt;

  template<typename F,typename P>
  size_t RootDef<F,P>::Fmt::out(Item& i) {return i.out(*this);}

  /////////////////////////////////////////////////
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,RootItem<F,P>& i) {o.out(i);return o;}

  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,const char i[]) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,char i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,unsigned char i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,int i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,unsigned int i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,long i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,unsigned long i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,double i) {o.raw(i);return o;}
  //arduino specific!!!!
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,const __FlashStringHelper *i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,const String &i) {o.raw(i);return o;}
  template<typename F,typename P>
  inline RootFmt<F,P>& operator<<(RootFmt<F,P>& o,const Printable&i) {o.raw(i);return o;}

};//AM5
