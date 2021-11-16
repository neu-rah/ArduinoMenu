/* -*- C++ -*- */
#pragma once

#include "../item.h"
#ifndef ARDUINO
#include <cstring>
#endif
namespace Menu
{

  //Value, for enumeration
  template <typename T, T val, bool isDef = false>
  struct Value
  {
    template <typename O>
    struct Part : O
    {
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      using ValueType = T;
      inline static constexpr T value() { return val; }
      inline static constexpr T get() { return val; }
      static bool valueIs(T &v) { return val == v; }
      static constexpr bool isDefault() { return isDef; }
    };
  };

  //enumerated field
  //as a regular field but holding a list of possible values
  template <typename V>
  struct EnumField
  {
    template <typename O>
    struct Part : O
    {
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      using Values = V; //Value type
      Values values;    //values enumeration
      //set target value from navigation index
      void enumSet(Idx i)
      {
        // _trace(clog<<"EnumField::enumSet idx:"<<i<<endl;clog.flush());
        auto sel = PathRef(1, &i);
        using Target = typename Base::ValueType;
        Target selVal = sel.template walk<Values, GetValue>(values);
        Base::set(selVal);
      }
      template <typename Nav>
      void enumSet(Nav &nav)
      {
        // _trace(clog<<"EnumField::enumSet<Nav>"<<nav.at()[0]<<endl);
        enumSet(nav.at()[0]);
      }
    };
  };

  //enumeration, opens enumerated values lists as sub-menu
  template <typename V>
  struct Choose
  {
    template <typename O>
    struct Part : EnumField<V>::template Part<O>
    {
      using Base = typename EnumField<V>::template Part<O>;
      using This = Part<Base>;
      using Base::Base;
      using Values = V;
      using ValueType = V;
      template <typename Nav>
      bool enter(Nav &nav)
      {
        nav.open(Base::values.template find<ValueIs, 0>(Base::get()));
        nav.focus(true);
        return true;
      }
      template <typename Nav, Cmd c>
      bool cmd(Nav &nav, int code = 0)
      {
        if (c == Cmd::Enter)
        {
          // _trace(clog<<"Choose::Cmd<Nav,Enter>"<<endl;clog.flush(););
          Base::enumSet(nav);
          nav.focus(false);
          nav.close();
          return false;
        }
        // auto r=
        Base::values.template cmd<Nav, c>(nav, code);
        return true;
      }
      template <typename Nav, typename Out>
      void onPrintMenuTo(Nav &nav, Out &out,Idx level, int n) { Base::values.template onPrintMenuTo<Nav, Out>(nav, out, level, n); }
      template <typename Nav, typename Out>
      void onPrintTitleTo(Nav &nav, Out &out) { Base::values.template onPrintTitleTo<Nav, Out>(nav, out); }
      template <typename Nav, typename Out>
      void onPrintBodyTo(Nav &nav, Out &out,Idx level, Idx selIdx, Idx n) { Base::values.template onPrintBodyTo<Nav, Out>(nav, out,level, n); }
      // template<typename Nav,typename Out> void onPrintItemTo(Nav& nav,Out& out,Idx level {O::obj().printTo(nav,out);}
      // void setDefault() {
      //   Idx selPath[1]{0};
      //   if(selPath[0]>=Base::values.len()) setDefault();
      // }
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &o, int n = 0, bool sel = false)
      {
        // _trace(clog<<"Choose::printTo"<<endl;clog.flush());
        //TODO:: use & instead of array!
        Idx selPath[1]{0};
        selPath[0] = Base::values.template find<ValueIs, 0>(Base::get());
        // _trace(clog
        //   <<"selPath[0]:"<<selPath[0]
        //   <<">= sz:"<<Base::values.len()
        //   <<"? "<<(selPath[0]>=Base::values.len()?"yes":"no")
        //   <<endl;clog.flush();
        // );
        if (selPath[0] >= Base::values.len())
        {
          // _trace(clog<<"out of range data"<<endl);
          selPath[0] = Base::values.template find<IsDefault, 0>();
          // _trace(clog<<"found default idx:"<<selPath[0]<<endl);
          if (selPath[0] >= Base::values.len())
            return;
          // _trace(clog<<"setting enum"<<endl);
          Base::enumSet(selPath[0]);
        }
        PathRef(1, selPath).template walk<decltype(Base::values), PrintTo, Nav &, Out &>(Base::values, nav, o, n, sel);
        // if(delegate) Base::printTo(nav,o,n,sel);
      }
      constexpr Idx sz() { return Base::values.sz(); }
      constexpr Idx _sz() { return Base::values._sz(); }
    };
  };

  //enumeration, enters field edit and navigates list of values
  template <typename V>
  struct Select
  {
    template <typename O>
    struct Part : Chain<ParentDraw::template Part, Choose<V>::template Part>::template To<O>
    {
      using Base = typename Chain<ParentDraw::template Part, Choose<V>::template Part>::template To<O>;
      using This = Part<Base>;
      using Base::Base;
      template <typename Nav, Cmd c>
      bool cmd(Nav &nav, int code = 0)
      {
        bool r = Base::template cmd<Nav, c>(nav, code);
        if (c == Cmd::Up || c == Cmd::Down)
          Base::enumSet(nav);
        return r;
      }
    };
  };

  //enumeration, change value on enter => wrap nav.
  template <typename V, char cursor = ':'>
  struct Toggle
  {
    template <typename O>
    struct Part : Chain<WrapNav::Part, ParentDraw::template Part, Choose<V>::template Part>::template To<O>
    {
      using Base = typename Chain<WrapNav::Part, ParentDraw::template Part, Choose<V>::template Part>::template To<O>;
      using This = Part<Base>;
      using Base::Base;
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &o, int n = 0, bool sel = false)
      {
        // _trace(clog<<"Toggle::printTo"<<endl;clog.flush());
        o.print(cursor);
        Base::printTo(nav, o, n, sel);
        // if(delegate) Base::printTo(nav,o,n,sel);
      }
      template <typename Nav>
      bool enter(Nav &nav)
      {
        Idx selIdx = Base::values.template find<ValueIs, 0>(Base::get());
        nav.open(selIdx);
        nav.up();
        Base::enumSet(nav);
        nav.close();
        return false;
      }
    };
  };

  // static binded to a reference
  template <typename T, T &target, T def>
  struct FieldRef
  {
    template <typename O>
    struct Part : O
    {
      using ValueType = T;
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      T reflex;
      template <typename... OO>
      Part(OO... oo) : reflex(def), Base(oo...) {}
      void setDefault() { reflex = target = def; }
      bool changed() { return reflex != target; }
      void changed(bool o) {
        if (o)
          reflex ^= 1;
        else
          reflex = target;
      }
      void set(T o) {
        target = o;
        Base::obj().action();
      }
      T get() const {return target;}
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &o, int n = 0, bool sel = false)
      {
        // _trace(clog<<"FieldRef::printTo"<<endl;clog.flush());
        o.print(get());
        if (delegate)
          Base::printTo(nav, o, n, sel);
      }
    };
  };

  //functional field, no data or reference stored, other than the change checking data ofcoz
  template<typename T,T(&fget)(),void(&fset)(T),T def>
  struct FuncField {
    template<typename O>
    struct Part:O {
      using ValueType = T;
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      T reflex;
      template <typename... OO>
      Part(OO... oo) : reflex(def), Base(oo...) {}
      void setDefault() { reflex = def; fset(def);}
      bool changed() { return reflex != fget(); }
      void changed(bool o) {
        if (o)
          reflex ^= 1;
        else
          reflex = fget();
      }
      void set(T o) {
        reflex = o;
        fset(o);
        Base::obj().action();
      }
      inline T get() const {return fget();}
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &o, int n = 0, bool sel = false)
      {
        o.print(get());
        if (delegate)
          Base::printTo(nav, o, n, sel);
      }
    };
  };

  // template<typename T,T(&fget)(),T(&fset)(T),T def>
  // struct FuncField:FuncFieldBase<T,fget,fset,def> {};
  // template<typename T,T(&fget)(),T def>
  // struct FuncField<T,fget,fget,def>:FuncFieldBase<T,fget,fget,def> {};

  template <typename T, T def>
  struct FieldValBase
  {
    template <typename O = Empty<Nil>>
    struct Part : O
    {
      using ValueType = T;
      using Base = O;
      using This = Part<O>;
      using Base::Base;
      Part(const This &) = delete;
      Part &operator=(const This &) = delete;
      T value;
      T dirty = true;
      template <typename... OO>
      Part(OO... oo) : value(def), Base(oo...) {}
      void setDefault() {
        dirty |= value == def;
        value = def;
      }
      bool changed() { return dirty; }
      void changed(bool o) { dirty = o; }
      void set(T o) {
        value = o;
        Base::obj().action();
      }
      T get() const { return value; }
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &o, int n, bool sel) {
        o.print(get());
        if (delegate)
          Base::printTo(nav, o, n, sel);
      }
    };
  };

  //storing data value
  template <typename T, T def>
  struct FieldVal {
    template <typename O = Empty<Nil>>
    struct Part : FieldValBase<T,def>::template Part<O> {
      using ValueType = T;
      using Base = typename FieldValBase<T,def>::template Part<O>;
      using This = Part<O>;
      using Base::Base;
    };
  };

  template <uint8_t def>
  struct FieldVal<uint8_t,def> {
    template <typename O = Empty<Nil>>
    struct Part : FieldValBase<uint8_t,def>::template Part<O> {
      using ValueType = uint8_t;
      using Base = typename FieldValBase<uint8_t,def>::template Part<O>;
      using This = Part<O>;
      using Base::Base;
      void changed(bool o) {}
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &o, int n, bool sel) {
        o.print((int)Base::get());
        if (delegate)
          Base::Base::printTo(nav, o, n, sel);
      }
    };
  };

  // template<typename T,T def,uint16_t id=0>
  // struct StaticFieldVal {
  //   template<typename O=Empty<Nil>>
  //   struct Part:O {
  //     using ValueType=T;
  //     using Base=O;
  //     using This=Part<Base>;
  //     using Base::Base;
  //     static T value;
  //     static T dirty;
  //     // template<typename... OO>
  //     // Part(OO... oo):value(def),Base(oo...) {}
  //     static bool changed() {return dirty;}
  //     static void changed(bool o) {dirty=o;}
  //     static void set(T o) {
  //       dirty=o!=value;
  //       value=o;
  //       // Base::obj().action();//static fields can not have associated action
  //       //unless we make a Static action... and eventually a static enable/disable
  //     }
  //     static T get() {return value;}
  //     template<typename Nav,typename Out,bool delegate=true>
  //     void printTo(Nav& nav,Out& o,int n,bool sel) {
  //       o.print(get());
  //       if(delegate)Base::printTo(nav,o,n,sel);
  //     }
  //   };
  // };


  //Static range (low and high values)
  template <typename T, T lowVal, T highVal>
  struct StaticRange
  {
    template <typename O = Empty<Nil>>
    struct Part : O
    {
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      static constexpr T low() { return lowVal; }
      static constexpr T high() { return highVal; }
      static void constrainField() { Base::obj().set(constrain(Base::obj().get(), low(), high())); }
    };
  };

  //numeric range validation and change deltas (step and tune)
  template <typename T, T lowVal, T highVal, T stepSz, T tuneSz, T def = 0>
  struct StaticSTRange
  {
    template <typename O = Empty<Nil>>
    struct Part : StaticRange<T, lowVal, highVal>::template Part<O>
    {
      using Base = typename StaticRange<T, lowVal, highVal>::template Part<O>;
      using This = Part<Base>;
      using Base::Base;
      static constexpr T tune() { return tuneSz; }
      static constexpr T step() { return stepSz; }
    };
  };

  //numeric range
  template<typename T>
  struct Range {
    template<typename O>
    struct Part:O {
      using This=Part<O>;
      using Base=O;
      using Base::Base;
      T lowVal,highVal;
      template<typename... OO>
      Part(T low,T high,OO... oo):lowVal(low),highVal(high),Base(oo...) {}
      constexpr T low() const { return lowVal; }
      constexpr T high() const { return highVal; }
      void constrainField() { Base::obj().set(constrain(Base::obj().get(), low(), high())); }
    };
  };

  //numeric range with step/tune change values
  template<typename T>
  struct STRange {
    template<typename O>
    struct Part:Range<T>::template Part<O> {
      using This=Part<O>;
      using Base=typename Range<T>::template Part<O>;
      using Base::Base;
      T stepSz,tuneSz;
      template<typename... OO>
      Part(T low,T high,T step, T tune,T def,OO... oo):stepSz(step),tuneSz(tune),Base(low,high,oo...) {}
      constexpr T tune() const { return tuneSz; }
      constexpr T step() const { return stepSz; }
    };
  };

  //TODO separate default values from this structures
  template <typename T, T lowVal, T highVal, T stepSz, T tuneSz, T def = 0>
  struct SCSTRange {
    template<typename O>
    struct Part:STRange<T>::template Part<O> {
      using This=Part<O>;
      using Base=typename STRange<T>::template Part<O>;
      template<typename... OO>
      Part(OO... oo):Base(lowVal,highVal,stepSz,tuneSz,def,oo...) {}
    };
  };

  //set nav status for field edit/tune
  struct EditCtrl
  {
    template <typename O = Empty<Nil>>
    struct Part : O
    {
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      template <typename Nav>
      bool enter(Nav &nav)
      {
        // _trace(clog << "EditCtrl::enter" << endl);
        if (nav.focus())
        {
          nav.tune(nav.tune() ^ Base::tune());
          nav.focus(nav.tune());
        }
        else
          nav.focus(true);
        // return
        // Base::enter(nav);
        return true;
      }
      template <typename Nav>
      bool esc(Nav &nav)
      {
        // _trace(clog<<"esc!"<<endl);
        nav.tune(0);
        nav.focus(0);
        // return
        Base::enter(nav);
        return true; //irrelevant
      }
    };
  };

  //when ranged or enumerated values can wrap
  struct Wrap
  {
    template <typename O>
    struct Part : O
    {
      inline static constexpr bool canWrap() { return true; }
    };
  };

  //validate numeric field value by range info
  struct RangeValid
  {
    template <typename O = Empty<Nil>>
    struct Part : EditCtrl::Part<O>
    {
      using Base = typename EditCtrl::Part<O>;
      using This = Part<Base>;
      using Base::Base;
      using Type = typename Base::ValueType;
      template <typename Nav>
      void stepit(Nav &nav, int dir)
      {
        // dir*=options->invertFieldKeys?-1:1;
        Type thisstep = nav.tune() ? Base::tune() : Base::step();
        // Base::obj().changed(true);
        //by default they are inverted.. now buttons and joystick have to flip them
        if (dir > 0) {
          if ((Base::high() - Base::get()) < thisstep)
            Base::set(Base::canWrap() ? Base::low() : Base::high());
          else
            Base::set(Base::get()+thisstep);
        } else {
          if ((Base::get() - Base::low()) < thisstep)
            Base::set(Base::canWrap() ? Base::high() : Base::low());
          else
            Base::set(Base::get()-thisstep);
        }
      }
      template <typename Nav>
      bool up(Nav &nav)
      {
        if (nav.focus()) {
          // Base::set(Base::get() + (nav.tune() ? Base::tune() : Base::step()));
          stepit(nav,1);
          return true;
        }
        return false;
      }
      template <typename Nav>
      bool down(Nav &nav)
      {
        if (nav.focus())
        {
          Base::set(Base::get() - (nav.tune() ? Base::tune() : Base::step()));
          // stepit<Nav,Type>(nav,-1);
          return true;
        }
        return false;
      }
    };
  };

  //change numeric value with acceleration
  template <int relaxTimeout = 300, int rate = 1>
  struct ValueAccel
  {
    template <typename O>
    struct Part : O
    {
      using Base = O;
      using This = Part<O>;
      using Type = typename Base::ValueType;
      static constexpr Type tune() { return 0; }
      // Type value=0;
      int accel = 0;
      TinyTimeUtils::template TickCnt<relaxTimeout> relax;
      using Base::get;
      using Base::high;
      using Base::low;
      using Base::set;
      template <typename Nav>
      bool up(Nav &nav)
      {
        // _trace(clog<<"Accel up: "<<accel<<endl);
        if (accel >= 0)
          accel += rate;
        else
          accel /= -2;
        if (high() - get() < accel)
          set(high());
        else
        {
          set(get() + accel);
          relax.reset();
        }
        return Base::up(nav);
      }
      template <typename Nav>
      bool down(Nav &nav)
      {
        // _trace(clog<<"Accel down: "<<accel<<endl);
        if (accel <= 0)
          accel -= rate;
        else
          accel /= -2;
        if (get() - low() < -accel)
          set(low());
        else
        {
          set(get() + accel);
          relax.reset();
        }
        return Base::down(nav);
      }
      template <typename Dev>
      inline void poll(Dev &dev)
      {
        // _trace(if(accel) clog<<"ValueAccel::poll() cnt:"<<((uint16_t)relax)<<endl);
        if (relax)
        {
          accel = accel >> relax;
          relax.reset();
        }
      }
    };
  };

  //static bind to numeric reference
  template <typename T, T &target, T def = 0>
  struct NumRef
  {
    template <typename O = Empty<Nil>>
    struct Part : Chain<RangeValid::Part, FieldRef<T, target, def>::template Part>::template To<O>
    {
      using Base = typename Chain<RangeValid::Part, FieldRef<T, target, def>::template Part>::template To<O>;
      using This = Part<Base>;
      using Base::Base;
      constexpr static bool valueIs(T &v) { return target == v; }
    };
  };

  //storing numeric value with validation range
  template <typename T, T def = 0>
  struct NumVal
  {
    template <typename O = Empty<Nil>>
    struct Part : Chain<RangeValid::Part, FieldVal<T, def>::template Part>::template To<O>
    {
      using Base = typename Chain<RangeValid::Part, FieldVal<T, def>::template Part>::template To<O>;
      using This = Part<Base>;
      using Base::Base;
    };
  };

  //edit a text buffer
  template <char *buffer, Idx buffSz, ConstText masks[], char def = ' '>
  struct TextEditBase
  {
    template <typename O = Empty<>>
    struct Part : IsMenu::Part<ParentDraw::Part<O>>
    {
      using Base = IsMenu::Part<ParentDraw::Part<O>>;
      using This = Part<O>;
      using Base::Base;
      static constexpr Idx sz() { return buffSz - 1; }
      template <typename Nav>
      static bool enter(Nav &nav) {
        // _trace(mdo<<"TextEditBase::enter\n\r");
        nav.open();
        nav.focus(true);
        return false;
      }
      static ConstText validator(int i) {
        return masks[i % (sizeof(masks) / sizeof(masks[0]))];
      }
      template <typename Nav, Cmd c>
      static bool cmd(Nav &nav, int code = 0) {
        switch (c)
        {
        //from AM4
        case Cmd::Enter:
          if (nav.tune() && !nav.edited())
          {
            nav.focus(false);
            nav.tune(false);
            nav.close();
            return false;
          }
          else
          {
            nav.tune(!nav.tune());
            if (nav.tune())
              nav.edited(false);
            return false;
          }
        case Cmd::Esc:
          nav.tune(false);
          nav.focus(false);
          nav.close();
          return false;
        case Cmd::Up:
          if (nav.tune())
          {
            const char *v = validator(nav.pos());
            const char *at = strchr(v, buffer[nav.pos()]);
            Idx pos = at ? at - v + 1 : 1;
            if (pos >= (Idx)strlen(v))
              pos = 0;
            buffer[nav.pos()] = v[pos];
            nav.edited(true);
            return true;
          }
          break;
        case Cmd::Down:
          if (nav.tune())
          {
            const char *v = validator(nav.pos());
            const char *at = strchr(v, buffer[nav.pos()]);
            int pos = at ? at - v - 1 : 0;
            if (pos < 0)
              pos = strlen(v) - 1;
            buffer[nav.pos()] = v[pos];
            nav.edited(true);
            return true;
          }
          break;
        default:
          break;
        }
        return nav.template doNav<c>(sz(), Base::styles()); //generic menu nav
      }
      template <typename Nav, typename Out, bool delegate = true>
      static void printTo(Nav &nav, Out &out, int n = 0, bool sel = false)
      {
        // _trace(clog<<"TextEditBase::printTo"<<endl;clog.flush());
        if (sel /*&&editing*/)
        { //if we are here then we are editing
          if (nav.pos())
            out.write(buffer, nav.pos());
          out.template fmtStart<Fmt::EditCursor>(nav.focus(), nav.tune());
          out.print((char)buffer[nav.pos()]);
          out.template fmtStop<Fmt::EditCursor>(nav.focus(), nav.tune());
          if (nav.pos() < sz() - 1)
            out.print(buffer + nav.pos() + 1);
        }
        else
          out.print(buffer);
        if (delegate)
          Base::printTo(nav, out, n, sel);
      }
      template <typename Nav, typename Out>
      static void onPrintMenuTo(Nav &nav, Out &out,Idx level, int) {}
      template <typename Nav, typename Out>
      static void onPrintTitleTo(Nav &nav, Out &out) {}
      template <typename Nav, typename Out>
      static void onPrintBodyTo(Nav &nav, Out &out,Idx level, Idx selIdx, Idx n) {}
      // template <typename Nav, typename Out>
      // void onPrintItemTo(Nav &nav, Out &out,Idx level, int n, bool sel)
      // {
      //   O::obj().template printTo<Nav, Out>(nav, out, n, sel);
      // }
    };
  };

  template <char *buffer, Idx len, ConstText mask[], char def = ' '>
  using TextEdit = Chain<
      AsEditMode::Part,
      AsValue::Part,
      TextEditBase<buffer, len, mask, def>::template Part>;

  template<typename T,T def=0>
  struct NumField {
    template<typename O>
    using Part=typename Chain<
      Menu::AsEditMode::Part,
      Menu::AsValue::Part,//format as value
      Menu::EditCtrl::Part,//editable
      Menu::RangeValid::Part,//use step/tune to change field value
      Menu::FieldVal<T,def>::template Part
    >::template To<O>;
  };

  template<typename T,T& ref,T def=0>
  struct NumFieldRef {
    template<typename O>
    using Part=typename Chain<
      Menu::AsEditMode::Part,
      Menu::AsValue::Part,//format as value
      Menu::EditCtrl::Part,//editable
      Menu::RangeValid::Part,//use step/tune to change field value
      Menu::FieldRef<T,ref,def>::template Part
    >::template To<O>;
  };

  template<typename T,T(&get)(),void(&set)(T),T def>
  struct NumFuncField {
    template<typename O>
    using Part=typename Chain<
      Menu::AsEditMode::Part,
      Menu::AsValue::Part,//format as value
      Menu::EditCtrl::Part,//editable
      Menu::RangeValid::Part,//use step/tune to change field value
      Menu::FuncField<T,get,set,def>::template Part
    >::template To<O>;
  };

}; //Menu namespace