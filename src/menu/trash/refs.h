/* -*- C++ -*- */
#pragma once

//this can be just types
template<typename O>
struct StaticObj {
  inline static O& obj() {return O();}
};

template<typename O, O& o>
struct StaticRef {
  inline static O& obj() {return o;}
};

//this are objects
template<typename O>
struct Ref {
  O& ref;
  inline Ref(O& r):ref(r) {}
  inline O& obj() const {return ref;}
};

template<typename O>
struct ConstRef {
  const O& ref;
  inline Ref(const O& r):ref(r) {}
  inline O& obj() const {return ref;}
};
