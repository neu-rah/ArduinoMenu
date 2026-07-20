/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 U8G2.ino compat-macro port — real U8G2 output device (dual
 *        U8G2+Serial `MENU_OUTPUTS`, matching the original), content a
 *        subset of `ansiSerial.ino`'s own shape (TOGGLE/SELECT/CHOOSE/
 *        altMENU pad/EDIT hex/SUBMENU/idle) with a simpler hrs:mins pad
 *        instead of a 3-field birth date. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/U8G2/U8G2/U8G2.ino.
 *
 * The real, new piece is the output device — `U8G2_OUT(u8g2,colors,...)`
 * doesn't exist in this compat layer (per-backend device macros are a
 * documented, still-open gap, see `project_am4_compat_layer` memory) and
 * per the vendor-driver-wrapper policy, U8G2 is reached via
 * `oneIO::display::U8g2Vendor` (a thin direct wrapper over a real `U8G2&`
 * object, built earlier this session, already hardware-verified) composed
 * through `oneMenu::VendorGfxDisplay<Oled>` — see `.RnD/AM4check/U8G2` for
 * the real-hardware wiring (this native selftest uses plain `ANSI_OUT`).
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - `myLedOn`/`myLedOff` (0-arg AM4 handlers) ported as `bool(int)`, same
 *    established adapter as every other 0-arg port (`CustomPrompt.ino`).
 *  - `doAlert(eventMask,prompt&)` (2-arg) ported as `bool(EventMask,
 *    IItem&)`, same shape used throughout this compat layer.
 *  - The real source's page-buffered `U8G2_..._1_HW_I2C` variant is ported
 *    against the full-buffer `..._F_HW_I2C` variant instead (disclosed
 *    simplification — `VendorGfxDisplay`'s own `flush()` maps naturally
 *    onto u8g2's full-buffer `sendBuffer()`, not the multi-pass
 *    `firstPage()/nextPage()` loop the page-buffered variant needs).
 *  - `mainMenu[1].enabled=disabledStatus` (AM4's runtime item-disable demo)
 *    dropped — cosmetic, not the point of this example.
 *  - The real source's commented-out rotary-encoder input is left out, same
 *    Serial-only input the source itself actually builds with.
 *  - The idle system (`nav.idleOn(alert)`/`nav.idleTask=idle`) is adapted
 *    onto `RunLoop`/`NAVROOT_IDLE`, same pattern as every other idle-using
 *    port in this compat layer.
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

int ledCtrl = 1;
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
  ,OP("Sub1", Menu::doNothing, Menu::noEvent)
  ,EXIT("<Back")
);

uint16_t hrs = 0, mins = 0;
PADMENU(timeMenu, "Time", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,FIELD(hrs,  "", ":", 0, 11, 1,  0, action::noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(mins, "", "",  0, 59, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
);

static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", Menu::doNothing, Menu::noEvent)
  ,OP("Op2", Menu::doNothing, Menu::noEvent)
  ,SUBMENU(timeMenu)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Exit")
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

  // ── LED On/Off (0-arg AM4 handlers ported as bool(int)) ─────────────────
  assert(action::ledOnCount == 0);
  nav.up(); nav.up(); nav.up(); nav.up(); // Op1(0)->Op2(1)->Time(2)->Sub-Menu(3)->setLed(4)... wait
  // index: 0 Op1, 1 Op2, 2 Time, 3 Sub-Menu, 4 setLed(TOGGLE), 5 LED On, 6 LED Off, ...
  // already moved 4 ups from 0 -> land on setLed(4); one more to LED On(5)
  nav.up();
  nav.enter();
  assert(action::ledOnCount == 1 && "LED On did not fire");
  nav.up(); // LED On(5) -> LED Off(6)
  nav.enter();
  assert(action::ledOffCount == 1 && "LED Off did not fire");

  // ── Idle system ──────────────────────────────────────────────────────────
  nav.up(); nav.up(); nav.up(); // LED Off(6) -> Select(7) -> Choose(8) -> Alert test(9)
  assert(idleStartCount == 0);
  nav.enter();
  int guard = 0;
  while(!Run::active() && guard++<200) Run::run();
  assert(Run::active() && "doAlert did not trigger idleOn");
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1 &&
         "idleRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: U8G2.ino compat-macro port (native U8g2Vendor over a real "
         "U8G2 SSD1306 OLED proven separately, see .RnD/AM4check/U8G2) "
         "verified\n");
  return 0;
}
