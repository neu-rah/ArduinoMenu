#include "serialOut.h"

namespace Menu {
  const panel default_serial_panels[] MEMMODE={{0,0,40,10}};
  navNode* default_serial_nodes[sizeof(default_serial_panels)/sizeof(panel)];
  panelsList default_serial_panel_list(
    default_serial_panels,
    default_serial_nodes,
    sizeof(default_serial_panels)/sizeof(panel)
  );
};
