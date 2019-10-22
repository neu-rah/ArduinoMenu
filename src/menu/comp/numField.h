/* -*- C++ -*- */
#pragma once
/**
* @file numField.h
* @brief range validated numeric field
*/

#include <menu.h>

// namespace AM5 {

  /**
  * The NumField class links to any numeric type variable
  * and allows changing it between the validation range
  */
  template<typename T,T& value,T low,T high,T step,T tune,typename I=Empty<>>
  class NumField:public I {
    public:
      using I::I;
      using This=NumField<T,value,low,high,step,tune,I>;
      inline NumField()
        :reflex(value)
        {}
      template<typename Out> inline void print() {
        Out::raw(value);
        I::template print<Out>();
      }
      template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
      inline void print(It& it,Nav& nav) {
        // out.template fmtValue<true,Nav,Out,This>(nav,out,*this,0);
        print<Out>();
        // out.template fmtValue<false,Nav,Out,This>(nav,out,*this,0);
      }
      inline bool up() {
        T s=tunning?tune:step;
        if (value+s<=high) value+=s;
        return true;
      }
      inline bool down() {
        T s=tunning?tune:step;
        if (value-s>=low) {
          value-=s;
          return true;
        }
        return false;
      }
      template<typename Nav> inline bool enter(Nav& nav,Idx at=0) {return tunning^=true;}
      // inline bool enter() {return tunning^=true;}
      inline bool esc() {return true;}//true=>allow unfocus
      inline Modes mode() {return tunning?Modes::Tune:Modes::Edit;}
    protected:
      bool tunning=false;//TODO: this state should be stored on navigation! (or field agent)
      T reflex;//to check if original value changed
      // T low,high,step,tune;
      // static ItemCmd<This> cmds;
  };

  // template<typename T>
  // using NumField=asValue<NavHandler<NumFieldDef<T>>>;
// };
