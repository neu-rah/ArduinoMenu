/* -*- C++ -*- */
#pragma once

/********************************
numeric and enumerated fields and field components
*/

#include "kernel/list.h"

namespace Menu {

  //targetting a user code variable (reference)
  template<typename T,T& value,bool loop=false>
  struct StaticField {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::Base;
      using ValueType=T;
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level) {
        out.raw(value);
        if(delegate) Base::template print<Nav,Out,op>(nav,out,level);
      }
      inline static void setValue(T v) {
        trace(MDO<<"StaticField::setValue to:"<<v<<endl);
        value=v;
        Base::setValue(v);
      }
      inline static void setValue(T& v) {
        trace(MDO<<"StaticField::setValue to:&"<<v<<endl);
        value=v;
        Base::setValue(v);
      }
    protected:
      T shadow;
    };
  };

  //numeric range validated field
  template<typename T,T& value,T low,T high,T step,T tune,bool loop=false>
  struct StaticNumFieldCore {
    template<typename I>
    struct Part:StaticField<T,value,loop>::template Part<I> {
      using Base=typename StaticField<T,value,loop>::template Part<I>;
      using Base::Base;
      inline static bool stepVal(T delta) {
        if (delta==0) return false;
        if (delta > 0) {
          if (high-value>=delta) value+=delta;
          else value=loop?low:high;
        } else {
          if (value-low>=-delta) value+=delta;
          else value=loop?high:low;
        }
        return true;
      }

      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux) {
        trace(MDO<<"Field::cmd "<<c<<" mode:"<<nav.mode()<<endl);
        if(nav.mode()==Mode::Normal&&c!=Cmd::Enter) return Base::template cmd<c,Nav>(nav,level,aux);
        switch(c) {
          case Cmd::Enter:
            switch(nav.mode()) {
              case Mode::Normal: nav.setMode(Mode::Edit);break;
              case Mode::Edit:nav.setMode(tune==0?Mode::Normal:Mode::Tune);break;
              case Mode::Tune: nav.setMode(Mode::Normal);break;
              default: return false;
            }
            return true;
          case Cmd::Esc:
            nav.setMode(Mode::Normal);
            return false;
          case Cmd::Up: stepVal(-(nav.mode()==Mode::Edit?step:tune));break;
          case Cmd::Down: stepVal(nav.mode()==Mode::Edit?step:tune);break;
          default:return false;
        }
        return true;
      }

    protected:
      T shadow;
    };
  };

  template<Expr Title,typename T,T& value,T lo,T hi,T step,T tune,Expr Unit=Empty>
  struct StaticNumField {
    template<typename I>
    struct Part:Chain<
        AsName<Title>::template Part,//(As) name format apply only to inner content
        WrapMode<>::template Part,//(Wrap) mode format, starts here and gores to end of remaining content
        StaticNumFieldCore<T,value,lo,hi,step,tune>::template Part,//the numeric field
        AsUnit<Unit>::template Part,//name format apply only to inner content
        Mutable::template Part//track changes
      >::template To<I>
    {
      using Base=typename Chain<
          AsName<Title>::template Part,//(As) name format apply only to inner content
          WrapMode<>::template Part,//(Wrap) mode format, starts here and gores to end of remaining content
          StaticNumFieldCore<T,value,lo,hi,step,tune>::template Part,//the numeric field
          AsUnit<Unit>::template Part,//name format apply only to inner content
          Mutable::template Part//track changes
        >::template To<I>;
        using Base::Base;
    };
  };

  //call action on navigation focus and set the value as if activated
  //(but does not change nav/activation state)
  struct ActOnUpdate {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux,Idx n) {
        trace(MDO<<"ActOnUpdate::cmd "<<c<<endl);
        if(c!=Cmd::Enter&&c!=Cmd::Esc&&Base::obj().isActive()) {
          if(Base::template cmd<c,Nav>(nav,level,aux,n)) {
            trace(MDO<<"ActOnUpdate::cmd level:"<<level<<" focus:"<<nav.focus()<<endl);
            trace(MDO<<"updating value to:"<<Base::obj().template getValue<typename Base::ValueType>(nav.focus())<<endl);
            Base::obj().setValue(Base::obj().template getValue<typename Base::ValueType>(nav.focus()));
            Base::obj().keepSel(nav.focus());
            Base::obj().action(nav,level);
            return true;
          }
          return false;
        } else return Base::template cmd<c,Nav>(nav,level,aux,n);
      }
    };
  };

  //recall selected/active index
  struct KeepSel {
    template<typename I>
    struct Part:I {
      Idx sel=0;
      using Base=I;
      using Base::Base;
      using Base::activate;
      inline Idx focusIndex() const {return sel;}
      inline void keepSel(Idx n) {sel=n;}
      template<typename Nav>
      inline void activate(Nav& nav,Idx level) {
        trace(MDO<<"{KeepSel} User selection was #"<<sel<<" path"<<nav.operator PathRef()<<endl);
        Base::activate(nav,level);
        nav.setPath(nav.depth(),sel);
        trace(MDO<<"{KeepSel} retored selection of level "<<nav.depth()<<" to "<<nav.operator PathRef()<<endl);
      }
      template<typename Nav>
      inline void activate(Nav& nav,Idx level,Idx n) {
        trace(MDO<<"{KeepSel} User selects #"<<n<<endl);
        sel=n;
        Base::activate(nav,level,n);
      }
      template<typename Nav>
      inline bool action(Nav& nav,Idx level) {
        trace(MDO<<"{KeepSel} action storing:"<<nav.focus()<<endl);
        sel=nav.focus();
        return Base::action(nav,level);
      }
    };
  };

  //store an enumerated value and optionally a nice text
  //this is a unit of enumeration
  template<typename V,V value>
  struct EnumValue {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::getValue;
      template<typename T>
      inline static constexpr T getValue() {return value;}
    };
  };

  //TODO: this is not yet calling the action on selection (when not using ActOnUpdate)
  //enumerated field, base for Toggle, Select and Choose
  template<typename T,T& value,typename Title,typename Ops,bool loop=true>
  struct EnumField {
    template<typename I>
    struct Part:Chain<IsActive::Part,KeepSel::Part,StaticMenu<Title,Ops>::template Part,StaticField<T,value,loop>::template Part>::template To<I> {
      using Base=typename Chain<IsActive::Part,KeepSel::Part,StaticMenu<Title,Ops>::template Part,StaticField<T,value,loop>::template Part>::template To<I>;
      using Base::Base;
      using Base::print;
      //TODO: arduinoi example still does not print well.. check changes
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level) {
        Base::template print<Nav,Out,op>(nav,out,level);
        Base::template print
          <Nav,Out,op,delegate>
          (nav,out,level,Base::obj().isActive()?nav.focus():Base::obj().focusIndex());
      }
    };
  };

  //TODO: coordinate edit cursor
  //set field value by a list of possible values (enumeration)
  //enter activates edit mode, nav keys select the value
  template<typename T,T& value,typename Title,typename Ops>
  struct SelectField {
    template<typename I>
    struct Part:Chain<ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I> {
      using Base=typename Chain<ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I>;
      using Base::Base;
    };
  };

  //TODO: this still needs a wrap loop and toggle on activation
  //enter toggles selected value from enumeration list
  //this field will always perform the value activation
  template<typename T,T& value,typename Title,typename Ops>
  struct ToggleField {
    template<typename I>
    struct Part:Chain<ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I> {
      using Base=typename Chain<ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I>;
      using Base::Base;
    };
  };

};
