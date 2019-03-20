/* -*- C++ -*- */
#pragma once

// namespace ArduinoMenu {
  enum Roles:uint16_t {
    noRole=0,
    panelRole=1,
    titleRole=2,
    bodyRole=4,
    itemRole=8,
    accelRole=16,
    cursorRole=32,
    modeRole=64,
    valueRole=128,
    unitRole=256,
    menuRole=512,
  };

  template<Roles m,typename O>
  struct Role:public O {
    using O::O;
    // Role(O& o):O(o) {}
    // static constexpr Roles mask=m;
    inline Out& out(Out& o) const {}
  };

  template<typename O>
  struct Role<itemRole,O>:public O {
    using O::O;
    // Role(O& o):O(o) {}
    // static constexpr Roles mask=m;
    inline Out& out(Out& o) const {}
  };

  template<typename O> using asPanel=Role<panelRole,O>;
  template<typename O> using asTitle=Role<titleRole,O>;
  template<typename O> using asBody=Role<bodyRole,O>;
  template<typename O> using asItem=Role<itemRole,O>;
  template<typename O> using asAccel=Role<accelRole,O>;
  template<typename O> using asCursor=Role<cursorRole,O>;
  template<typename O> using asMode=Role<modeRole,O>;
  template<typename O> using asUnit=Role<unitRole,O>;
  template<typename O> using asMenu=Role<menuRole,O>;
// };
