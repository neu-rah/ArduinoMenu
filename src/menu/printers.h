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
    // using This=FullPrinter<O>;
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    void printMenuRaw(PrintHead<P> p,const Item& o) {
      // MENU_DEBUG_OUT<<"FullPrinter::printMenuRaw"<<endl;
      p.printer.fmtMenu(p,true);
      O::printMenuRaw(p,o);
      for(size_t n=0;n<o.size();n++) {
        O::printMenuRaw(PrintHead<P>{p.menuOut,p.printer,n},o[n]);
      }
      p.printer.fmtMenu(p,false);
    }
  };

  template<typename O>
  struct TextCursorPrinter:public O {
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    void printMenuRaw(PrintHead<P> p,const Item& o) {
      // MENU_DEBUG_OUT<<"TextCursorPrinter::printMenuRaw"<<endl;
      // if (p.menuOut.selected(p)) 
      p.printer.fmtCursor(p,true);
      // o.out(p.printer);
      O::printMenuRaw(p,o);
      if (selected(p)) p.printer.fmtCursor(p,false);
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    void printMenuRaw(PrintHead<P> p,const Item& o) {
      // MENU_DEBUG_OUT<<"TitlePrinter::printMenuRaw"<<endl;
      #if (MENU_INJECT_PARTS==true)
        //guess i wont need this
        PrinterPart pp;
        o.out(*reinterpret_cast<MenuOutCap<TitlePrinter<O>>*>(this),pp);
      #else
        o.out(p.menuOut);//TODO: need viewport for non-text devices
      #endif
      O::printMenuRaw(p,o);
    }
  };
};//Menu namespace
