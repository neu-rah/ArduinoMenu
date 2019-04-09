/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// store text on flash (Arduino framework)

#include "../items.h"

namespace AM5 {

  template<typename T,typename O=Empty>
  class NumFieldDef:public O {
    public:
      using This=NumFieldDef<T,O>;
      // using O::O;
      inline NumFieldDef(T& var,T l, T h,T s,T t=0)
        :value(&var)
        ,reflex(var)
        ,low(l)
        ,high(h)
        ,step(s)
        ,tune(t) {}
      // constexpr static inline bool canNav() {return true;}
      inline NavAgent navAgent() {return {this,&cmds};}
      inline void out(MenuOut &o) const {
        //reflex=*value;can not update here!
        o<<*value;
      }
      inline bool up() {
        T s=tunning?tune:step;
        if (*value+s<=high) *value+=s;
        return true;
      }
      inline bool down() {
        T s=tunning?tune:step;
        if (*value-s>=low) {
          *value-=s;
          return true;
        }
        return false;
      }
      inline bool enter() {return tunning^=true;}
      inline bool esc() {return false;}
    protected:
      bool tunning=false;//TODO: this state should be stored on navigation! (or field agent)
      T* value;
      T reflex;//to check if original value changed
      T low,high,step,tune;
      static ItemCmd<This> cmds;
  };

  template<typename T>
  using NumField=asValue<NumFieldDef<T>>;
};
