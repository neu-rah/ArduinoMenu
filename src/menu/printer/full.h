/* -*- C++ -*- */
#pragma once

namespace Menu {

  template<
    typename O,
    template<typename> class itemFmt=asItem,
    template<typename> class titleFmt=asTitle,
    template<typename> class menuFmt=asMenu,
    template<typename> class panelFmt=asPanel
  > struct deviceParts:public O {};

  template<typename O>
  struct FullPrinter:public O {
    using This=FullPrinter<O>;
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void printMenuRaw(Item& o) {
      O::printMenuRaw(o);
      for(size_t n=0;n<o.size();n++)
        o[n].out(*(MenuOut*)this);
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    using O::O;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void printMenuRaw(Item& o) {
      Serial<<"TitlePrinter::printMenuRaw"<<endl;
      #if (MENU_INJECT_PARTS==true)
        PrinterPart pp;
        o.out(*reinterpret_cast<MenuOutCap<TitlePrinter<O>>*>(this),pp);
      #else
        o.out(*(MenuOut*)this);//TODO: need viewport for non-text devices
      #endif
      O::printMenuRaw(o);
    }
  };
};//Menu namespace
