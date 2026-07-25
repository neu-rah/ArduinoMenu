/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 Blink.ino compat-macro port — the simplest possible menu (also
 *        this repo's own README example): two FIELD()s (timeOn/timeOff)
 *        driving a blink() helper. See github.com/neu-rah/ArduinoMenu
 *        examples/Blink/Blink/Blink.ino.
 *
 * Pure assembly — every macro this needs (MENU/FIELD/EXIT, MENU_INPUTS/
 * MENU_OUTPUTS/NAVROOT) already exists in am4.h; no new compat-layer
 * machinery needed for this port.
 *
 * Deliberate deviations from the original, beyond the usual #include swap:
 *  - SERIAL_OUT(Serial)/serialIn(Serial) swapped for ANSI_OUT + a
 *    deterministic NoOpIn — no real Serial under `platform=native`, same
 *    convention as every other port in this directory (see
 *    examples/serialio's own header comment).
 *  - LEDPIN/pinMode/digitalWrite dropped — no real hardware in this
 *    native-only selftest; blink() itself is kept and exercised directly
 *    against explicit tick values instead of millis().
 *  - FIELD()'s step/tune params are accepted but ignored (am4.h's own doc
 *    comment: "value always steps by 1") — same already-established gap as
 *    examples/serialio's own birthDate FIELD()s.
 *  - FIELD()'s fn slot wants a real, non-overloaded void() — Menu::doNothing
 *    is bool(int), doesn't fit — same constraint every prior FIELD() port
 *    already needed (see examples/serialio's own action::noOp).
 *
 * The selftest drives nav.up()/enter() directly, same verification style as
 * examples/{am4compat,serialio,fullIdle}'s own selftests.
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

unsigned int timeOn=10;
unsigned int timeOff=90;

void noOp() {}

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,1000,10,1, noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,10000,10,1,noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

#define MAX_DEPTH 1

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

// same shape as the original Blink.ino's blink(), driven by an explicit
// tick value here instead of millis() (no real clock in this selftest).
bool blink(unsigned long tick, unsigned int timeOn, unsigned int timeOff) {
  return tick % (unsigned long)(timeOn+timeOff) < (unsigned long)timeOn;
}

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=timeOn, 1=timeOff, 2=<Back>
  assert(timeOn == 10);
  nav.enter();  // open timeOn edit
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();     // Up increases in edit mode
  assert(timeOn == 11 && "timeOn FIELD did not edit");
  nav.enter();  // commit
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  nav.up();     // timeOn(0) -> timeOff(1)
  assert(timeOff == 90);
  nav.enter();
  nav.up();
  assert(timeOff == 91 && "timeOff FIELD did not edit");
  nav.enter();

  assert(blink(0, timeOn, timeOff) == true);
  assert(blink(timeOn, timeOn, timeOff) == false);
  assert(blink(timeOn+timeOff-1, timeOn, timeOff) == false);
  assert(blink(timeOn+timeOff, timeOn, timeOff) == true);

  printf("OK: Blink.ino compat-macro port — FIELD() editing + blink() timing verified\n");
  return 0;
}
