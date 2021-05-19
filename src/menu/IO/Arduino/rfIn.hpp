#include "rfIn.h"

namespace Menu {

  template<int repeatTimeout>
  template<typename In>
  volatile uint32_t RFIn<repeatTimeout>::Part<In>::rfCmd;

  template<int repeatTimeout>
  template<typename In>
  uint32_t RFIn<repeatTimeout>::Part<In>::lastCmd;

  // template<int repeatTime,int pauseTimeout>
  // template<typename In>
  // Tick<repeatTime> RFIn<repeatTime,pauseTimeout>::Part<In>::radioRepeat;

  // template<int repeatTime,int pauseTimeout>
  // template<typename In>
  // Tick<pauseTimeout> RFIn<repeatTime,pauseTimeout>::Part<In>::radioPause;

  template<int repeatTimeout>
  template<typename In>
  Tick<repeatTimeout> RFIn<repeatTimeout>::Part<In>::radioTimeout;

  template<int idBits,int handlerId>
  template<typename O>
  unsigned long RFIdLock<idBits,handlerId>::Part<O>::pairId=0;
};

