/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// Printers - generate event messages for the formatter and walk the structure

namespace Menu {

  template<typename O>
  struct SelItemPrinter:public O {
    // using This=FullPrinter<O>;
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    using titleFmt=typename RAW_DEVICE::Parts::template titleFmt<P>;
    template<typename P>
    using itemFmt=typename RAW_DEVICE::Parts::template itemFmt<P>;
    template<typename P>
    void printMenuRaw(PrintHead<P> p,const Item& o) {
      // MENU_DEBUG_OUT<<"FullPrinter::printMenuRaw "<<o.size()<<endl;
      p.printer.fmtMenu(p,true);
      O::printMenuRaw(p,o);
      reinterpret_cast<itemFmt<O>*>(this)
        ->printMenuRaw(PrintHead<P>{p.menuOut,p.printer,O::pos()},o[O::pos()]);
      p.printer.fmtMenu(p,false);
    }
  };

  template<typename O>
  struct FullPrinter:public O {
    // using This=FullPrinter<O>;
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    using titleFmt=typename RAW_DEVICE::Parts::template titleFmt<P>;
    template<typename P>
    using itemFmt=typename RAW_DEVICE::Parts::template itemFmt<P>;
    template<typename P>
    void printMenuRaw(PrintHead<P> p,const Item& o) {
      // MENU_DEBUG_OUT<<"FullPrinter::printMenuRaw "<<o.size()<<endl;
      // p.printer.fmtMenu(p,true);
      // reinterpret_cast<titleFmt<O>*>(this)->printMenuRaw(p,o);
      // MenuOutCap<titleFmt<O>>(p.menuOut).printMenuRaw(p,o);;
      // O::printMenuRaw(p,o);
      for(size_t n=0;n<o.size();n++) {
        reinterpret_cast<itemFmt<O>*>(this)->printMenuRaw(PrintHead<P>{p.menuOut,p.printer,n},o[n]);
      }
      p.printer.fmtMenu(p,false);
    }
  };

  template<typename O>
  struct TextAccelPrinter:public O {
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    void printMenuRaw(PrintHead<P> p,const Item& o) {
      p.printer.fmtAccel(p,true);
      p.printer.fmtAccel(p,false);
      O::printMenuRaw(p,o);
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
      p.printer.fmtCursor(p,false);
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
        O::printMenuRaw(p,o);
      #else
        //instead of sending the request thru a chain of calls (as above)
        //we just call the fmt functions direrctly (this would be the result of the above)
        //since we have access to th eprinter head
        p.printer.fmtTitle(p,true);
        o.out(p.menuOut);
        p.printer.fmtTitle(p,false);
        //this part does NOT wrap the next
        O::printMenuRaw(p,o);
      #endif
    }
  };

  template<typename O>
  struct ItemPrinter:public O {
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    void printMenuRaw(PrintHead<P> p,const Item& o) {
      // MENU_DEBUG_OUT<<"ItemPrinter::printMenuRaw"<<endl;
      p.printer.fmtItem(p,true);
      o.out(p.menuOut);
      O::printMenuRaw(p,o);
      p.printer.fmtItem(p,false);
    }
  };

  //collection of printer parts to customize part printing
  //for panel|menu|title|items
  template<
    template<typename> class i=ID,
    template<typename> class t=ID,
    template<typename> class m=ID,
    template<typename> class p=ID,
    typename O=Void
  > struct DeviceParts:public O {
    using O::O;
    template<typename T> using itemFmt=i<T>;
    template<typename T> using titleFmt=t<T>;
    template<typename T> using menuFmt=m<T>;
    template<typename T> using panelFmt=p<T>;
  };

};//Menu namespace
