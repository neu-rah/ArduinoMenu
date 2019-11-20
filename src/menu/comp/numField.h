/* -*- C++ -*- */
#pragma once
/**
* @file numField.h
* @brief range validated numeric field
*/

#include <menu.h>

// namespace AM5 {

  /**
  * The NumValue class links to any numeric type variable
  * and allows changing it between the validation range
  */
  template<typename T,T& value,T low,T high,T step,T tune=0,typename I=Empty>
  class NumValue:public I {
    public:
      using I::I;
      using This=NumValue<T,value,low,high,step,tune,I>;
      inline NumValue():reflex(value){}
      inline static constexpr bool canNav() {return true;}
      inline static constexpr bool isMenu() {return false;}
      inline static constexpr bool parentDraw(Idx=0) {return true;}
      inline static bool activate() {
        //nav.setMode(Modes::Edit);//either need Nav& or nav should do this part
        return true;
      }
      template<typename Out,Roles role=Roles::Raw>
      inline void print(Out& out) {out.raw(value,role);}
      using I::cmd;
      template<Cmds c,typename Nav>
      inline void cmd(Nav& nav) {
        switch(c) {
          case Cmds::Enter:
            if (nav.mode()==Modes::Tune||tune==0) {
              nav.setMode(Modes::Normal);
              nav.close();
            } else nav.setMode(Modes::Tune);
            return;
          case Cmds::Esc:nav.close();break;
          case Cmds::Up: {
              T s=(nav.mode()==Modes::Tune)?tune:step;
              if (value+s<=high) value+=s;
            }
            return;
          case Cmds::Down: {
              T s=(nav.mode()==Modes::Tune)?tune:step;
              if (value-s>=low) value-=s;
            }
            return;
          default:I::template cmd<c,Nav>(nav);
        }
      }

    protected:
      T reflex;//to check if original value changed
  };

  template<typename Label,typename T,T& value,T low,T high,T step,T tune=0,typename Unit=Empty>
  using NumField=StaticWrap<AsMode<AsValue<NumValue<T,value,low,high,step,tune>>>,AsName<Label>,AsUnit<Unit>>;

// };
