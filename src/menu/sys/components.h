/* -*- C++ -*- */
#pragma once

#include "menu/item.h"

namespace Menu
{

  //emit formatting tag events
  template <Fmt tag>
  struct As
  {
    template <typename O>
    struct Part : O
    {
      using Base = O;
      using This = Part<O>;
      using Base::Base;
      using IsTag = Yes;
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &out, int n = 0, bool sel = false)
      {
        using BaseTag = typename Base::IsTag;
        out.template fmtStart<tag>(nav.focus(), nav.tune(), n, sel);
        Base::template printTo<Nav, Out, BaseTag::value() || delegate>(nav, out, n, sel);
        out.template fmtStop<tag>(nav.focus(), nav.tune(), n, sel);
        // using SkipBase=typename Base::Base;
        // if(delegate&&!BaseTag::value())
        //   SkipBase::template printTo<Nav,Out,true>(nav,out,n,sel);
      }
    };
  };

  using AsPanel = As<Fmt::Panel>;
  using AsMenu = As<Fmt::Menu>;
  using AsTitle = As<Fmt::Title>;
  using AsBody = As<Fmt::Body>;
  using AsItem = As<Fmt::Item>;
  using AsPad = As<Fmt::Pad>;
  using AsIndex = As<Fmt::Index>;
  using AsCursor = As<Fmt::Cursor>;
  using AsField = As<Fmt::Field>;
  using AsAccel = As<Fmt::Accel>;
  using AsLabel = As<Fmt::Label>;
  using AsEditMode = As<Fmt::EditMode>;
  using AsValue = As<Fmt::Value>;
  using AsUnit = As<Fmt::Unit>;

  //hold an Id of type T
  template <typename T>
  struct StaticIdOf
  {
    template <T id>
    struct Id
    {
      template <typename O = Empty<Nil>>
      struct Part:O{
        bool chkId(T n) const { return id == n; }
      };
    };
  };

  //Idx type Id
  template<Idx id>
  using Id = StaticIdOf<Idx>::Id<id>;

  //empty dumb part
  struct Zen {template <typename O>struct Part : O{};};

  //holds runtiume construct const char* (C string)
  struct Text
  {
    template <typename O = Empty<Nil>>
    struct Part : O
    {
      // named("Text")
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      const char *text;
      template <typename P, typename... OO>
      constexpr Part(P o, OO... oo) : text(o), Base(oo...) {}
      // template<typename Next,typename P,typename... OO>
      // static Next build(P,OO... oo) {return Next(oo...);}
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &o, int n, bool sel)
      {
        o.print(text);
        if (delegate)
          Base::printTo(nav, o, n, sel);
      }
    };
  };

  //use static constructed `const char*`
  template <const ConstText *text>
  struct StaticText
  {
    template <typename O = Empty<Nil>>
    struct Part : O
    {
// named("StaticText")
#if defined(RS_DEBUG)
      static constexpr const char *name = "StaticText";
      static constexpr const char *(named)() { return name; }
      // static void names() {
      //   MDO::print(name);
      //   MDO::print("(\"");
      //   MDO::print(text[0]);
      //   MDO::print("\")");
      //   MDO::print("/");
      //   Base::names();
      // }
#endif
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &out, int n = 0, bool sel = false)
      {
        out.print(text[0]);
        if (delegate)
          Base::printTo(nav, out, n, sel);
      }
    };
  };

  // using Events=uint8_t;
  // enum class Event:Events {
  //   None=0<<0,
  //   Enter=1<<0,
  //   Esc=1<<1,
  //   Focus=1<<2,
  //   Blur=1<<3,
  //   Update=1<<4,
  //   Any=0xFF
  // };

  // template<Events handled,bool (*fn)(Events)>
  // struct On {
  //   bool event(Events e) {return fn(e);}
  // };

    //Associate an action
  //triggered at `enter` call
  template <ActionFn fn>
  struct Action {
    template <typename O>
    struct Part : O {
      // named("Action");
      using Base = O;
      using This = Part<O>;
      using Base::Base;
      template <typename Nav>
      inline bool enter(Nav& nav) {
        // _trace(clog<<"Action::enter"<<endl);
        Base::enter(nav);return action();}
      inline bool action() {//note: can not be static because of `enabled()`, its a runtime property... move it to enter
        return Base::obj().enabled() ? fn() : !has<Style::IsMenu>(Base::styles());
      }
    };
  };

  //enable/diable runtime setting
  template <bool e = true>
  struct EnDis
  {
    template <typename I>
    struct Part : public I
    {
    public:
      using I::I;
      inline bool enabled() const { return isEnabled; }
      inline void enable(bool b) { isEnabled = b; }

    protected:
      bool isEnabled = e;
    };
  };

  //disable commands when condition not met
  //use this to disable commands when device is off
  template<typename F,F& f>
  struct When {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      template<typename Nav,Cmd c>
      bool cmd(Nav& nav,int code=0) {
        // _trace(clog<<"When::cmd "<<f()<<endl);
        return f()?Base::template cmd<Nav,c>(nav,code):false;
      }
    };
  };

  template<ActionFn f>
  struct OnUpdate {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      using V=typename Base::ValueType;
      void set(V o) {
        // _trace(clog<<"OnUpdate::set"<<endl);
        Base::set(o);
        f();
      }
    };    
  };

  //utility, use constText as label
  template <const ConstText *text>
  using StaticLabel = Chain<AsLabel::Part, StaticText<text>::template Part>;

  #ifdef ARDUINO
    //use static constructed `const char*`
    // template <const PROGMEM char *text>
    // struct FlashText {
    //   template <typename O = Empty<Nil>>
    //   struct Part : O {
    //     using Base = O;
    //     using This = Part<Base>;
    //     using Base::Base;
    //     template <typename Nav, typename Out, bool delegate = true>
    //     void printTo(Nav &nav, Out &out, int n = 0, bool sel = false) {
    //       out.print(text[0]);
    //       if (delegate)
    //         Base::printTo(nav, out, n, sel);
    //     }
    //   };
    // };

    // template <const PROGMEM char *text>
    // using FlashLabel = Chain<AsLabel::Part, FlashText<text>::template Part>;

  #endif
}; //Menu namespace