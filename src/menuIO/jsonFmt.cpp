#ifdef MENU_FMT_WRAPS
  #include "../menuDefs.h"
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
          default: return "???";
        }
      }
      void printIndent() {
        for(int n=0;n<partIndent;n++) MENU_DEBUG_OUT<<"  ";
      }
    #endif
    int cursorCtrl=0;
    void jsonOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx) {
      o<<"\",options\":[";
      // o<<node.sz();
      for(idx_t n=0;n<node.sz();n++)
        o<<(n?",":"")<<"\""<<node[n]<<"\"";
      o<<"]";
    }
  };
#endif
