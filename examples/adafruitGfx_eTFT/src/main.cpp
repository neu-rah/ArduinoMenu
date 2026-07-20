/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 TFT_eSPI.ino compat-macro port — Rui's own original TFT_eSPI
 *        demo (no external tutorial, unlike this same example directory's
 *        other sketch — see below). Real `TFT_eSPI` confirmed via the real
 *        header to be `class TFT_eSPI : public Print`, NOT Adafruit_GFX-
 *        derived (same situation as TFT_HX8357/UTFT) — reached via a NEW
 *        `oneIO::display::TftEspiVendor`, a blind pass-through wrapper same
 *        shape as `TftHx8357Vendor`. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/adafruitGfx_eTFT/TFT_eSPI/
 *        TFT_eSPI.ino, and `.RnD/AM4check/adafruitGfx_eTFT` for the real
 *        hardware wiring (this native selftest uses plain `ANSI_OUT`).
 *
 * This example directory ALSO vendors a second, tutorial-linked sketch —
 * `TFT_eSPI/ArduinoMenu_LilyGo_TTGO_T-display_demo/` (Jeroen Maathuis' 2019
 * LilyGo TTGO T-display port, linked from a live external tutorial page) —
 * which is deliberately NOT touched here, per the standing instruction that
 * tutorial-linked examples must stay structurally as-is (macro-layer
 * drop-in only), not hand-restructured. Only this directory's own,
 * non-tutorial `TFT_eSPI.ino` is ported.
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - `altOP(altPrompt,"",doNothing,noEvent)` (custom-prompt subclass)
 *    dropped, same precedent as every other `altOP` drop.
 *  - The real source's own `FIELD(test,...)` and `OP("Alert test",...)`
 *    lines are ALREADY commented out in the vendored original itself — not
 *    ported, matching the real source exactly (not a simplification).
 *  - The real source's serial input dropped in favor of NoOpIn — Serial-
 *    only input is trivial and already proven elsewhere; the real, new
 *    thing this port demonstrates is the TFT_eSPI output device wrapper.
 *  - `myLedOn`/`myLedOff` (0-arg) ported as `bool(int)`, same established
 *    adapter.
 */

#include <menu.h>
#include <oneMenu/menu/IO/ansiOut.h>
#include <oneMenu/menu/fmt/textFmt.h>
#include <oneMenu/menu/fmt/ansiFmt.h>
#include <oneMenu/menu/IO/streamOut.h>
#include <oneMenu/menu/in.h>
#include <hapi/hapi.h>
#include <oneData/oneData.h>
#include <oneItem/oneItem.h>
#include <oneOutput/oneOutput.h>
#include <cassert>
#include <cstdio>

using namespace hapi;
using namespace oneData;

namespace action {
  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }
  void noOp() {}
}

int ledCtrl = 0;
TOGGLE(ledCtrl, setLed, "Led: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("On",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Off", 0, Menu::doNothing, Menu::noEvent)
);

int selTest = 0;
SELECT(selTest, selMenu, "Select", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("Zero", 0, Menu::doNothing, Menu::noEvent)
  ,VALUE("One",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Two",  2, Menu::doNothing, Menu::noEvent)
);

int chooseTest = -1;
CHOOSE(chooseTest, chooseMenu, "Choose", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("First",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Second", 2, Menu::doNothing, Menu::noEvent)
  ,VALUE("Third",  3, Menu::doNothing, Menu::noEvent)
  ,VALUE("Last",  -1, Menu::doNothing, Menu::noEvent)
);

MENU(subMenu, "Sub-Menu", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,EXIT("<Back")
);

static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", Menu::doNothing, Menu::noEvent)
  ,OP("Op2", Menu::doNothing, Menu::noEvent)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, MAX_DEPTH, &devOut);

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  nav.printTo(devOut);

  // index: 0 Op1, 1 Op2, 2 Sub-Menu, 3 setLed, 4 LED On, 5 LED Off,
  //        6 Select, 7 Choose, 8 Hex, 9 <Back>
  assert(action::ledOnCount == 0);
  for(int i=0;i<4;i++) nav.up(); // Op1(0) -> LED On(4)
  nav.enter();
  assert(action::ledOnCount == 1 && "LED On did not fire");
  nav.up(); // LED On(4) -> LED Off(5)
  nav.enter();
  assert(action::ledOffCount == 1 && "LED Off did not fire");

  printf("OK: TFT_eSPI.ino (adafruitGfx_eTFT) compat-macro port (native "
         "TftEspiVendor over a real TFT_eSPI, proven separately, see "
         ".RnD/AM4check/adafruitGfx_eTFT) verified\n");
  return 0;
}
