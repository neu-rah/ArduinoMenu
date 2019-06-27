#include "../menuDefs.h"
#ifdef MENU_FMT_WRAPS
  #include "jsonFmt.h"

  namespace Menu {
    #ifdef MENU_DEBUG
      int partIndent=0;
      const char* partName(menuOut::fmtParts part) {
        switch (part){
          case menuOut::fmtPanel: return "fmtPanel";
          case menuOut::fmtTitle: return "fmtTitle";
          case menuOut::fmtBody: return "fmtBody";
          case menuOut::fmtOp: return "fmtOp";
          case menuOut::fmtIdx: return "fmtIdx";
          case menuOut::fmtCursor: return "fmtCursor";
          case menuOut::fmtOpBody: return "fmtOpBody";
          case menuOut::fmtPreview: return "fmtPreview";
          case menuOut::fmtPrompt: return "fmtPrompt";
          case menuOut::fmtField: return "fmtField";
          case menuOut::fmtToggle: return "fmtToggle";
          case menuOut::fmtSelect: return "fmtSelect";
          case menuOut::fmtChoose: return "fmtChoose";
          case menuOut::fmtUnit: return "fmtUnit";
          case menuOut::fmtTextField: return "fmtTextField";
          case menuOut::fmtCursorOpen: return "fmtCursorOpen";
          case menuOut::fmtCursorClose: return "fmtCursorClose";
          case menuOut::fmtValue: return "fmtVluea";
          default: return "???";
        }
      }
      void printIndent() {
        for(int n=0;n<partIndent;n++) MENU_DEBUG_OUT<<"  ";
      }
    #endif
    int cursorCtrl=0;
    void jsonOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx) {
      // o<<",\"options\":[";
      o.print(",\"options\":[");
      // o<<node.sz();
      for(idx_t n=0;n<node.sz();n++) {
        o.print(n?",":"");
        o.print("\"");
        o.print(node[n].getText());
        o.print("\"");
        // o<<(n?",":"")<<"\""<<node[n]<<"\"";
      }
      // o<<"],\"sel\":\""<<((menuVariantBase*)&node)->sync()<<"\"";
      o.print("],\"sel\":\"");
      o.print(((menuVariantBase*)&node)->sync());
      o.print("\"");
    }
  };
#endif
