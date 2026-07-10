/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 codeCtrl.ino compat-macro port — "user code ctrl command":
 *        drives navigation directly via API calls from custom button
 *        -polling code, instead of a real input-device driver. See the
 *        original at github.com/neu-rah/ArduinoMenu examples/codeCtrl/
 *        codeCtrl/codeCtrl.ino.
 *
 * Pure assembly, no new compat-layer machinery — every macro/mechanism
 * this file needs already exists in am4.h (TOGGLE/SELECT/CHOOSE/VALUE,
 * EDIT, FIELD, OP's 1/2/3-arg auto-dispatch, NAVROOT_IDLE's manual
 * idleOn/idleOff). This is also the *most* faithful port of AM4's own
 * "user code ctrl command" lesson so far: AM4's own header comment says
 * this mode's whole point is "calling doNav ... allows you to implement
 * ANY input device" — OneMenu's `nav.up()`/`nav.enter()` (used directly in
 * loop() below, from plain digitalRead()-polled buttons) already *are*
 * that same "drive nav via direct API calls from your own input code"
 * capability, no translation needed at all.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `showEvent`/`action2` (real AM4 signature: 1-arg, `result(eventMask)`)
 *    ported as `bool(EventMask,IItem&)` (2-arg, unused `IItem&`) — the
 *    *existing* `EventActionItem` branch already gives this full event
 *    dispatch with zero functional loss, same "trim the handler shape"
 *    precedent as `serialio`'s own `action1`.
 *  - `myLedOn`/`myLedOff` (real AM4 signature: 0-arg) ported as `bool(int)`,
 *    same established adapter shape as `Confirm`'s `systemExit`.
 *  - `altOP(altPrompt,"",...)` inside `subMenu` dropped — custom-prompt
 *    subclass, same precedent as `handlers`/`serialio`'s own `altOP` drops.
 *  - The manual `stringIn`/direct-`doInput()` demo (reading `Serial`,
 *    writing it to a `stringIn` buffer, then calling `nav.doInput(strIn)`
 *    by hand) is dropped, not translated — the original's own comments
 *    call it out as a *secondary*, tangential demonstration ("but here we
 *    demonstrate the use of stringIn in direct call") layered on top of
 *    the real serial input `NAVROOT` already provides, not the example's
 *    core lesson (which is the button-driven `nav.up()`/`nav.enter()`
 *    calls, faithfully kept below).
 *  - `nav.idleOn(alert)` (manual-only — this example, unlike `serialio`/
 *    `CustomPrompt`, never wires `nav.timeOut`/`idleTask` at all, no
 *    auto-idle) ported via `NAVROOT_IDLE`'s manual `idleOn()`/`idleOff()`
 *    only, no `am4compat::IdleTimeout` needed.
 *
 * The selftest drives nav.up()/enter()/esc() and Run::run() directly, same
 * style as every other port in this compat layer.
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

static const char* textFilter = " .0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTWXYZ";
static const char* textMask[] = {textFilter};
char name[11] = "          ";

bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", action::action1, Menu::anyEvent)
  ,OP("Op2", action::action2, Menu::enterEvent)
  ,EDIT("Name", name, textMask, action::noOp, Menu::noEvent, Menu::noStyle)
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

// deterministic zero-op input source — the selftest drives nav directly,
// same as the original's own button-polling loop() drives nav directly.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);

// manual-only idle (no nav.timeOut/idleTask in the original — see file
// header comment) — still needs a Run type for NAVROOT_IDLE's idleOn()/
// idleOff() to have something to swap.
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

  // index 0=Op1,1=Op2,2=Name,3=Test,4=subMenu,5=setLed,6=LEDOn,7=LEDOff,
  // 8=selMenu,9=chooseMenu,10=Alert,11=<Back>
  // (Cmd::Up increments the selected index, Cmd::Down decrements — same
  // "user code ctrl" mapping the original's own NAV_BTN uses via doNav(upCmd).)

  // ── Op1 (real 3-arg) / Op2 (1-arg trimmed to 2-arg) ──────────────────────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = Op1
  assert(action::op1Count == 1 && "action1's real 3-arg shape did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (Op2)
  nav.enter();
  assert(action::op2Count == 1 && "action2's 2-arg trim did not fire on Enter");

  // ── EDIT() Name — single shared validator, cycled pos%N (same shape
  // TextField.ino/serialio's own Name field already proved) ───────────────
  nav.up();     // Op2(1) -> Name(2)
  assert(strcmp(name, "          ")==0);
  nav.enter();  // opens edit mode
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.down();   // cycle position 0's char
  assert(name[0] != ' ' && "EDIT()'s validator did not cycle position 0");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  // ── subMenu: OP()'s 2-arg trim (showEvent) fires on Enter ────────────────
  nav.up(); nav.up();  // Name(2) -> Test(3) -> subMenu(4)
  nav.enter();
  assert(nav.level() == 1 && "subMenu did not open");
  nav.enter();  // Sub1 (index 0 inside subMenu)
  assert(action::subCount == 1 && "Sub1's 2-arg showEvent did not fire on Enter");
  nav.esc();
  assert(nav.level() == 0);

  // ── TOGGLE ────────────────────────────────────────────────────────────
  assert(ledCtrl == -1);
  nav.up();     // subMenu(4) -> setLed(5)
  nav.enter();
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to and sync the next VALUE()");

  // ── LED On/Off (0-arg AM4 handlers ported as bool(int)) ─────────────────
  assert(action::ledOnCount == 0);
  nav.up();     // setLed(5) -> LEDOn(6)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up();     // LEDOn(6) -> LEDOff(7)
  nav.enter();
  assert(action::ledOffCount == 1);

  // ── Alert test: manual-only idle — doAlert's Enter triggers
  // nav.idleOn(alertRun); alertRun terminates deterministically and calls
  // Run::idleOff() itself (no am4compat::IdleTimeout — there's no
  // auto-timeout in the original). ─────────────────────────────────────
  nav.up(); nav.up(); nav.up();  // LEDOff(7) -> selMenu(8) -> chooseMenu(9) -> Alert(10)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: codeCtrl.ino compat-macro port (button-driven nav.up()/enter(), manual idle) verified\n");
  return 0;
}
