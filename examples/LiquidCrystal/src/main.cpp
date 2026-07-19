/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 LiquidCrystal.ino compat-macro port — "menu output to standard
 *        arduino LCD (LiquidCrystal)", input: encoder + Serial. See the
 *        original at github.com/neu-rah/ArduinoMenu examples/LiquidCrystal/
 *        LiquidCrystal/LiquidCrystal.ino.
 *
 * This was previously blocked — not by anything in this compat layer, but
 * by a real, pre-existing bug in the third-party `neu-rah/PCINT` library
 * AM4's own `encoderIn.h`/`keyIn.h` depend on (`INT_MODE_TYPE` only
 * `#define`d in the ARM/ESP branch, referenced unconditionally on plain AVR
 * — confirmed via a full clean compile attempt, see `.RnD/AM4check/
 * LiquidCrystal-original`'s own restored-vendored-state note). Unblocked by
 * *not* bridging AM4's own encoder/LCD drivers at all: OneInput already has
 * a complete, independent quadrature-encoder-over-PCINT stack
 * (`oneInput::Encoder` + `oneInput::avr::AvrEncPins<Group,Port,BitA,BitB>`,
 * raw `PCICR`/`PCMSK` register access, zero third-party dependency) plus a
 * button-over-PCINT stack (`oneInput::BtnCapture`+`Click`+`Hold`+`Debounce`+
 * `oneInput::avr::AvrBtnPin`), and OneIO has a native parallel HD44780
 * driver (`oneIO::display::Hd44780<RS,EN,D4,D5,D6,D7>`, any 6 IOP `OutPin`
 * types — no `LiquidCrystal`/`Wire` dependency at all) wired through
 * OneMenu's own `oneMenu::LcdOut<Lcd>` (`lcdOut.h`). This is genuinely a
 * *different* mechanism from AM4's own encoder (edge-triggered PCINT vs.
 * AM4's own polled-in-ISR quadrature read) — not a translation, a native
 * replacement — but produces the same nav Up/Down/Enter/Esc capability the
 * original's encoder+button input demonstrates.
 *
 * This file (the published, native-selftest example) exercises the menu
 * logic only — same convention as `btnNav`'s own real-hardware split: the
 * real LCD+encoder+button wiring (RS=D8/PB0, EN=D9/PB1, D4-D7=D4-D7/PD4-7;
 * encA=A2/PC2, encB=A1/PC1, encBtn=A3/PC3, all PCINT1/PORTC) is proven
 * separately via a real AVR build at `.RnD/AM4check/LiquidCrystal`
 * (gitignored research harness), not duplicated into this native-only file.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `action1`/`action2`/`showEvent` (real AM4 signature: genuinely 3-arg,
 *    `result(eventMask,navNode&,prompt&)` — unlike some other AM4 examples
 *    that use a trimmed 1-arg shape for the same-named functions) all keep
 *    their real 3-arg shape here, `bool(EventMask,INav&,IItem&)`, same
 *    precedent as `btnNav`'s own `action1`/`ansiSerial`'s own `showEvent`.
 *  - `showEvent` is ALSO used, in the real source, as `subMenu`'s own
 *    `MENU()`-level title/fn parameter — but `MENU()`'s fn slot only
 *    auto-dispatches a 1-arg `bool(EventMask)` shape (see am4.h's own doc
 *    comment on `menuDefStyle`); a separate `titleEvent` (1-arg) fills that
 *    slot instead, same substitution `ansiSerial`'s own port already made
 *    for the identical situation.
 *  - `doAlert` (real 2-arg: `result(eventMask,prompt&)`) ported as
 *    `bool(EventMask,IItem&)`.
 *  - `myLedOn`/`myLedOff` (real 0-arg) ported as `bool(int)`, same
 *    established adapter shape as `Confirm`'s `systemExit`/`codeCtrl`'s own
 *    `myLedOn`/`myLedOff` — tracked via counters only (not also writing a
 *    `ledCtrl`-like variable), same simplification `codeCtrl` already made.
 *  - `altOP(altPrompt,"",showEvent,anyEvent)` inside `subMenu` dropped —
 *    custom-prompt subclass, same precedent as `handlers`/`ansiSerial`'s own
 *    `altOP` drops.
 *  - `doAlert`'s `nav.idleOn(alert)` + `nav.idleTask=idle` (the real
 *    idleStart/idling/idleEnd 3-state callback) is manual-only (no
 *    `nav.timeOut` in the original — `codeCtrl`'s own precedent) — ported
 *    via `NAVROOT_IDLE`'s manual `Run::idleOn()`/`idleOff()`, no
 *    `am4compat::IdleTimeout` needed. The 3-state split (AM4's real
 *    `idleStart`/`idling`/`idleEnd`) is approximated the same way
 *    `fullIdle`'s own `idleRun()` already does: a local tick counter
 *    distinguishes "just entered" from "still idling" from "leaving", and
 *    (since there's no real hardware Enter keypress to wait for in a
 *    native selftest) leaves idle deterministically after a fixed tick
 *    count instead of waiting for real input — same "simplify for a
 *    deterministic selftest" reasoning used throughout this compat layer.
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
  bool action2(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { op2Count++; return true; }

  int subCount = 0;
  bool showEvent(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { subCount++; return true; }

  // subMenu's own MENU()-level title fn — see file header comment on why
  // this is separate from showEvent (MENU()'s fn slot only auto-dispatches
  // the 1-arg bool(EventMask) shape).
  int titleCount = 0;
  bool titleEvent(oneMenu::EventMask) { titleCount++; return true; }

  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }

  // FIELD()'s fn slot needs a plain void() (see am4.h's own doc comment on
  // FIELD() — unlike OP()'s auto-dispatching bool(int)/EventCall shapes,
  // FIELD() always wires a real EventCall<mask,fn>, fn must match exactly).
  void noOp() {}
}

int test = 55;

int ledCtrl = -1;
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

// altOP(altPrompt,"",showEvent,anyEvent) dropped — custom-prompt subclass,
// see file header comment.
MENU(subMenu, "Sub-Menu", action::titleEvent, Menu::anyEvent, Menu::noStyle
  ,OP("Sub1", action::showEvent, Menu::anyEvent)
  ,OP("Sub2", action::showEvent, Menu::anyEvent)
  ,OP("Sub3", action::showEvent, Menu::anyEvent)
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
// real hardware input (encoder+button over PCINT) is proven separately,
// see file header comment.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);

// manual-only idle (no nav.timeOut in the original — codeCtrl's own
// precedent) — still needs a Run type for NAVROOT_IDLE's idleOn()/idleOff().
bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;

NAVROOT_IDLE(nav, mainMenu, /*maxDepth*/2, in, out, Run);

// 3-state idle callback (AM4's real idleStart/idling/idleEnd) — approximated
// via a tick counter, same shape as fullIdle's own idleRun(); leaves idle
// after a fixed tick count instead of waiting for a real Enter keypress
// (no real hardware input in a native selftest — see file header comment).
int alertStartCount = 0, alertEndCount = 0, alertTicks = 0;
bool alertRun() {
  if(alertTicks==0) {
    alertStartCount++;
    printf("suspending menu!\n");
  }
  printf("alert test\n[select] to continue...\n");
  alertTicks++;
  if(alertTicks>=3) {
    alertEndCount++;
    printf("resuming menu.\n");
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

  // index 0=Op1,1=Op2,2=Test,3=subMenu,4=setLed,5=LEDOn,6=LEDOff,
  // 7=selMenu,8=chooseMenu,9=Alert,10=<Back>

  // ── Op1/Op2: real 3-arg action1/action2 ──────────────────────────────────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = Op1
  assert(action::op1Count == 1 && "action1's real 3-arg shape did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (Op2)
  nav.enter();
  assert(action::op2Count == 1 && "action2's real 3-arg shape did not fire on Enter");

  // ── subMenu: titleEvent (MENU()-level) + showEvent (OP()-level), both
  // real event dispatch ─────────────────────────────────────────────────────
  nav.up(); nav.up();  // Op2(1) -> Test(2) -> subMenu(3)
  int titleAtLanding = action::titleCount;  // Focus already fires titleEvent on landing
  assert(titleAtLanding > 0 && "subMenu's own MENU()-level titleEvent did not observe Focus on landing");
  nav.enter();
  assert(nav.level() == 1 && "subMenu did not open");
  assert(action::titleCount > titleAtLanding &&
         "subMenu's own MENU()-level titleEvent did not additionally fire on Enter");
  nav.enter();  // Sub1 (index 0 inside subMenu)
  assert(action::subCount == 1 && "Sub1's real 3-arg showEvent did not fire on Enter");
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

  // ── Alert test: manual-only idle with a real 3-state (start/idling/end)
  // callback — doAlert's Enter triggers nav.idleOn(alertRun); alertRun
  // terminates deterministically and calls Run::idleOff() itself. ─────────
  nav.up(); nav.up(); nav.up();  // LEDOff(6) -> selMenu(7) -> chooseMenu(8) -> Alert(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: LiquidCrystal.ino compat-macro port (native parallel-HD44780 + "
         "PCINT-driven encoder/button hardware proven separately, see "
         ".RnD/AM4check/LiquidCrystal) verified\n");
  return 0;
}
