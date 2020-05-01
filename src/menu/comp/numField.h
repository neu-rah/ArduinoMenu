/* -*- C++ -*- */
#pragma once
/**
* @file numField.h
* @brief range validated numeric field
*/

#include <staticMenu.h>

namespace Menu {

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
      using I::canNav;
      inline static constexpr bool canNav() {return true;}
      using I::parentDraw;
      inline static constexpr bool parentDraw(Idx=0) {return true;}
      using I::activate;
      inline static ActRes activate() {return ActRes::Open;}
      inline static ActRes activate(Ref,Idx) {return activate();}
      template<typename Out,Tag role=Tag::Raw,bool toPrint=true>
      inline void print(Out& out) {
        out.template print<T,Out,toPrint>(value,out,role);
      }
      using I::cmd;
      template<Cmds c,typename Nav>
      inline void cmd(Nav& nav,Idx n=0) {
        _trace(MDO<<"NumValue::cmd "<<c<<" "<<n<<endl);
        switch(c) {
          case Cmds::Enter:
            if (nav.mode()==Mode::Tune||tune==0) {
              nav.setMode(Mode::Normal);
              nav.close();
            } else nav.setMode(Mode::Tune);
            return;
          case Cmds::Esc:nav.close();break;
          case Cmds::Up: {
              T s=(nav.mode()==Mode::Tune)?tune:step;
              if (value+s<=high) value+=s;
            }
            return;
          case Cmds::Down: {
              T s=(nav.mode()==Mode::Tune)?tune:step;
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
  using NumField=StaticWrap<
    AsMode<AsValue<NumValue<T,value,low,high,step,tune>>>,
    AsName<Label>,
    AsUnit<Unit>
  >;

};
