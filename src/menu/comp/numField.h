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
      inline static constexpr bool canNav() {return true;}
      inline static constexpr bool parentDraw() {return true;}
      template<typename Out> inline void print() {
        Out::raw(value);
        I::template print<Out>();
      }
      template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
      inline void print(It& it,Nav& nav) {
        it.template fmt<Roles::Mode,true ,It,Out,Nav>(nav);
        it.template fmt<Roles::Mode,false ,It,Out,Nav>(nav);
        it.template fmt<Roles::Value,true ,It,Out,Nav>(nav);
        print<Out>();
        it.template fmt<Roles::Value,false ,It,Out,Nav>(nav);
      }
      using I::cmd;
      template<Cmds c,typename It,typename Nav>
      inline bool cmd(It& it,Nav& nav) {
        trace(MDO<<"numField::cmd:"<<c<<endl);
        switch(c) {
          case Cmds::Activate:
            nav.setMode(Modes::Edit);
            return true;//yes we can process commands
          case Cmds::Enter:
            if (nav.mode()==Modes::Tune) {
              nav.setMode(Modes::Normal);
              return false;
            } else {
              nav.setMode(Modes::Tune);
              return true;
            }
            // return tunning^=true;//assuming true => proceed
          case Cmds::Esc:assert(false);return true;
          case Cmds::Up: {
              T s=(nav.mode()==Modes::Tune)?tune:step;
              if (value+s<=high) {
                value+=s;
                return true;
              }
            } return false;
          case Cmds::Down: {
              T s=(nav.mode()==Modes::Tune)?tune:step;
              if (value-s>=low) {
                value-=s;
                return true;
              }
            } return false;
          default:break;
        }
        return I::template cmd<c,It,Nav>(it,nav);
      }

    protected:
      T reflex;//to check if original value changed
  };

  // template<typename T>
  // using NumField=asValue<NavHandler<NumFieldDef<T>>>;
// };
