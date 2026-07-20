/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 lcdMono.ino (adafruitGfx_lcdMono) compat-macro port — real
 *        Adafruit_PCD8544 (Nokia 5110) output device, simpler menu than
 *        `tft.ino`'s own port (TOGGLE/FIELD/SUBMENU(setLed)/LED on-off/
 *        idle, no SELECT/CHOOSE/second sub-menu). See the original at
 *        github.com/neu-rah/ArduinoMenu examples/adafruitGfx_lcdMono/
 *        lcdMono/lcdMono.ino.
 *
 * Real, disclosed gap found building this one, not present in `tft.ino`'s
 * own port: `Adafruit_PCD8544` (like `Adafruit_SSD1306`) is genuinely
 * double-buffered — it needs an explicit `display()` call after drawing,
 * which `Adafruit_GFX`'s own base class has no virtual method for (each
 * concrete driver defines its own, non-virtually) — `oneIO::display::
 * AdaGfxVendor`'s own header comment already flagged this exact case as
 * unsupported by its polymorphic-`Adafruit_GFX&`-bound design. Fixed by
 * adding `oneIO::display::AdaGfxBufferedVendor<VendorT,...>` (same file),
 * bound to the CONCRETE vendor type instead of the base, so `flush()` can
 * call `driver->display()` directly — same "flush() reaches the real
 * buffered vendor call" shape `U8g2Vendor`'s own `flush()`->`sendBuffer()`
 * already uses. See `.RnD/AM4check/adafruitGfx_lcdMono` for the real
 * hardware wiring (this native selftest uses plain `ANSI_OUT`).
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - `myLedOn`/`myLedOff` (0-arg) ported as `bool(int)`.
 *  - The real source's rotary-encoder input dropped, Serial-only, same
 *    "focus the new capability" precedent as every other GFX-device port
 *    this round.
 *  - The idle system (`nav.idleOn(alert)`/`nav.idleTask=idle`) is adapted
 *    onto `RunLoop`/`NAVROOT_IDLE`, same pattern as every other idle-using
 *    port.
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
// NOTE: deliberately no `using namespace oneMenu;` — see examples/am4compat's
// own comment for why (oneMenu::Menu<> collides with AM4's Menu namespace).

namespace action {
  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }
  void noOp() {}
}

int test = 55;
int ledCtrl = 0;

TOGGLE(ledCtrl, setLed, "Led: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("On",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Off", 0, Menu::doNothing, Menu::noEvent)
);

bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(test, "Test", "%", 0, 100, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav/Run directly.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, MAX_DEPTH, &devOut);

bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;

NAVROOT_IDLE(nav, mainMenu, MAX_DEPTH, in, out, Run);

int idleStartCount = 0, idleEndCount = 0, idleTicks = 0;

bool idleRun() {
  idleTicks++;
  if(idleTicks==1) idleStartCount++;
  if(idleTicks>=3) {
    idleEndCount++;
    idleTicks = 0;
    Run::idleOff();
  }
  return true;
}

bool mainRun() { return nav.poll(); }

bool doAlert(oneMenu::EventMask e, oneMenu::IItem&) {
  if(e & oneMenu::EventMask::Enter) nav.idleOn(idleRun);
  return true;
}

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  nav.printTo(devOut);

  // index: 0 Test, 1 setLed, 2 LED On, 3 LED Off, 4 Alert test, 5 <Back>
  assert(action::ledOnCount == 0);
  nav.up(); nav.up(); // Test(0) -> setLed(1) -> LED On(2)
  nav.enter();
  assert(action::ledOnCount == 1 && "LED On did not fire");
  nav.up(); // LED On(2) -> LED Off(3)
  nav.enter();
  assert(action::ledOffCount == 1 && "LED Off did not fire");

  nav.up(); // LED Off(3) -> Alert test(4)
  assert(idleStartCount == 0);
  nav.enter();
  int guard = 0;
  while(!Run::active() && guard++<200) Run::run();
  assert(Run::active() && "doAlert did not trigger idleOn");
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1 &&
         "idleRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: lcdMono.ino (adafruitGfx_lcdMono) compat-macro port (native "
         "AdaGfxBufferedVendor over a real Adafruit_PCD8544 Nokia 5110 "
         "proven separately, see .RnD/AM4check/adafruitGfx_lcdMono) "
         "verified\n");
  return 0;
}
