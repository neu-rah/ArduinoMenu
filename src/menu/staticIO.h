/* -*- C++ -*- */
#pragma once

#include "api.h"
#ifndef ARDUINO
#include <string>
#endif

namespace Menu {
  //aux class for monometric text measure
  template<int szX=1,int szY=1>
  struct TextMeasure {
    template<typename O>
    struct Part:O {
      template<typename T>
      static inline Area measure(T o) {
        #ifdef ARDUINO
          return {(Idx)String(o).length(),maxCharHeight()};
        #else
          return {_str(o),maxCharHeight()};
        #endif
      }
      inline static constexpr int maxCharWidth() {return 1;}
      inline static constexpr int maxCharHeight() {return 1;}
      int textWidth(const char*s) const {return measure(s);}
      protected:
      #ifndef ARDUINO
        static inline Idx _str(const char*o){return std::string(o).length();}
        template<typename T>
        static inline Idx _str(T i){return std::string(std::to_string(i)).length();}
      #endif
    };
  };

  struct PartialDraw {
    template<typename O>
    struct Part:O {
      inline static constexpr bool partialDraw() {return true;}
    };
  };

  template<typename Dev,Dev& dev>
  struct StreamOut {
    template<typename O>
    struct Part:O {
      using This=StreamOut<Dev,dev>::Part<O>;
      template<bool toPrint=true>
      inline static void nl() {
        trace(MDO<<"StreamOut::nl<"<<(toPrint?"true":"false")<<">()"<<endl);
        if(toPrint) dev<<endl;
      }
      template<typename T,bool toPrint=true>
      inline static void raw(T o) {
        trace(MDO<<"StreamOut::raw<"<<(toPrint?"true":"false")<<">("<<o<<")"<<endl);
        if(toPrint) dev<<o;
      }
    };
  };

  template<typename Dev,Dev& dev>
  struct StreamIn {
    template<typename O>
    struct Part:O {
      template<typename Nav>
      inline static bool cmd(Nav& nav) {
        char c;
        dev>>c;
        switch(c) {
          case '+': return nav.cmd(Cmd::Up);
          case '-': return nav.cmd(Cmd::Down);
          case '*': return nav.cmd(Cmd::Enter);
          case '/': return nav.cmd(Cmd::Esc);
          default:return false;
        }
      }
    };
  };

  struct None:Nil {
    template<typename N=Nil>
    struct Part:N {
      template<typename Nav>
      inline static bool cmd(Nav& nav) {return false;}
    };
  };

  template<Expr... O>
  struct StaticMenuOut:Chain<O...,Void>::template To<Obj<StaticMenuOut<O...>>> {
    using Base=typename Chain<O...,Void>::template To<Obj<StaticMenuOut<O...>>>;
    using This=StaticMenuOut<O...>;
    using Base::Base;
  };
};
