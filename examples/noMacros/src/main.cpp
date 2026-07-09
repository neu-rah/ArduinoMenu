/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4's noMacros.ino, rebuilt on OneMenu's own native machinery
 *        (2026-07-09) — not translated through the AM4-compat macro layer at
 *        all, deliberately: the original demonstrates that AM4's macros are
 *        just sugar over hand-constructible objects (raw prompt/menuNode/
 *        navRoot instances). OneMenu's own "no macros" story is its native
 *        component composition (ItemDef/menuDef/staticBody/INavDef) — the
 *        actual generic-purpose objects am4.h's MENU/OP/EXIT macros
 *        themselves expand into. There's no OneMenu equivalent of AM4's raw
 *        prompt/menuNode/navRoot class hierarchy to hand-construct (the
 *        compat layer only shims the macro layer, not that hierarchy), so
 *        this isn't a line-for-line translation of the original — it's the
 *        same *idea* ("build the menu without macros") expressed in
 *        OneMenu's own terms.
 *
 * Real Serial/Arduino target (matching the original, and Blink/Button) —
 * doInput()/doOutput() driven directly in loop(), not through am4.h's
 * Pool/NAVROOT convenience (which itself only exists via the macro layer)
 * — same "by hand, no macros" spirit for the I/O cycle too.
 */

#include <oneMenu/oneMenu.h>
#include <oneMenu/menu/IO/arduino/serialIn.h>
#include <oneMenu/menu/IO/arduino/serialOut.h>
#include <oneMenu/menu/fmt/textFmt.h>
#include <oneMenu/menu/IO/idParser.h>
#include <oneMenu/menu/IO/pcKbdIn.h>
#include <hapi/hapi.h>
#include <oneData/oneData.h>
#include <oneItem/oneItem.h>
#include <oneOutput/oneOutput.h>

using namespace hapi;
using namespace oneData;
using namespace oneMenu;
// (no AM4 macros/Menu:: shim involved in this file at all, so no collision
// risk — using namespace oneMenu; is safe here, unlike the am4compat/
// handlers examples.)

void op1Func();
void op2Func();

namespace action {
  bool op1(Sz) { op1Func(); return true; }
  bool op2(Sz) { op2Func(); return true; }
}

// the menu, built directly — no MENU()/OP() macros
auto mainMenu = menuDef<WrapNav>(
  ItemDef<Text>{"Main menu"},
  staticBody(
    ItemDef<Action<action::op1>, Text>{"Op 1"},
    ItemDef<Action<action::op2>, Text>{"Op 2"}
  )
);

#define MAX_DEPTH 1

// input/output devices, built directly — same compile-time chain shape
// am4.h's own serialIn/SERIAL_OUT macros assemble internally (see am4.h),
// just spelled out here instead of hidden behind a macro.
InDef<SerialIn, IdParser, PCKbd> in;
OutDef<
  FullPrinter, TextFmt, DataParser<>, CtrlChars,
  Cursor<>, Gate, SerialOut,
  StaticPos<0,0>, StaticArea<40,6>
> out;

// nav root, built directly — no NAVROOT() macro
INavDef<TreeNav, Root<decltype(mainMenu), mainMenu>> nav;

void op1Func() { Serial.println("Op 1 executed"); }
void op2Func() { Serial.println("Op 2 executed"); }

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu initialized without macros (OneMenu native).");
  nav.printTo(out);
}

void loop() {
  // no Pool/nav.poll() (that convenience is itself only reachable through
  // am4.h) — doInput()/doOutput() driven by hand, matching the original's
  // "no macros" spirit for the I/O cycle too.
  in.doInput(nav, MAX_DEPTH);
  nav.doOutput(out);
  delay(100);
}
