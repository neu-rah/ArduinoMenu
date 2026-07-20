/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 Button_Navigation.ino compat-macro port — "MENU NAVIGATION
 *        WITH JUST 4 BUTTONS": same SSD1306 OLED as `SSD1306Ascii.ino`,
 *        but real physical button input (4 dedicated pins, one fixed nav
 *        command each) instead of Serial, plus LED brightness/PWM control.
 *        Created by Tamojit Saha (github.com/TamojitSaha) — see the
 *        original at github.com/neu-rah/ArduinoMenu examples/
 *        SSD1306Ascii_Button_Navigation/Button_Navigation/
 *        Button_Navigation.ino.
 *
 * Content-identical to `SSD1306Ascii.ino`'s own port for everything except
 * the two real differences this example is actually about — see that
 * file's own header comment for the OLED/font rationale, not repeated
 * here. This file's own two real differences:
 *  - **Single OLED output device** (no Serial fanout — the original's own
 *    `outputs[] = {&outOLED}`, one device, unlike the plain example's
 *    dual OLED+Serial). Simpler `idle()` too: no per-device branching
 *    needed (the plain example's own disclosed simplification doesn't
 *    even apply here — there's only one device to begin with).
 *  - **Real dedicated-nav-button input** (`keyIn<4>`, 4 pins — SEL/UP/
 *    DOWN/ESC — each firing its own fixed nav command directly, not the
 *    Click/Hold-derived Enter/Esc-only mapping single-button ports
 *    (`LiquidCrystal`/`clickEncoder`/`ajoy`) use). New, small OneInput/
 *    OneMenu components for this shape (added this session):
 *    `oneInput::PressCapture` (immediate on-press, no click/hold duration
 *    distinction — matches AM4's own instant-fire behavior for a
 *    dedicated button) + `oneMenu::DirectBtnIn<HW,FixedCmd>` (one pin, one
 *    fixed `Cmd`, the "no click/hold" sibling of `BtnIn`).
 *  - LED brightness/PWM (`adjustBrightness`, `ledOn`/`ledOff` via real
 *    `analogWrite`) and a separate built-in-LED on/off pair
 *    (`internalLedOn`/`internalLedOff`) — all 0-arg AM4 handlers, ported
 *    as `bool(int)`, established adapter shape. `adjustBrightness` is
 *    itself `FIELD()`'s own `fn` (needs `void()`, same fix every other
 *    FIELD()-using port already needs).
 *
 * Real handler-shape mapping, same as `SSD1306Ascii.ino`'s own port:
 * `action1(eventMask)` genuinely 1-arg here too, trimmed to `bool(
 * EventMask,IItem&)`; `action2`/`showEvent` real 3-arg, kept as `bool(
 * EventMask,INav&,IItem&)`; `showEvent` doubles as `subMenu`'s own
 * `MENU()`-level title fn, same `titleEvent` substitution.
 *
 * Real hardware (4 real buttons on D3/D8/D4/D1, `INPUT_PULLUP` — matching
 * the original's own `NAV_BUTTONS_INPUT_PULLUP` default — plus a real PWM
 * LED on D11) is proven separately at `.RnD/AM4check/
 * SSD1306Ascii_Button_Navigation` (gitignored), same "native selftest
 * here, real device wiring there" split as every other port this session.
 *
 * The selftest drives nav.up()/enter() directly, same style as every
 * other port in this compat layer.
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
  bool action1(oneMenu::EventMask, oneMenu::IItem&) { op1Count++; return true; }

  int op2Count = 0;
  bool action2(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { op2Count++; return true; }

  int subCount = 0;
  bool showEvent(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { subCount++; return true; }

  int titleCount = 0;
  bool titleEvent(oneMenu::EventMask) { titleCount++; return true; }

  int ledOnCount = 0, ledOffCount = 0;
  bool ledOn(int)  { ledOnCount++;  return true; }
  bool ledOff(int) { ledOffCount++; return true; }

  int internalLedOnCount = 0, internalLedOffCount = 0;
  bool internalLedOn(int)  { internalLedOnCount++;  return true; }
  bool internalLedOff(int) { internalLedOffCount++; return true; }

  int adjustBrightnessCount = 0;
  void adjustBrightness() { adjustBrightnessCount++; }
}

int ledCtrl = 1;  // HIGH, matching the original's own default
int brightnessValue = 15;

TOGGLE(ledCtrl, setLed, "Led: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("On",  1, action::ledOn,  Menu::enterEvent)
  ,VALUE("Off", 0, action::ledOff, Menu::enterEvent)
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
// same precedent as every other port.
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
  ,FIELD(brightnessValue, "Brightness", "%", 0, 100, 5, 5, action::adjustBrightness, Menu::enterEvent, Menu::wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::internalLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::internalLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EXIT("<Back")
);

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// real button hardware (4 dedicated pins via DirectBtnIn) is proven
// separately, see file header comment.
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

  // index 0=Op1,1=Op2,2=Brightness,3=subMenu,4=setLed,5=LEDOn,6=LEDOff,
  // 7=selMenu,8=chooseMenu,9=Alert,10=<Back>

  // ── Op1 (2-arg trim of a real 1-arg AM4 handler) / Op2 (real 3-arg) ──────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = Op1
  assert(action::op1Count == 1 && "action1's 2-arg trim did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (Op2)
  nav.enter();
  assert(action::op2Count == 1 && "action2's real 3-arg shape did not fire on Enter");

  // ── FIELD() Brightness — adjustBrightness fires on edit-mode toggle ──────
  nav.up();     // Op2(1) -> Brightness(2)
  nav.enter();
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  assert(action::adjustBrightnessCount > 0 && "adjustBrightness did not fire entering edit mode");
  nav.esc();

  // ── subMenu: titleEvent (MENU()-level) + showEvent (OP()-level) ──────────
  nav.up();     // Brightness(2) -> subMenu(3)
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

  // ── TOGGLE (real ledOn/ledOff VALUE() handlers) ──────────────────────────
  nav.up();     // subMenu(3) -> setLed(4)
  nav.enter();
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to the next VALUE()");
  // NOTE: VALUE()'s own per-option fn/mask (ledOn/ledOff here — a real
  // difference from the plain SSD1306Ascii.ino port, whose own TOGGLE
  // uses doNothing) are documented as accepted-but-ignored in this compat
  // layer (am4.h's own VALUE() doc comment) — kept here for fidelity to
  // what the real AM4 source actually wires, not because it fires.
  (void)action::ledOnCount; (void)action::ledOffCount;

  // ── LED On/Off (internal LED, 0-arg AM4 handlers ported as bool(int)) ────
  assert(action::internalLedOnCount == 0);
  nav.up();     // setLed(4) -> LEDOn(5)
  nav.enter();
  assert(action::internalLedOnCount == 1);
  nav.up();     // LEDOn(5) -> LEDOff(6)
  nav.enter();
  assert(action::internalLedOffCount == 1);

  // ── Alert test: manual-only idle with a generic idle message ─────────────
  nav.up(); nav.up(); nav.up();  // LEDOff(6) -> selMenu(7) -> chooseMenu(8) -> Alert(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: Button_Navigation.ino compat-macro port (native Ssd1306+"
         "OledDisplay OLED + real 4-button DirectBtnIn hardware proven "
         "separately, see .RnD/AM4check/SSD1306Ascii_Button_Navigation) "
         "verified\n");
  return 0;
}
