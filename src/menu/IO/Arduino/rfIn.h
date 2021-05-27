#pragma once

#include "menu/in.h"
#include <debug.h>
using namespace Parts;

#include "../../tinyTimeUtils/src/tinyTimeUtils.h"
using namespace TinyTimeUtils;

namespace Menu {

  // this can be done:
  // 1) within menu options => scan structure to build shell
  // 2) as a separate list
  // => use both!
  // radio will not use this, we wil use old fashion switch case
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
  // template<Idx _code,bool _repeat=true>
  // struct CmdCode {
  //   static constexpr Idx code() {return _code;}
  //   static constexpr Idx repeat() {return _repeat;}
  //   static bool codeIs(Idx i) {return i==code();}
  //   // template<typename O>
  //   // struct Part:O {
  //     // using Base=O;
  //     // using This=Part<Base>;
  //     // using Base::Base;
  //   // };
  // };
  //
  // union CmdCode {
  //   inline CmdCode(uint8_t o):raw(o) {}
  //   uint8_t raw;
  //   struct {
  //     uint8_t code:7;
  //     uint8_t repeat:1;
  //   };
  // };
  //
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

  //use a button for pairing
  struct ArduinoPairBtn {
    template<typename O>
    struct Part:O {
      bool rcvCmd(unsigned long raw) {
        // clog<<"pairing:"<<O::PinAPI::get()<<endl;
        return O::rcvCmd(raw,O::PinAPI::get());
      }
    };
  };

  //lock to remote id (pairing)
  //idBits: size of id in bits
  template<int idBits=16,int handlerId=0>
  struct RFIdLock {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      static unsigned long pairId;
      static bool rcvCmd(unsigned long raw,bool pair=false) {
        // _trace(clog<<"RFIdLock::rcvCmd: 0x"<<hex<<raw<<endl);
        int ds=O::Type::dataSz();
        int sz=ds-idBits;
        unsigned long mask=~(((unsigned long)(~0))<<(ds-idBits));
        unsigned long idMask=((unsigned long)(~0))>>((sizeof(raw)<<3)-ds);
        unsigned long id=(raw&idMask)>>sz;
        if(pair) {
          pairId=id;
          return true;
        }
        if(id!=pairId) return false;
        return id==pairId&&O::rcvCmd(raw&mask);
      }
      template<typename M>
      bool save(M& midia) {
        _trace(clog<<"RFIdLock::save 0x"<<hex<<pairId<<endl);
        return midia.write(pairId)&&Base::save(midia);
      }
      template<typename M>
      bool load(M& midia) {
        bool r=midia.read(pairId);
        _trace(clog<<"RFIdLock::load 0x"<<hex<<pairId<<endl);
        return r&&Base::load(midia);
      }
    };    
  };

  //RF input device, reads command codes and eventually call parser
  template<int repeatTimeout=100>
  struct RFIn {
    template<typename In=Quiet<>>
    struct Part:In {
      using Base=In;
      using This=Part<In>;
      using Base::Base;
      static constexpr bool isReader=true;
      static volatile uint32_t rfCmd;
      static uint32_t lastCmd;
      // static Tick<repeatTime> radioRepeat;
      // static Tick<pauseTimeout> radioPause;
      static Tick<repeatTimeout> radioTimeout;//must be greater the repeat period
      //radio receiver callback
      static bool rcvCmd(unsigned long cmd) {
        rfCmd=cmd;
        if(radioTimeout) {
          lastCmd=0;
          radioTimeout.reset();
        } else if(!(lastCmd==cmd&&Base::repeats((uint8_t)cmd))) radioTimeout.reset();
        // _trace(clog<<"RFIn::rcvCmd: 0x"<<hex<<cmd<<endl);
        return true;
      }
      // void relax() {
      //   // _trace(clog<<"RFIn::relax"<<endl);
      //   //allow commands to repeat
      //   if(radioRepeat) {
      //     lastCmd=0;
      //     rfCmd=0;
      //   }
      //   if(radioPause) rfCmd=0;
      //   In::relax();//pass on
      // }
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

