/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 OzOledAscii.ino compat-macro port — content-identical to
 *        `SSD1306Ascii.ino`'s own port (confirmed via a real diff against
 *        both vendored sources — same Op1/Op2/subMenu(showEvent+altOP
 *        dropped)/FIELD/LED on-off/idle tree, no TOGGLE/SELECT/CHOOSE
 *        here though — see below). See the original at
 *        github.com/neu-rah/ArduinoMenu examples/OzOledAscii/
 *        OzOledAscii.ino.
 *
 * The real OzOLED vendor library (kdkanishka/ozoled-oled-display-096-i2c-
 * driver-library) targets a 128x64 I2C OLED — the same SSD1306 chip
 * `SSD1306Ascii.ino`'s own port already reaches natively (`oneIO::
 * display::Ssd1306<Transport,W,H>` + `oneMenu::OledDisplay<Oled>`) — reused
 * directly here too, same "OneIO already covers this exact chip, no new
 * vendor wrapper needed" precedent. See `.RnD/AM4check/OzOledAscii` for
 * the real-hardware wiring (identical to `SSD1306Ascii`'s own).
 *
 * Real content difference from `SSD1306Ascii.ino`'s own port, confirmed
 * via the real diff: this source has NO `TOGGLE`/`SELECT`/`CHOOSE` fields
 * at all — just `Op1`/`Op2`/`subMenu`/`FIELD("Test",...)`/`LED On`/`LED
 * Off`/`Alert test`. `action1` (real AM4 signature: 1-arg) ported as
 * `bool(EventMask,IItem&)`, `action2`/`showEvent` (real 3-arg) kept
 * directly, same precedents as `SSD1306Ascii.ino`'s own port. `altOP`
 * dropped. `ledOn`/`ledOff` (0-arg) ported as `bool(int)`.
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

  void noOp() {}
}

int test = 55;

// altOP(altPrompt,"",showEvent,anyEvent) dropped — custom-prompt subclass.
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
  ,OP("LED On",  action::ledOn,  Menu::enterEvent)
  ,OP("LED Off", action::ledOff, Menu::enterEvent)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EXIT("<Back")
);

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
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);

bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;

NAVROOT_IDLE(nav, mainMenu, /*maxDepth*/2, in, out, Run);

int alertStartCount = 0, alertEndCount = 0, alertTicks = 0;
bool alertRun() {
  if(alertTicks==0) alertStartCount++;
  alertTicks++;
  if(alertTicks>=3) {
    alertEndCount++;
    alertTicks = 0;
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
  nav.printTo(devOut);

  // index 0=Op1,1=Op2,2=Test,3=subMenu,4=LEDOn,5=LEDOff,6=Alert,7=<Back>
  assert(action::op1Count == 0);
  nav.enter();
  assert(action::op1Count == 1);

  assert(action::op2Count == 0);
  nav.up();
  nav.enter();
  assert(action::op2Count == 1);

  nav.up(); nav.up(); // Op2(1) -> Test(2) -> subMenu(3)
  int titleAtLanding = action::titleCount;
  assert(titleAtLanding > 0);
  nav.enter();
  assert(nav.level() == 1);
  nav.enter(); // Sub1
  assert(action::subCount == 1);
  nav.esc();
  assert(nav.level() == 0);

  nav.up(); // subMenu(3) -> LED On(4)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up(); // LED On(4) -> LED Off(5)
  nav.enter();
  assert(action::ledOffCount == 1);

  nav.up(); // LED Off(5) -> Alert test(6)
  assert(!Run::active());
  nav.enter();
  assert(Run::active());
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1);

  printf("OK: OzOledAscii.ino compat-macro port (native Ssd1306+OledDisplay "
         "OLED hardware proven separately, see .RnD/AM4check/OzOledAscii) "
         "verified\n");
  return 0;
}
