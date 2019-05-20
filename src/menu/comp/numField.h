/* -*- C++ -*- */
#pragma once
/**
* @file numField.h
* @brief range validated numeric field
*/

#include <menu.h>

// namespace AM5 {

  template<typename T,typename I=Empty<>>
  class NumField:public I {
    public:
      using I::I;
      inline NumField(T& var,T l, T h,T s,T t=0)
        :value(&var)
        ,reflex(var)
        ,low(l)
        ,high(h)
        ,step(s)
        ,tune(t) {}
      // constexpr static inline bool canNav() {return true;}
      // inline NavAgent activate() {return {this,&cmds};}
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
      // static ItemCmd<This> cmds;
  };

  // template<typename T>
  // using NumField=asValue<NavHandler<NumFieldDef<T>>>;
// };
