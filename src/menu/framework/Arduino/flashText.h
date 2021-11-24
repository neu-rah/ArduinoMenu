#pragma once

namespace Menu {
  template <const PROGMEM ConstText* text>
  struct StaticFlashText {
    template <typename O = Empty<Nil>>
    struct Part : O {
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &out, int n = 0, bool sel = false) {
        out.print(text[0]);
        if (delegate)
          Base::printTo(nav, out, n, sel);
      }
    };
  };

  struct FlashText {
    template <typename O = Empty<Nil>>
    struct Part : O {
      using Base = O;
      using This = Part<Base>;
      using Base::Base;
      const ConstText* text;
      Part(const ConstText* text):text(text) {}
      template <typename Nav, typename Out, bool delegate = true>
      void printTo(Nav &nav, Out &out, int n = 0, bool sel = false) {
        out.print(text[0]);
        if (delegate)
          Base::printTo(nav, out, n, sel);
      }
    };
  };

  template <const PROGMEM ConstText* text>
  struct SCFlashText {
    template <typename O = Empty<Nil>>
    struct Part:FlashText::Part<O> {
      using Base=typename FlashText::Part<O>;
      Part():Base(text) {}
    };
  };

  // template <const PROGMEM ConstText *text>
  // using FlashLabel = Chain<AsLabel::Part, FlashText<text>::template Part>;
};