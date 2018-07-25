#ifdef MENU_FMT_WRAPS
  #include "../menuDefs.h"
  #include "jsonFmt.h"

  namespace Menu {
    void jsonOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx) {
      o<<"\"options\":[";
      for(idx_t n=0;n<node.sz();n++)
        o<<(n?",":"")<<"\""<<node[n]<<"\"";
      o<<"]";
    }
  };
#endif
