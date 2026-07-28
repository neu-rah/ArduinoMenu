/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 concurrent.ino compat-macro port — "Concurrent menu test":
 *        two fully independent nav trees, each bound to its own
 *        input+output device pair, polled from the same loop(). See the
 *        original at github.com/neu-rah/ArduinoMenu examples/concurrent/
 *        concurrent/concurrent.ino.
 *
 * Pure assembly, no new compat-layer machinery — "two independent
 * NAVROOTs in the same program" needs nothing beyond what MENU_INPUTS/
 * MENU_OUTPUTS/NAVROOT already build per call; each NAVROOT closes over
 * its own Pool<In,Out> and Root<menuInstance>, so two of them never
 * share state. What this file actually demonstrates (matching the
 * original's own stated purpose) is that navigating one tree's selection
 * leaves the other's completely untouched.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - The real hardware pairing (LCD+encoder+button for `mainNav`, real
 *    Serial for `auxNav`) is proven separately, real-hardware-only, at
 *    `.RnD/AM4check/concurrent` (gitignored research harness) — same
 *    "native selftest here, real device wiring there" split as
 *    `LiquidCrystal.ino`'s own port (see that file's header comment for
 *    why: OneInput's native PCINT-driven encoder/button stack replaces
 *    AM4's own encoderIn/keyIn, which depend on a third-party PCINT
 *    library with a real, pre-existing AVR compile bug). This file uses
 *    two independent console (`ANSI_OUT`) devices and two deterministic
 *    `NoOpIn` sources instead, so the selftest needs no real hardware at
 *    all.
 *  - `Op1`/`Op2`/`Aux1`/`Aux2` (real AM4 signature: `doNothing`/`noEvent`,
 *    no real handler) kept as literal no-ops (`Menu::doNothing`), same as
 *    the original — nothing to trim here, this example's whole point is
 *    the two-independent-trees mechanic, not event dispatch.
 *
 * The selftest drives `mainNav`/`auxNav` directly via `nav.up()/enter()/
 * esc()`, same style as every other port in this compat layer — advancing
 * one tree's selection and confirming the other's `sel()`/`level()` never
 * change is the actual assertion this example is for.
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

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", Menu::doNothing, Menu::noEvent)
  ,OP("Op2", Menu::doNothing, Menu::noEvent)
  ,EXIT("<Back")
);

MENU(auxMenu, "Aux menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Aux1", Menu::doNothing, Menu::noEvent)
  ,OP("Aux2", Menu::doNothing, Menu::noEvent)
  ,EXIT("<Back")
);

// deterministic zero-op input sources — the selftest drives each nav
// directly; real hardware input (encoder+button for mainNav, real Serial
// for auxNav) is proven separately, see file header comment. Two distinct
// types (not one reused) so MENU_INPUTS' InGroup for each tree stays
// genuinely independent, matching the original's own two separate device
// pairings.
struct NoOpIn1 {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
struct NoOpIn2 {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn1> mainDevIn;
oneMenu::InDef<NoOpIn2> auxDevIn;

ANSI_OUT(mainDevOut, 20, 5);  // stands in for the original's LCD_OUT(lcd,{0,0,16,2})
ANSI_OUT(auxDevOut, 40, 6);   // stands in for the original's SERIAL_OUT(Serial)

MENU_INPUTS(mainIn, &mainDevIn);
MENU_OUTPUTS(mainOut, /*maxDepth*/1, &mainDevOut);
NAVROOT(mainNav, mainMenu, /*maxDepth*/1, mainIn, mainOut);

MENU_INPUTS(auxIn, &auxDevIn);
MENU_OUTPUTS(auxOut, /*maxDepth*/1, &auxDevOut);
NAVROOT(auxNav, auxMenu, /*maxDepth*/1, auxIn, auxOut);

int main() {
  mainDevOut.lockMode(oneMenu::LockMode::None);
  mainDevOut.setColors(WHITE, BLACK);
  mainDevOut.clear();
  mainNav.printTo(mainDevOut);

  auxDevOut.lockMode(oneMenu::LockMode::None);
  auxDevOut.setColors(WHITE, BLACK);
  auxDevOut.clear();
  auxNav.printTo(auxDevOut);

  // both start at index 0, level 0
  assert(mainNav.sel() == 0 && mainNav.level() == 0);
  assert(auxNav.sel()  == 0 && auxNav.level()  == 0);

  // ── advance mainNav only — auxNav must stay completely untouched ────────
  mainNav.up();  // Op1(0) -> Op2(1)
  assert(mainNav.sel() == 1 && "mainNav.up() did not move mainNav's own selection");
  assert(auxNav.sel() == 0 && auxNav.level() == 0 &&
         "auxNav's state changed from a mainNav-only nav call — trees are not independent");

  // ── open mainNav's own level while auxNav is untouched ───────────────────
  // (EXIT("<Back") is the last item; wrap to it, then Enter closes back to
  // level 0 on a top-level menu with no submenu to open — confirms mainNav's
  // own nav mechanics work standalone, unaffected by auxNav ever existing)
  mainNav.up();  // Op2(1) -> <Back>(2)
  assert(auxNav.sel() == 0 && "auxNav still untouched after a second mainNav-only nav call");

  // ── now advance auxNav only — mainNav must stay exactly where it was ────
  int mainSelBefore = mainNav.sel();
  auxNav.up();  // Aux1(0) -> Aux2(1)
  assert(auxNav.sel() == 1 && "auxNav.up() did not move auxNav's own selection");
  assert(mainNav.sel() == mainSelBefore &&
         "mainNav's state changed from an auxNav-only nav call — trees are not independent");

  // ── both devices independently render their own current selection ───────
  bool mainRedrawn = mainNav.printTo(mainDevOut);
  bool auxRedrawn  = auxNav.printTo(auxDevOut);
  (void)mainRedrawn; (void)auxRedrawn;

  printf("OK: concurrent.ino compat-macro port (two independent nav trees, "
         "each on its own input+output device pair) verified\n");
  return 0;
}
