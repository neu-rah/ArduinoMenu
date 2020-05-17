/* -*- C++ -*- */
#pragma once

/********************************
numeric and enumerated fields and field components
*/

#include "kernel/list.h"

namespace Menu {

  //statically targetting a user code variable (reference)
  template<typename T,T& value,bool loop=false>
  struct StaticField {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::Base;
      using ValueType=T;
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level,bool selected) {
        //TODO:: coordinate edit cursor here
        // MUST remove the composition cursor `WrapMode<>::template Part`
        // and do a proper check for active here
        // either nby tracking active or by receiving item index
        // receiving item index is more powerfull, however it requires split of ALL walker callees

        out.template fmt<Tag::Mode,true>(0,selected,Base::obj().enabled(),nav.mode());
        out.raw(value);
        if(delegate) Base::template print<Nav,Out,op>(nav,out,level,selected);
        out.template fmt<Tag::Mode,false>(0,selected,Base::obj().enabled(),nav.mode());
      }
      inline static T getValue() {return value;}
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

  //targetting a user code variable (reference)
  template<typename T,bool loop=false>
  struct Field {
    template<typename I>
    struct Part:I {
      using Base=I;
      // using Base::Base;
      using ValueType=T;
      template<typename... OO>
      Part(T& target,OO... oo):Base(oo...),value(target) {}
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level,bool selected) {
        out.raw(value);
        if(delegate) Base::template print<Nav,Out,op>(nav,out,level);
      }
      inline T getValue() const {return value;}
      inline void setValue(T v) {
        trace(MDO<<"Field::setValue to:"<<v<<endl);
        value=v;
        Base::setValue(v);
      }
      inline void setValue(T& v) {
        trace(MDO<<"Field::setValue to:&"<<v<<endl);
        value=v;
        Base::setValue(v);
      }
    protected:
      T& value;
      T shadow;
    };
  };

  template<typename T,typename I,bool loop=false>
  struct NumFieldAbs:I {
    using Base=I;
    using Base::Base;
    using Base::cmd;
    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,Idx level,Idx aux) {
      trace(MDO<<"Field::cmd "<<c<<" mode:"<<nav.mode()<<endl);
      if(nav.mode()==Mode::Normal&&c!=Cmd::Enter) return Base::template cmd<c,Nav>(nav,level,aux);
      switch(c) {
        case Cmd::Enter:
          switch(nav.mode()) {
            case Mode::Normal: nav.setMode(Mode::Edit);break;
            case Mode::Edit:nav.setMode(Base::obj().getTune()==0?Mode::Normal:Mode::Tune);break;
            case Mode::Tune: nav.setMode(Mode::Normal);break;
            default: return false;
          }
          return true;
        case Cmd::Esc:
          nav.setMode(Mode::Normal);
          return false;
        case Cmd::Up:
          stepVal(-(nav.mode()==Mode::Edit?Base::obj().getStep():Base::obj().getTune()));
          break;
        case Cmd::Down:
          stepVal(nav.mode()==Mode::Edit?Base::obj().getStep():Base::obj().getTune());
          break;
        default:return false;
      }
      return true;
    }
    inline bool stepVal(T delta) {
      if (delta==0) return false;
      if (delta > 0) {
        if (Base::obj().getHigh()-Base::obj().getValue()>=delta) Base::obj().setValue(Base::obj().getValue()+delta);
        else Base::obj().setValue(loop?Base::obj().getLow():Base::obj().getHigh());
      } else {
        if (Base::obj().getValue()-Base::obj().getLow()>=-delta) Base::obj().setValue(Base::obj().getValue()+delta);
        else Base::obj().setValue(loop?Base::obj().getHigh():Base::obj().getLow());
      }
      return true;
    }
  };

  //static numeric range validated field
  template<typename T,T& value,T low,T high,T step,T tune,bool loop=false>
  struct StaticNumFieldCore {
    template<typename I>
    struct Part:StaticField<T,value,loop>::template Part<NumFieldAbs<T,I,loop>> {
      using Base=typename StaticField<T,value,loop>::template Part<NumFieldAbs<T,I,loop>>;
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
      inline static T getStep() {return step;}
      inline static T getTune() {return tune;}
      inline static T getLow() {return low;}
      inline static T getHigh() {return high;}
    protected:
      T shadow;
    };
  };

  //static numeric range validated field
  template<typename T,bool loop=false>
  struct NumFieldCore {
    template<typename I>
    struct Part:Field<T,loop>::template Part<NumFieldAbs<T,I,loop>> {
      using Base=typename Field<T,loop>::template Part<NumFieldAbs<T,I,loop>>;
      // using Base::Base;
      template<typename... OO>
      Part(T& value,T low,T high,T step,T tune,OO... oo)
        :Base(value,oo...),low(low),high(high),step(step),tune(tune) {}
      inline T getStep() const {return step;}
      inline T getTune() const {return tune;}
      inline T getLow() const {return low;}
      inline T getHigh() const {return high;}

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
          case Cmd::Up: Base::obj().stepVal(-(nav.mode()==Mode::Edit?step:tune));break;
          case Cmd::Down: Base::obj().stepVal(nav.mode()==Mode::Edit?step:tune);break;
          default:return false;
        }
        return true;
      }

    protected:
      T low,high,step,tune;
      T shadow;
    };
  };

  template<Expr Title,typename T,T& value,T lo,T hi,T step,T tune,bool loop=false,Expr Unit=Empty>
  using StaticNumField=Alias<
    AsName<Title>::template Part,//(As) name format apply only to inner content
    // WrapMode<>::template Part,//(Wrap) mode format, starts here and gores to end of remaining content
    StaticNumFieldCore<T,value,lo,hi,step,tune,loop>::template Part,//the numeric field
    AsUnit<Unit>::template Part,//name format apply only to inner content
    Mutable::template Part//track changes
  >;

  template<Expr Title,typename T,bool loop=false,Expr Unit=Empty>
  using NumField=Alias<
    AsName<Title>::template Part,//(As) name format apply only to inner content
    // WrapMode<>::template Part,//(Wrap) mode format, starts here and gores to end of remaining content
    NumFieldCore<T,loop>::template Part,//the numeric field
    AsUnit<Unit>::template Part,//name format apply only to inner content
    Mutable::template Part//track changes
  >;

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

  template<typename Title,typename Body>
  using StaticEnum=Alias<
    CmdChange::Part,
    SetWalker::Part,
    EnumNav::Part,
    StaticMenuCore<Title,Body>::template Part
  >;

  //enumerated field, base for Toggle, Select and Choose
  template<typename T,T& value,typename Title,typename Ops,bool loop=true>
  struct EnumField {
    template<typename I>
    struct Part:Chain<IsActive::Part,KeepSel::Part,StaticEnum<Title,Ops>::template Part,StaticField<T,value,loop>::template Part>::template To<I> {
      using Base=typename Chain<IsActive::Part,KeepSel::Part,StaticEnum<Title,Ops>::template Part,StaticField<T,value,loop>::template Part>::template To<I>;
      using Base::Base;
      using Base::print;
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level,bool selected) {
        constexpr bool toPrint=op==Op::Printing;
        Base::template print<Nav,Out,op>(nav,out,level,selected);
        out.template fmt<Tag::Mode,true,toPrint>(0,selected,true,nav.mode());
        out.template fmt<Tag::Value,true,toPrint>();
        Base::template printItem
          <Nav,Out,op,delegate>
          (nav,out,level,Base::obj().isActive()?nav.pos():Base::obj().storedSel());
        out.template fmt<Tag::Value,false,toPrint>();
        out.template fmt<Tag::Mode,false,toPrint>(0,selected,true,nav.mode());
        //never delegate, it will print the label again!
        // if(delegate) Base::template print<Nav,Out,op,delegate>(nav,out,level);
      }
    };
  };

  //enter toggles selected value from enumeration list
  template<typename T,T& value,typename Title,typename Ops>
  struct ToggleField {
    template<typename I>
    struct Part:Chain<ToggleNav::Part,DefaultNav::Part,ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I> {
      using Base=typename Chain<ToggleNav::Part,DefaultNav::Part,ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I>;
      using Base::Base;
      using Base::activate;
    };
  };

  //set field value by a list of possible values (enumeration)
  //enter activates edit mode, nav keys select the value (inplace)
  template<typename T,T& value,typename Title,typename Ops>
  struct SelectField {
    template<typename I>
    struct Part:Chain<ActOnSub::Part,ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I> {
      using Base=typename Chain<ActOnSub::Part,ParentPrint::template Part,EnumField<T,value,Title,Ops>::template Part>::template To<I>;
      using Base::Base;
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        auto r=Base::template cmdItem<c,Nav>(nav,level,aux,n);
        nav.setMode(Base::obj().isActive()?Mode::Edit:Mode::Normal);
        return r;
      }
      using Base::activate;
      template<typename Nav>
      inline void activate(Nav& nav,Idx level) {
        nav.setMode(Mode::Edit);
        Base::activate(nav,level);
        _trace(MDO<<"SelectField::activate "<<nav.mode()<<endl);
      }
    };
  };

  //opens enumeration of alternative values as submenu
  template<typename T,T& value,typename Title,typename Ops>
  struct ChooseField {
    template<typename I>
    struct Part:Chain<ActOnSub::Part,EnumField<T,value,Title,Ops>::template Part>::template To<I> {
      using Base=typename Chain<ActOnSub::Part,EnumField<T,value,Title,Ops>::template Part>::template To<I>;
      using Base::Base;
    };
  };

};
