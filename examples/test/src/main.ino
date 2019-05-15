/* case study:
composition: Z<A> ==========================

STATIC
DATA:    [=         ]   9.4% (used 192 bytes from 2048 bytes)
PROGRAM: [          ]   4.8% (used 1482 bytes from 30720 bytes)

VIRTUAL
DATA:    [=         ]   9.8% (used 200 bytes from 2048 bytes)
PROGRAM: [          ]   4.9% (used 1498 bytes from 30720 bytes)

8 bytes of ram

composition: Z<E<D<C<B<A>>>>> =======================

STATIC
DATA:    [=         ]   9.4% (used 192 bytes from 2048 bytes)
PROGRAM: [          ]   4.8% (used 1482 bytes from 30720 bytes)

VIRTUAL
DATA:    [=         ]  11.1% (used 228 bytes from 2048 bytes)
PROGRAM: [=         ]   5.2% (used 1602 bytes from 30720 bytes)

because virtual table is unique for each class that changes a function
so having long vtables does not go well with composition
*/

/*
#define STATIC

#ifdef STATIC
  struct A {
    template<typename O>
    static inline void b() {O::_b();}
    template<typename O>
    static inline void c() {O::_c();}
    template<typename O>
    static inline void d() {O::_d();}
    template<typename O>
    static inline void e() {O::_e();}
    template<typename O>
    static inline void f() {O::_g();}
  };

  template<typename O> struct B:public O {static inline void b() {Serial.println("B::b");}};
  template<typename O> struct C:public O {static inline void b() {Serial.println("C::c");}};
  template<typename O> struct D:public O {static inline void b() {Serial.println("D::d");}};
  template<typename O> struct E:public O {static inline void b() {Serial.println("E::e");}};

  template<typename O>
  class Z:public O {
    public:
      static inline void _b() {a.b();}
      static inline void _c() {a.c();}
      static inline void _d() {a.d();}
      static inline void _e() {a.e();}
      static inline void _g() {a.g();}
      inline void g() const {Serial.println("Z::g");}
    protected:
      static Z a;
  };

  template<typename O>
  Z<O> Z<O>::a;

#else
  struct A {
    virtual inline void b() const=0;
    virtual inline void c() const=0;
    virtual inline void d() const=0;
    virtual inline void e() const=0;
    virtual inline void g() const=0;
  };

  template<typename O> struct B:public O {inline void b() const override {Serial.println("B::b");}};
  template<typename O> struct C:public O {inline void c() const override {Serial.println("C::c");}};
  template<typename O> struct D:public O {inline void d() const override {Serial.println("D::d");}};
  template<typename O> struct E:public O {inline void e() const override {Serial.println("E::e");}};

  template<typename O>
  class Z:public O {
    public:
      inline void g() const override {Serial.println("Z::g");}
  };

  Z<E<D<C<B<A>>>>> a;
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial);
  #ifdef STATIC
    Z<E<D<C<B<A>>>>>::template f<Z<A>>();
  #else
    a.g();
  #endif
}

void loop() {}
*/

//and this case is too complicated
//the idea was to break long vtables into individual components
//to avoid numerous vtable population due to long inheritance chains

template<typename O,typename F,typename... OO>
struct Func {
  using Target=void(O::*)(OO...);
  inline Func(O& o,F f):o(o),f(f) {}
  virtual void operator()(OO... oo) {(o.*f)(oo...);}
  O& o;
  F f;
};

struct A {};

template<typename O>
struct Z:public O {
  static Func<Z<O>,[]{Serial.println("Z::z");}> f;
};

Z<A> o;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  o.f();
}

void loop() {}
