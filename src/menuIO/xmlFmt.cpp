#ifdef MENU_FMT_WRAPS
  #include "../menu.h"
  #include "xmlFmt.h"

  namespace Menu {
    menuOut& operator<<(menuOut&o,classes c) {
      switch(c) {
        case noClass:return o<<"noClass";break;
        case promptClass:return o<<"prompt";break;
        case fieldClass:return o<<"field";break;
        case toggleClass:return o<<"toggle";break;
        case selectClass:return o<<"select";break;
        case chooseClass:return o<<"choose";break;
        case valueClass:return o<<"value";break;
        case menuClass:return o<<"menu";break;
        default: return o;
      }
    }

    void outputOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx) {
      o<<"<node data-path=\"/menu?at=";
      nav.root->printPath(o);
      o<<"/"<<idx<<"\">";
      for(idx_t n=0;n<node.sz();n++)
        o<<"<value"<<(n==node.selected()?" selected=\"selected\"":"")<<">"<<node[n]<<"</value>";
      o<<"</node>";
    }
  };
#endif
