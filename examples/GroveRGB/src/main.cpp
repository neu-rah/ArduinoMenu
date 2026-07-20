/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 GroveRGB.ino compat-macro port — content close to
 *        `LCD_Malpartida.ino` (I2C.ino)'s own port (Op1/Op2 3-arg trimmed
 *        to 2-arg, TOGGLE/SELECT/CHOOSE/subMenu/FIELD/idle — no `EDIT`
 *        hex field, commented out in this real source). See the original
 *        at github.com/neu-rah/ArduinoMenu examples/GroveRGB/GroveRGB/
 *        GroveRGB.ino.
 *
 * Real design call: unlike `LCD_Malpartida.ino`/`LCD_PCF8574.ino` (plain
 * HD44780 over a PCF8574 I2C GPIO expander, already fully covered by
 * OneIO's native `I2cLcd`), the Grove RGB Backlight LCD is a genuinely
 * different chip — its own native I2C protocol (two addresses: one for
 * the HD44780-compatible text controller, one for the RGB backlight PWM
 * controller), not reachable through `I2cLcd` at all. Per the
 * vendor-driver-wrapper policy, this calls Seeed's own real `rgb_lcd`
 * object directly via a new thin `oneIO::display::GroveRgbLcdVendor`
 * (built this round), satisfying `oneMenu::LcdOut<Lcd>`'s own real
 * contract. See `.RnD/AM4check/GroveRGB` for the real-hardware wiring
 * (this native selftest uses plain `ANSI_OUT`).
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - `altOP(altPrompt,"",showEvent,anyEvent)` dropped, same precedent as
 *    every other `altOP` drop.
 *  - `action1`/`action2` (real 3-arg AM4 shape) ported as `bool(EventMask,
 *    IItem&)` (2-arg trim), same precedent as `serialio.ino`'s own port.
 *  - `myLedOn`/`myLedOff` (0-arg) ported as `bool(int)`.
 *  - The real source's `ClickEncoder` input dropped — Serial-only,
 *    matching every prior port's own "focus the new capability, disclose
 *    the rest" precedent (this port's own point is the Grove RGB LCD
 *    device, not input hardware already proven elsewhere).
 *  - The idle system (`nav.idleTask=idle`/`nav.idleOn(alert)`) is adapted
 *    onto `RunLoop`/`NAVROOT_IDLE`, same pattern as every other idle-using
 *    port.
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
  bool action2(oneMenu::EventMask, oneMenu::IItem&) { op2Count++; return true; }

  int enterCount = 0, exitCount = 0, focusCount = 0, blurCount = 0;
  void countEvent(oneMenu::EventMask e) {
    if(e & oneMenu::EventMask::Enter) enterCount++;
    if(e & oneMenu::EventMask::Exit)  exitCount++;
    if(e & oneMenu::EventMask::Focus) focusCount++;
    if(e & oneMenu::EventMask::Blur)  blurCount++;
  }
  bool showEvent(oneMenu::EventMask e, oneMenu::INav&, oneMenu::IItem&) { countEvent(e); return true; }
  bool titleEvent(oneMenu::EventMask e) { countEvent(e); return true; }

  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }

  void noOp() {}
}

int test = 55;

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

  // index: 0 Op1, 1 Op2, 2 Test, 3 Sub-Menu, 4 setLed, 5 LED On, 6 LED Off,
  //        7 Select, 8 Choose, 9 Alert test, 10 <Back>
  assert(action::op1Count == 0);
  nav.enter();
  assert(action::op1Count == 1);
  nav.up();
  nav.enter();
  assert(action::op2Count == 1);

  nav.up(); nav.up(); // Op2(1) -> Test(2) -> Sub-Menu(3)
  assert(action::focusCount == 1);
  nav.enter();
  assert(nav.level() == 1 && action::enterCount == 1);
  nav.enter();
  assert(action::enterCount == 2);
  nav.esc();
  assert(nav.level() == 0 && action::exitCount == 1);

  nav.up(); // Sub-Menu(3) -> setLed(4)
  nav.enter();
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to and sync the next VALUE()");

  nav.up(); // setLed(4) -> LED On(5)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up(); // LED On(5) -> LED Off(6)
  nav.enter();
  assert(action::ledOffCount == 1);

  nav.up(); nav.up(); // LED Off(6) -> Select(7) -> Choose(8)
  nav.up(); // Choose(8) -> Alert test(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active());
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1);

  printf("OK: GroveRGB.ino compat-macro port (native GroveRgbLcdVendor "
         "over a real Seeed Grove RGB LCD proven separately, see "
         ".RnD/AM4check/GroveRGB) verified\n");
  return 0;
}
