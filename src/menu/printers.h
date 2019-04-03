/* -*- C++ -*- */
#pragma once

namespace Menu {

  // template<
  //   typename O,
  //   template<typename> class itemFmt=asItem,
  //   template<typename> class titleFmt=asTitle,
  //   template<typename> class menuFmt=asMenu,
  //   template<typename> class panelFmt=asPanel
  // > struct deviceParts:public O {};

  template<typename O>
  struct FullPrinter:public O {
    using This=FullPrinter<O>;
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void printMenuRaw(PrintHead p,Item& o) {
      // MENU_DEBUG_OUT<<"FullPrinter::printMenuRaw"<<endl;
      p.printer.fmtMenu(true);
      O::printMenuRaw(p,o);
      for(size_t n=0;n<o.size();n++)
        o[n].out(p.printer);
      p.printer.fmtMenu(false);
    }
  };

  template<typename O>
  struct TextCursorPrinter:public O {
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void printMenuRaw(PrintHead p,Item& o) {
      // MENU_DEBUG_OUT<<"TextCursorPrinter::printMenuRaw"<<endl;
      if (selected(p)) p.printer.fmtCursor(true);
      // o.out(p.printer);
      O::printMenuRaw(p,o);
      if (selected(p)) p.printer.fmtCursor(false);
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void printMenuRaw(PrintHead p,Item& o) {
      // MENU_DEBUG_OUT<<"TitlePrinter::printMenuRaw"<<endl;
      #if (MENU_INJECT_PARTS==true)
        //guess i wont need this
        PrinterPart pp;
        o.out(*reinterpret_cast<MenuOutCap<TitlePrinter<O>>*>(this),pp);
      #else
        o.out(p.printer);//TODO: need viewport for non-text devices
      #endif
      O::printMenuRaw(p,o);
    }
  };
};//Menu namespace
