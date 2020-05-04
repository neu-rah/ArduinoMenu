
#include "serialOut.h"

namespace Menu {
  #ifdef MENU_DEBUG
    constexpr idx_t w=20;
  #else
    constexpr idx_t w=40;
  #endif
  constMEM panel default_serial_panels[] MEMMODE={{0,0,w,10}};//arbitrary serial panel dimensions
  navNode* default_serial_nodes[sizeof(default_serial_panels)/sizeof(panel)];
  panelsList default_serial_panel_list(
    default_serial_panels,
    default_serial_nodes,
    sizeof(default_serial_panels)/sizeof(panel)
  );
};
