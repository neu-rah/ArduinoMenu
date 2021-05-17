#pragma once

#include "menu/in.h"
#include <debug.h>
using namespace Parts;

#include <tinyTimeUtils.h>
using namespace TinyTimeUtils;

namespace Menu {

  /*
  this can be done:
  1) within menu options => scan structure to build shell
  2) as a separate list
  => use both!
  */
  //commands base
  struct Command {
    template<typename O>
    struct Part {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
    };
  };

  //radio command id
  template<Idx code,bool repeat=true>
  struct CmdCode {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
    };
  };

  //shell command definition
  //TODO:
  //  how to match text?
  //  how to define params
  //  menu options can all be commands, so do we need this as item part?
  //  we do need this for non menu ralated commands => separate list
  struct ShellCmd {
  struct Int {};
  struct Float {};
  struct Str {};
  template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      template<typename M>
      static bool match(M&) {return false;}
      static bool match(ConstText m) {
        return false;
      }
      
    };
  };

    //RF input device, reads command codes and eventually call parser
  template<int repeatTime=200,int pauseTimeout=500>
  struct RFIn {
    template<typename In=Quiet<>>
    struct Part:In {
      static constexpr bool isReader=true;
      static volatile uint32_t rfCmd;
      static uint32_t lastCmd;
      static Tick<repeatTime> radioRepeat;
      static Tick<pauseTimeout> radioPause;
      static bool rcvCmd(unsigned long cmd) {
        rfCmd=cmd;
        radioPause.reset();
        // _trace(clog<<"RFIn::rcvCmd: 0x"<<hex<<cmd<<endl);
        return true;
      }
      void relax() {
        // _trace(clog<<"RFIn::relax"<<endl);
        //allow commands to repeat
        if(radioRepeat) {
          lastCmd=0;
          rfCmd=0;
        }
        if(radioPause) rfCmd=0;
        In::relax();//pass on
      }
      template<typename Nav>
      inline bool parseKey(Nav& nav) {
        if(lastCmd!=rfCmd) {
          // _trace(clog<<"RFin::parseKey: 0x"<<hex<<rfCmd<<endl);
          lastCmd=rfCmd;
          return In::parseCmd(nav,lastCmd);
        }
        return In::parseKey(nav);
      }
    };
  };

};

