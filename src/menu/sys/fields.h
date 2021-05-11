/* -*- C++ -*- */
#pragma once

#include "../item.h"
#ifndef ARDUINO
#include <cstring>
#endif
namespace Menu {

  //Value, for enumeration
  template<typename T,T val,bool def=false>
  struct Value {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      T value() {return val;}
      T get() const {return val;}
      static bool valueIs(T& v) {return val==v;}
    };
  };
  
  //enumerated field
  //as a regular field but holding a list of possible values
  template<typename V>
  struct EnumField {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      using Values=V;//Value type
      Values values;//values enumeration
      //set target value from navigation index
      template<typename Nav> 
      void enumSet(Nav& nav) {
        auto sel=PathRef(1,nav.at());
        using Target=typename Base::TargetType;
        Target selVal=sel.template walk<Values,GetValue>(values);
        Base::set(selVal);
      }
    };
  };

  //enumation, opens enumerated values lists as sub-menu
  template<typename V>
  struct Choose {
    template<typename O>
    struct Part:EnumField<V>::template Part<O> {
      using Base=typename EnumField<V>::template Part<O>;
      using This=Part<Base>;
      using Base::Base;
      using Values=V;
      template<typename Nav> bool enter(Nav& nav) {
        nav.open(Base::values.template find<ValueIs,0>(Base::get()));
        nav.focus(true);
        return true;
      }
      template<typename Nav,Cmd c> 
      bool cmd(Nav& nav,int code=0) {
        if(c==Cmd::Enter) {
          Base::enumSet(nav);
          nav.focus(false);
          nav.close();
          return false;
        }
        auto r=Base::values.template cmd<Nav,c>(nav,code);
        return true;
      }
      template<typename Nav,typename Out> void onPrintMenuTo(Nav& nav,Out& out,int n) {Base::values.template onPrintMenuTo<Nav,Out>(nav,out,n);}
      template<typename Nav,typename Out> void onPrintTitleTo(Nav& nav,Out& out) {Base::values.template onPrintTitleTo<Nav,Out>(nav,out);}
      template<typename Nav,typename Out> void onPrintBodyTo(Nav& nav,Out& out,Idx selIdx,Idx n)  {Base::values.template onPrintBodyTo<Nav,Out>(nav,out,n);}
      // template<typename Nav,typename Out> void onPrintItemTo(Nav& nav,Out& out {O::obj().printTo(nav,out);}
      constexpr Idx sz() {return Base::values.sz();}
      constexpr Idx _sz() {return Base::values._sz();}
    };
  };

  //enumeration, enters field edit and navigates list of values
  template<typename V>
  struct Select {
    template<typename O>
    struct Part:Chain<ParentDraw::template Part,Choose<V>::template Part>::template To<O> {
      using Base=typename Chain<ParentDraw::template Part,Choose<V>::template Part>::template To<O>;
      using This=Part<Base>;
      using Base::Base;
      template<typename Nav,Cmd c> 
      bool cmd(Nav& nav,int code=0) {
        bool r=Base::template cmd<Nav,c>(nav,code);
        if(c==Cmd::Up||c==Cmd::Down) Base::enumSet(nav);
        return r;
      }
    };
  };

  //enumeration, change value on enter => wrap nav.
  template<typename V,char cursor=':'>
  struct Toggle {
    template<typename O>
    struct Part:Chain<WrapNav::Part,ParentDraw::template Part,Choose<V>::template Part>::template To<O> {
      using Base=typename Chain<WrapNav::Part,ParentDraw::template Part,Choose<V>::template Part>::template To<O>;
      using This=Part<Base>;
      using Base::Base;
      template<typename Nav,typename Out,bool delegate=true>
      void printTo(Nav& nav,Out& o,int n=0,bool sel=false) {
        o.print(cursor);
        if(delegate) Base::printTo(nav,o,n,sel);
      }
      template<typename Nav> bool enter(Nav& nav) {
        Idx selIdx=Base::values.template find<ValueIs,0>(Base::get());
        nav.open(selIdx);
        nav.up();
        Base::enumSet(nav);
        nav.close();
        return false;
      }
    };
  };

  // static binded to a reference
  template<typename T,T& target,T def>
  struct FieldRef {
    template<typename O>
    struct Part:O {
      using TargetType=T;
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      T reflex;
      template<typename... OO>
      Part(OO... oo):reflex(def),Base(oo...) {}
      bool changed() {return reflex!=target;}
      void changed(bool o) {if(o) reflex^=1; else reflex=target;}
      void set(T o) const {target=o;}
      T get() const {return target;}
      template<typename Nav,typename Out,bool delegate=true>
      void printTo(Nav& nav,Out& o,int n=0,bool sel=false) {
        o.print(get());
        if(delegate)Base::printTo(nav,o,n,sel);
      }
    };
  };

  //storing data value
  template<typename T,T def>
  struct FieldVal {
    template<typename O=Empty<Nil>>
    struct Part:O {
      using TargetType=T;
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      T value;
      T dirty=true;
      template<typename... OO>
      Part(OO... oo):value(def),Base(oo...) {}
      bool changed() {return dirty;}
      void changed(bool o) {dirty=o;}
      void set(T o) {value=o;}
      T get() const {return value;}
      template<typename Nav,typename Out,bool delegate=true>
      void printTo(Nav& nav,Out& o,int n,bool sel) {
        o.print(get());
        if(delegate)Base::printTo(nav,o,n,sel);
      }
    };
  };

  //Static range (low and high values)
  template<typename T,T lowVal,T highVal>
  struct StaticRange {
    template<typename O=Empty<Nil>>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      constexpr T low() const {return lowVal;}
      constexpr T high() const {return highVal;}
      void constrainField() {Base::obj().set(constrain(Base::obj().get(), low(), high()));}
    };
  };

  //numeric range validation and change deltas (step and tune)
  template<typename T,T lowVal,T highVal,T stepSz,T tuneSz,T def=0>
  struct StaticSTRange {
    template<typename O=Empty<Nil>>
    struct Part:StaticRange<T,lowVal,highVal>::template Part<O> {
      using Base=typename StaticRange<T,lowVal,highVal>::template Part<O>;
      using This=Part<Base>;
      using Base::Base;
      constexpr T tune() const {return tuneSz;}
      constexpr T step() const {return stepSz;}
    };
  };

  //set nav status for field edit/tune
  struct EditCtrl {
    template<typename O=Empty<Nil>>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      template<typename Nav>
      bool enter(Nav& nav) {
        if(nav.focus()) {
          nav.tune(nav.tune()^Base::tune());
          nav.focus(nav.tune());
        } else nav.focus(true);
        return true;
      }
    };
  };

  //validate numeric field value by range info
  struct RangeValid {
    template<typename O=Empty<Nil>>
    struct Part:EditCtrl::Part<O> {
      using Base=typename EditCtrl::Part<O>;
      using This=Part<Base>;
      using Base::Base;
      bool wrap=false;//TODO: use static style instead
      bool canWrap() {return wrap;}
      template<typename Nav,typename T>
      void stepit(Nav& nav,int dir) {
        // dir*=options->invertFieldKeys?-1:1;
        T thisstep = nav.tune()?Base::tune():Base::step();
        Base::changed(true);
        //by default they are inverted.. now buttons and joystick have to flip them
        if (dir > 0) {
          if ((Base::high()-Base::target()) < thisstep)
            Base::set(canWrap()?Base::low():Base::high());
          else
            Base::target() += thisstep;
        } else {
          if ((Base::target()-Base::low()) < thisstep)
            Base::target() = canWrap()?Base::high():Base::low();
          else
            Base::target() -= thisstep;
        }
      }
      template<typename Nav>
      bool up(Nav& nav) {
        if(nav.focus()) {
          Base::set(Base::get()+(nav.tune()?Base::tune():Base::step()));
          return true;
        }
        return false;
      }
      template<typename Nav>
      bool down(Nav& nav) {
        if(nav.focus()) {
          Base::set(Base::get()-(nav.tune()?Base::tune():Base::step()));
          return true;
        }
        return false;
      }
    };
  };

  //static bind to numeric reference
  template<typename T,T& target,T def=0>
  struct NumRef {
    template<typename O=Empty<Nil>>
    struct Part:Chain<RangeValid::Part,FieldRef<T,target,def>::template Part>::template To<O> {
      using Base=typename Chain<RangeValid::Part,FieldRef<T,target,def>::template Part>::template To<O>;
      using This=Part<Base>;
      using Base::Base;
      constexpr static bool valueIs(T& v) {return target==v;}
    };
  };

  //storing numeric value
  template<typename T,T def=0>
  struct NumVal {
    template<typename O=Empty<Nil>>
    struct Part:Chain<RangeValid::Part,FieldVal<T,def>::template Part>::template To<O> {
      using Base=typename Chain<RangeValid::Part,FieldVal<T,def>::template Part>::template To<O>;
      using This=Part<Base>;
      using Base::Base;
    };
  };

  //edit a text buffer
  template<char* buffer,Idx buffSz,ConstText masks[],char def=' '>
  struct TextEditBase {
    template<typename O=Empty<>>
    struct Part:IsMenu::Part<ParentDraw::Part<O>> {
      using Base=IsMenu::Part<ParentDraw::Part<O>>;
      using This=Part<O>;
      using Base::Base;
      static constexpr Idx sz() {return buffSz-1;}
      template<typename Nav> bool enter(Nav& nav) {
        // _trace(mdo<<"TextEditBase::enter\n\r");
        nav.open();
        nav.focus(true);
        return false;}
      ConstText validator(int i) 
        {return masks[i%(sizeof(masks)/sizeof(masks[0]))];}
      template<typename Nav,Cmd c> 
      bool cmd(Nav& nav,int code=0) {
        // _trace(
        //   mdo<<"TextEditBase::cmd<"<<c<<">"
        //   <<" code:"<<code
        //   <<" focus:"<<nav.focus()
        //   <<" tune:"<<nav.tune()
        //   <<"\n\r"
        // );
        switch(c) {
          //from AM4
          case Cmd::Enter:
            if(nav.tune()&&!nav.edited()){
              nav.focus(false);
              nav.tune(false);
              nav.close();
              return false;
            } else {
              nav.tune(!nav.tune());
              if(nav.tune()) nav.edited(false);
              return false;
            }
          case Cmd::Esc:
            nav.tune(false);
            nav.focus(false);
            nav.close();
            return false;
          case Cmd::Up:
            if (nav.tune()) {
              const char* v=validator(nav.pos());
              const char *at=strchr(v,buffer[nav.pos()]);
              int pos=at?at-v+1:1;
              if (pos>=(Idx)strlen(v)) pos=0;
              buffer[nav.pos()]=v[pos];
              nav.edited(true);
              return true;
            }
            break;
          case Cmd::Down:
            if (nav.tune()) {
              const char* v=validator(nav.pos());
              const char *at=strchr(v,buffer[nav.pos()]);
              int pos=at?at-v-1:0;
              if (pos<0) pos=strlen(v)-1;
              buffer[nav.pos()]=v[pos];
              nav.edited(true);
              return true;
            }
            break;
          default:break;
        }
        return nav.template doNav<c>(sz(),Base::styles());//generic menu nav
      }
      template<typename Nav,typename Out,bool delegate=true>
      void printTo(Nav& nav,Out& out,int n=0,bool sel=false) {
        if(sel/*&&editing*/) {//if we are here then we are editing
          if(nav.pos()) out.write(buffer,nav.pos());
          out.template fmtStart<Fmt::EditCursor>(nav.focus(),nav.tune());
          out.print((char)buffer[nav.pos()]);
          out.template fmtStop<Fmt::EditCursor>(nav.focus(),nav.tune());
          if(nav.pos()<sz()-1) out.print(buffer+nav.pos()+1);
        } else out.print(buffer);
        if(delegate) Base::printTo(nav,out,n,sel);
      }
      template<typename Nav,typename Out> void onPrintMenuTo(Nav& nav,Out& out,int)          {/*_trace(mdo<<"onPrintMenuTo\n\r");*/}
      template<typename Nav,typename Out> void onPrintTitleTo(Nav& nav,Out& out)         {/*_trace(mdo<<"onPrintTitleTo\n\r");*/}
      template<typename Nav,typename Out> void onPrintBodyTo(Nav& nav,Out& out,Idx selIdx,Idx n)           {/*_trace(mdo<<"onPrintBodyTo\n\r");*/}
      template<typename Nav,typename Out> void onPrintItemTo(Nav& nav,Out& out,int n,bool sel) {
        O::obj().template printTo<Nav,Out>(nav,out,n,sel);
      }
    };
  };

  template<char* buffer,Idx len,ConstText mask[],char def=' '>
  using TextEdit=Chain<
    AsEditMode::Part,
    AsValue::Part,
    TextEditBase<buffer,len,mask,def>::template Part
  >;

};//Menu namespace