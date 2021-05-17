#include "rfIn.h"

namespace Menu {

  template<int repeatTime,int pauseTimeout>
  template<typename In>
  volatile uint32_t RFIn<repeatTime,pauseTimeout>::Part<In>::rfCmd;

  template<int repeatTime,int pauseTimeout>
  template<typename In>
  uint32_t RFIn<repeatTime,pauseTimeout>::Part<In>::lastCmd;

  template<int repeatTime,int pauseTimeout>
  template<typename In>
  Tick<repeatTime> RFIn<repeatTime,pauseTimeout>::Part<In>::radioRepeat;

  template<int repeatTime,int pauseTimeout>
  template<typename In>
  Tick<pauseTimeout> RFIn<repeatTime,pauseTimeout>::Part<In>::radioPause;

};

