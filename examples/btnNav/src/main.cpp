/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 btnNav.ino compat-macro port — "Digital keypad (3 buttons)
 *        using the menu keyIn driver": output to Serial, input from 3 real
 *        buttons (via AM4's real `Menu::keyIn<3>`, `menuIO/altKeyIn.h`
 *        variant) *and* Serial, combined. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/btnNav/btnNav/btnNav.ino.
 *
 * Pure assembly — every macro/mechanism this file needs already exists:
 * TOGGLE/SELECT/CHOOSE/VALUE, FIELD, OP's 1/2/3-arg auto-dispatch,
 * NAVROOT_IDLE's manual idleOn/idleOff (am4.h), plus
 * `am4compat::MenuInBridge` (input, `compat/am4/menuInBridge.h`) — already
 * built for exactly this driver. This file (the portable, native-testable
 * menu logic) never touches the bridge header directly — same split as
 * every other bridge-touching port: the *real* `Menu::keyIn<3>` wiring
 * lives only in the AVR-only harness (`.RnD/AM4check/btnNav`), since that
 * header has no native build path (Arduino-only).
 *
 * Real finding while researching this port: the real source `#include`s
 * `menuIO/altKeyIn.h`, not `menuIO/keyIn.h` — both define a `class
 * keyIn<N>` behind the *same* include guard (`RSITE_KEYIN_DEF_H`), so
 * they're mutually exclusive per translation unit; whichever is `#include`d
 * first wins. `altKeyIn.h`'s own `keyIn<N>` uses a 3-field `keyMap{pin,
 * code,mode}` (an explicit `pinMode` per key, `altKeyMapDef.h`) instead of
 * `keyIn.h`'s 2-field version with hardcoded pull-up-on-negative-pin logic
 * — but it's still `Menu::menuIn`-derived, still reads bytes via `Menu::
 * defaultNavCodes`, so `MenuInBridge` needed zero changes to support it —
 * the AVR harness just `#include`s the matching header, confirming the
 * bridge design covers *both* `keyIn` variants, not just the one already
 * proven in `menuInBridge.h`'s own original AVR check.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `action1` kept genuinely 3-arg (real AM4 signature:
 *    `result(eventMask,navNode&,prompt&)`) — today's real
 *    `EventActionItemNav` branch, byte-for-byte the same shape the original
 *    already used.
 *  - `action2`/`showEvent` (real AM4 signature: 1-arg, `result(eventMask)`)
 *    trimmed to `bool(EventMask,IItem&)` (2-arg, unused `IItem&`) — the
 *    *existing* `EventActionItem` branch, same precedent as `codeCtrl`'s own
 *    `action2`/`showEvent`. `subMenu`'s own `MENU()` title fn is `doNothing`
 *    in the real source (not `showEvent`) — kept as-is, no title-level
 *    dispatch needed here.
 *  - `myLedOn`/`myLedOff` (real AM4 signature: 0-arg) ported as `bool(int)`,
 *    same established adapter shape as `Confirm`'s `systemExit`.
 *  - `altOP(altPrompt,"",...)` inside `subMenu` dropped — custom-prompt
 *    subclass, same precedent as every other `altOP` drop this session.
 *  - AM4's own `chainStream<2>` combining `keyIn`+`serialIn` is replaced
 *    with `MENU_INPUTS`'s own `InGroup` combining a `MenuInBridge`-wrapped
 *    button source with OneMenu's *native* serial input
 *    (`ArduinoSerialIn`/`IdxParser`/`PCKbd`) — per the established decision
 *    that AM4-driver bridging is for hardware OneMenu has no native
 *    equivalent for; serial input already does.
 *  - Manual-only idle (`nav.idleOn(alert)`, no `nav.timeOut` wired in the
 *    original) — `NAVROOT_IDLE`'s manual `idleOn()`/`idleOff()` alone, same
 *    shape `codeCtrl`/`LiquidCrystal`'s own idle systems already used.
 *
 * The selftest drives nav.up()/enter()/esc() and Run::run() directly, same
 * style as every other port in this compat layer — the real button/serial
 * hardware is only exercised (compile-only) in the AVR-only harness.
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
  int op1Count = 0;
  bool action1(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { op1Count++; return true; }

  int op2Count = 0;
  bool action2(oneMenu::EventMask, oneMenu::IItem&) { op2Count++; return true; }

  // showEvent (real AM4 signature: 1-arg, `result(eventMask)`, used only by
  // Sub1/Sub2/Sub3's OP()) ported as bool(EventMask,IItem&) — the *existing*
  // EventActionItem branch, same trim as action2 below and codeCtrl's own
  // showEvent. subMenu's own MENU() title fn is `doNothing` in the real
  // source (not showEvent) — no title-level dispatch here.
  int subCount = 0;
  bool showEvent(oneMenu::EventMask, oneMenu::IItem&) { subCount++; return true; }

  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }

  void noOp() {}
}

int test = 55;

int ledCtrl = -1;
TOGGLE(ledCtrl, setLed, "Led: ", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,VALUE("On", 1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Off", 0, Menu::doNothing, Menu::noEvent)
);

int selTest = 0;
SELECT(selTest, selMenu, "Select", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,VALUE("Zero", 0, Menu::doNothing, Menu::noEvent)
  ,VALUE("One",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Two",  2, Menu::doNothing, Menu::noEvent)
);

int chooseTest = -1;
CHOOSE(chooseTest, chooseMenu, "Choose", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,VALUE("First",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Second", 2, Menu::doNothing, Menu::noEvent)
  ,VALUE("Third",  3, Menu::doNothing, Menu::noEvent)
  ,VALUE("Last",  -1, Menu::doNothing, Menu::noEvent)
);

// altOP(altPrompt,"",showEvent,enterEvent) dropped — custom-prompt subclass,
// see file header comment.
MENU(subMenu, "Sub-Menu", Menu::doNothing, Menu::anyEvent, Menu::wrapStyle
  ,OP("Sub1", action::showEvent, Menu::enterEvent)
  ,OP("Sub2", action::showEvent, Menu::enterEvent)
  ,OP("Sub3", action::showEvent, Menu::enterEvent)
  ,EXIT("<Back")
);

bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", action::action1, Menu::anyEvent)
  ,OP("Op2", action::action2, Menu::enterEvent)
  ,FIELD(test, "Test", "%", 0, 100, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EXIT("<Back")
);

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// the real button/serial input only exists in the AVR-only harness.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);

// manual-only idle (no nav.timeOut in the original).
bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;

NAVROOT_IDLE(nav, mainMenu, /*maxDepth*/2, in, out, Run);

int alertStartCount = 0, alertEndCount = 0, alertTicks = 0;
bool alertRun() {
  if(alertTicks==0) {
    alertStartCount++;
    printf("alert test\n[select] to continue...\n");
  }
  alertTicks++;
  if(alertTicks>=3) {
    alertEndCount++;
    alertTicks=0;
    Run::idleOff();
  }
  return true;
}

bool mainRun() { return nav.poll(); }

bool doAlert(oneMenu::EventMask e, oneMenu::IItem&) {
  if(e & oneMenu::EventMask::Enter) nav.idleOn(alertRun);
  return true;
}

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=Op1,1=Op2,2=Test,3=subMenu,4=setLed,5=LEDOn,6=LEDOff,7=selMenu,
  // 8=chooseMenu,9=Alert,10=<Back>

  // ── Op1 (real 3-arg) / Op2 (1-arg trimmed to 2-arg) ──────────────────────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = Op1
  assert(action::op1Count == 1 && "action1's real 3-arg shape did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (Op2)
  nav.enter();
  assert(action::op2Count == 1 && "action2's 2-arg trim did not fire on Enter");

  // ── subMenu: OP()'s 2-arg trim (showEvent) fires on Enter ────────────────
  nav.up(); nav.up();  // 1 -> 2(Test) -> 3(subMenu)
  nav.enter();
  assert(nav.level() == 1 && "subMenu did not open");
  nav.enter();  // Sub1 (index 0 inside subMenu)
  assert(action::subCount == 1 && "Sub1's 2-arg showEvent did not fire on Enter");
  nav.esc();
  assert(nav.level() == 0);

  // ── TOGGLE ────────────────────────────────────────────────────────────
  assert(ledCtrl == -1);
  nav.up();     // subMenu(3) -> setLed(4)
  nav.enter();
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to and sync the next VALUE()");

  // ── LED On/Off (0-arg AM4 handlers ported as bool(int)) ─────────────────
  assert(action::ledOnCount == 0);
  nav.up();     // setLed(4) -> LEDOn(5)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up();     // LEDOn(5) -> LEDOff(6)
  nav.enter();
  assert(action::ledOffCount == 1);

  // ── Alert test: manual-only idle ─────────────────────────────────────
  nav.up(); nav.up(); nav.up();  // LEDOff(6) -> selMenu(7) -> chooseMenu(8) -> Alert(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: btnNav.ino compat-macro port (TOGGLE/SELECT/CHOOSE/subMenu/idle verified;"
         " real 3-button + serial input is AVR-only, see .RnD/AM4check/btnNav)\n");
  return 0;
}
