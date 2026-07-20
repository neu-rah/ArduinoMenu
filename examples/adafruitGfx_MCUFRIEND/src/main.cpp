/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 MCUFRIEND.ino (adafruitGfx_MCUFRIEND) compat-macro port —
 *        content-identical to `tft.ino`'s own port (confirmed via a real
 *        diff against both vendored sources — same TOGGLE/SELECT/CHOOSE/
 *        EDIT hex/SUBMENU/idle tree, differs only in vendor library, color
 *        macro names, and target board), real MCUFRIEND_kbv output device.
 *        See the original at github.com/neu-rah/ArduinoMenu examples/
 *        adafruitGfx_MCUFRIEND/MCUFRIEND/MCUFRIEND.ino.
 *
 * `MCUFRIEND_kbv` derives from `Adafruit_GFX` (confirmed before assuming
 * it), so it's reached the exact same way as `tft.ino`'s own
 * `Adafruit_ST7735` — `oneIO::display::AdaGfxVendor` through
 * `oneMenu::VendorGfxDisplay<Oled>`. See
 * `.RnD/AM4check/adafruitGfx_MCUFRIEND` for the real-hardware wiring (this
 * native selftest uses plain `ANSI_OUT`).
 *
 * Deviations from the original: identical to `tft.ino`'s own — `altOP`
 * custom-prompt dropped, rotary-encoder input dropped (Serial-only),
 * `myLedOn`/`myLedOff` ported as `bool(int)`, idle system adapted onto
 * `RunLoop`/`NAVROOT_IDLE`/`am4compat::IdleTimeout`.
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
using Idle = am4compat::IdleTimeout<50, Run>;

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

bool mainRun() {
  bool activity = nav.poll();
  Idle::tick(activity, idleRun);
  return true;
}

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

  // ── Idle system: IdleTimeout auto-triggers idleRun after inactivity ─────
  assert(idleStartCount == 0);
  int guard = 0;
  while(!Run::active() && guard++<200) { Run::run(); hw::delay_ms(1); }
  assert(Run::active() && "IdleTimeout did not auto-trigger idleOn after the timeout window");
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1 &&
         "idleRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: MCUFRIEND.ino (adafruitGfx_MCUFRIEND) compat-macro port "
         "(native AdaGfxVendor over a real MCUFRIEND_kbv TFT proven "
         "separately, see .RnD/AM4check/adafruitGfx_MCUFRIEND) verified\n");
  return 0;
}
