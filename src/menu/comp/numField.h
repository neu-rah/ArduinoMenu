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
      inline static /*constexpr*/ bool parentDraw(Idx=0) {return true;}
      inline static bool activate() {return true;}
      template<typename Out,Roles role=Roles::Raw>
      inline void print(Out& out) {
        out.raw(value);
        I::template print<Out,role>(out);
      }
      template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
      inline void print(It& it,Nav& nav,Out& out) {
        it.template fmt<Roles::Mode,true ,Nav,Out>(nav,out);
        it.template fmt<Roles::Mode,false ,Nav,Out>(nav,out);
        it.template fmt<Roles::Value,true ,Nav,Out>(nav,out);
        print<Out>(out);
        it.template fmt<Roles::Value,false ,Nav,Out>(nav,out);
      }
      using I::cmd;
      template<Cmds c,typename Nav>
      inline void cmd(Nav& nav) {
        _trace(MDO<<"numField::cmd:"<<c<<endl);
        switch(c) {
          case Cmds::Enter:
            if (nav.mode()==Modes::Tune||tune==0) {
              nav.setMode(Modes::Normal);
              nav.close();
            } else nav.setMode(Modes::Tune);
            return;
          case Cmds::Esc:assert(false);break;
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
  using NumField=StaticWrap<NumValue<T,value,low,high,step,tune>,Label,Unit>;

// };
