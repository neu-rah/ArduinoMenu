#include "../menuDefs.h"
#ifdef MENU_FMT_WRAPS
  #include "xmlFmt.h"

  namespace Menu {
    void outputOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx) {
      o<<"<node>";
      for(idx_t n=0;n<node.sz();n++)
        o<<"<value"<<(n==node.selected()?" selected=\"selected\"":"")<<">"<<node[n]<<"</value>";
      o<<"</node>";
    }
  };
#endif
