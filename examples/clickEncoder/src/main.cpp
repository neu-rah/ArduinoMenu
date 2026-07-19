/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 clickEncoder.ino compat-macro port — "Arduino menu using
 *        clickEncoder and I2C LCD". See the original at github.com/neu-rah/
 *        ArduinoMenu examples/clickEncoder/clickEncoder/clickEncoder.ino.
 *
 * Content-identical to `LiquidCrystal.ino`'s own port (same TOGGLE/SELECT/
 * CHOOSE/FIELD/SUBMENU/OP/3-state-idle shape, same real 3-arg action1/
 * action2/showEvent handlers) — see that file's own header comment for the
 * full rationale on every one of those pieces, not repeated here. This
 * file's only two real differences are the encoder's own driving mechanism
 * and one extra field:
 *  - The original drives its encoder via a *different* third-party stack
 *    entirely from `LiquidCrystal.ino`'s own blocker: `TimerOne` (a
 *    hardware Timer1 periodic-interrupt library) polling a `ClickEncoder`
 *    object once per tick, not edge-triggered PCINT. OneInput's native
 *    `Encoder`+`AvrEncPins` (edge-triggered PCINT, the same stack
 *    `LiquidCrystal.ino`'s own port uses) replaces it just as cleanly — no
 *    `TimerOne`/`ClickEncoder` dependency needed at all. Real wiring proven
 *    at `.RnD/AM4check/clickEncoder` (gitignored), same native-selftest/
 *    real-hardware split as `LiquidCrystal.ino`.
 *  - The original also has a real `EDIT("Hex",buf1,hexNr,...)` field
 *    (commented out in `LiquidCrystal.ino`'s own source) — a 4-*distinct*-
 *    position mask (`{"0","x",hexDigit,hexDigit}` — fixed "0x" prefix, then
 *    2 free hex digits), same shape `TextField.ino`'s own IP-address field
 *    already proved (distinct per-position validators, not identical ones
 *    like `TextField.ino`'s own separate Hex field).
 *  - Real LCD is I2C (`LiquidCrystal_I2C`, 20x4) rather than parallel
 *    (16x2) — ported onto OneIO's native `I2cLcd` (PCF8574 backpack, same
 *    chip family, no third-party driver), not the parallel `Hd44780`
 *    `LiquidCrystal.ino`'s own port uses.
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
#include <cstring>

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

  int titleCount = 0;
  bool titleEvent(oneMenu::EventMask) { titleCount++; return true; }

  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }

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
// same precedent as LiquidCrystal.ino's own port.
MENU(subMenu, "Sub-Menu", action::titleEvent, Menu::anyEvent, Menu::noStyle
  ,OP("Sub1", action::showEvent, Menu::anyEvent)
  ,OP("Sub2", action::showEvent, Menu::anyEvent)
  ,OP("Sub3", action::showEvent, Menu::anyEvent)
  ,EXIT("<Back")
);

// ── Hex field: 4 *distinct* positions ("0","x",hexDigit,hexDigit — a fixed
// "0x" prefix then 2 free hex digits), same shape as TextField.ino's own IP
// field (distinct per-position validators) — see file header comment. ────
static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("OpA", action::action1, Menu::anyEvent)
  ,OP("OpB", action::action2, Menu::enterEvent)
  ,FIELD(test, "Test", "%", 0, 100, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// real hardware input (PCINT-driven encoder+button, replacing the original's
// TimerOne+ClickEncoder) is proven separately, see file header comment.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);

bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;

NAVROOT_IDLE(nav, mainMenu, /*maxDepth*/2, in, out, Run);

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

  // index 0=OpA,1=OpB,2=Test,3=subMenu,4=setLed,5=LEDOn,6=LEDOff,
  // 7=selMenu,8=chooseMenu,9=Alert,10=Hex,11=<Back>

  // ── OpA/OpB: real 3-arg action1/action2 ──────────────────────────────────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = OpA
  assert(action::op1Count == 1 && "action1's real 3-arg shape did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (OpB)
  nav.enter();
  assert(action::op2Count == 1 && "action2's real 3-arg shape did not fire on Enter");

  // ── subMenu: titleEvent (MENU()-level) + showEvent (OP()-level) ──────────
  nav.up(); nav.up();  // OpB(1) -> Test(2) -> subMenu(3)
  int titleAtLanding = action::titleCount;
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
  // callback ───────────────────────────────────────────────────────────────
  nav.up(); nav.up(); nav.up();  // LEDOff(6) -> selMenu(7) -> chooseMenu(8) -> Alert(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  // ── EDIT() Hex — 4-distinct-position mask, same shape as TextField.ino's
  // own IP field (fixed "0x" prefix, 2 free hex digits) ───────────────────
  nav.up();     // Alert(9) -> Hex(10)
  assert(strcmp(buf1, "0x11")==0);
  nav.enter();  // opens edit mode
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();     // cycle position 0 — fixed to "0", must stay unchanged
  assert(buf1[0] == '0' && "EDIT()'s fixed position-0 validator ({\"0\"}) must not change");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  printf("OK: clickEncoder.ino compat-macro port (native I2C-LCD + "
         "PCINT-driven encoder/button hardware proven separately, see "
         ".RnD/AM4check/clickEncoder) verified\n");
  return 0;
}
