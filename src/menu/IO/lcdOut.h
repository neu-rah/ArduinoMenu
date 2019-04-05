/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use arduino standard LCD library as menu output

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <streamFlow.h>
#include "../out.h"
#include "../nav.h"
#include "../panels.h"
#include "../fmt/textCursor.h"
#include "../fmt/cursorPos.h"
#include "../fmt/titleWrap.h"

namespace Menu {

  template<typename P,typename Dev,Dev& dev, typename O=Void>
  struct LCDOutDef:public O {
    using RAW_DEVICE = LCDOutDef<P,Dev,dev,O>;
    using Parts=P;
    using O::O;
    template<typename T>
    inline void raw(T i) {
      if (!operator bool()) return;
      dev.setCursor(O::posX(),O::posY());
      // Serial<<"lcd.setCursor("<<posX()<<","<<posY()<<") "<<i<<endl;
      O::useX(dev.print(i));
    }
  };

  using LCDParts=DeviceParts<
    Chain<TextCursorPrinter,ItemPrinter>::To,//emit format messages for accel, cursor amd item
    TitlePrinter//emit format messages for titles (fmtTitle)
  >;

  template<typename O>
  using TitleWrap=TitleWrapFmt<O>;

  template<typename O>
  using SingleLineMenu=SingleLineScrollCtrl<O,1>;

  using LCDFmt = Menu::Chain<//wrap inner types
    DebugFmt,//add debug info when enabled
    TextCursorFmt,//signal selected option on text mode
    CursorPosFmt,//cursor control, change line at item end
    TitleWrap,//wrap title in []
    FullPrinter,//print inner then options
    SingleLineMenu,//scroll on every move (text lines)
    NavNode//flat navigation control (no sub menus)
  >;

};//Menu

template<typename LCD,LCD& lcd,typename Panel=Menu::StaticPanel<0,0,16,2>,typename Parts=Menu::LCDParts>
using LCDOutDev=Menu::LCDOutDef<
  Parts,
  LCD,
  lcd,
  Menu::Viewport<Panel>
>;
