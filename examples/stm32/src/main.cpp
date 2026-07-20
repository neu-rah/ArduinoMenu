/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 stm32.ino compat-macro port — a real STM32F103 (blue pill)
 *        platform retarget, content-identical to `serialio.ino`'s own
 *        "everything" showcase minus the `Name` masked EDIT field
 *        (confirmed via a real diff against both vendored sources; every
 *        other field/handler matches). Real output/input is Serial3
 *        (USART3), not Serial. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/stm32/stm32/stm32.ino.
 *
 * The real, new piece is the target platform, not new compat-layer
 * machinery: OneChip already has real STM32F103 support (`chips/stm32/
 * stm32f103.h`, `stm32Uart.h`) — see `.RnD/AM4check/stm32` for the real
 * hardware wiring (a real `ArduinoSerialOut<HardwareSerial,Serial3>`, the
 * STM32duino Arduino core's own `Serial3` global mapped to USART3, same
 * template shape `SERIAL_OUT(Serial)` already uses on AVR). This native
 * selftest uses plain `ANSI_OUT` like every other port.
 *
 * Deviations from the original, beyond the usual #include swap: same as
 * `serialio.ino`'s own port (`altOP` dropped, `showEvent`/`action1`/
 * `action2`/`myLedOn`/`myLedOff` adapted the same way, idle system on
 * `RunLoop`/`am4compat::IdleTimeout`/`NAVROOT_IDLE`) — see that file's own
 * header comment for the full reasoning, not repeated here. `action2`'s
 * real signature here is 2-arg (`result(eventMask,prompt&)`, no `navNode&`)
 * — ported as `bool(EventMask,IItem&)` directly, no trim needed. No `Name`
 * EDIT field in this source (only `Hex`).
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

float test = 55;

int ledCtrl = -1;
TOGGLE(ledCtrl, setLed, "Led: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("On", 1, Menu::doNothing, Menu::noEvent)
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

uint16_t year = 2017, month = 10, day = 7;
PADMENU(birthDate, "Birth", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,FIELD(year,  "", "/", 1900, 3000, 20, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(month, "", "/", 1,    12,   1,  0, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,FIELD(day,   "", "",  1,    31,   1,  0, action::noOp, Menu::noEvent, Menu::wrapStyle)
);

static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

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
  ,EDIT("Hex",  buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(birthDate)
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
using Idle = am4compat::IdleTimeout<50, Run>;

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

int alertStartCount = 0, alertEndCount = 0, alertTicks = 0;
bool alertRun() {
  alertTicks++;
  if(alertTicks==1) alertStartCount++;
  if(alertTicks>=3) {
    alertEndCount++;
    alertTicks = 0;
    Run::idleOff();
  }
  return true;
}

bool mainRun() {
  bool activity = nav.poll();
  Idle::tick(activity, idleRun);
  return true;
}

bool doAlert(oneMenu::EventMask e, oneMenu::IItem&) {
  if(e & oneMenu::EventMask::Enter) nav.idleOn(alertRun);
  return true;
}

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  nav.printTo(devOut);

  // index 0=Op1,1=Op2,2=Test,3=subMenu,4=setLed,5=LEDOn,6=LEDOff,7=selMenu,
  // 8=chooseMenu,9=Alert,10=Hex,11=birthDate,12=<Back>
  assert(action::op1Count == 0);
  nav.enter();
  assert(action::op1Count == 1);

  nav.up();
  nav.enter();
  assert(action::op2Count == 1);

  nav.up(); nav.up();  // -> subMenu(3)
  assert(action::focusCount == 1);
  nav.enter();
  assert(nav.level() == 1 && action::enterCount == 1);
  nav.enter();
  assert(action::enterCount == 2);
  nav.esc();
  assert(nav.level() == 0 && action::exitCount == 1);

  nav.up();     // -> setLed(4)
  nav.enter();
  assert(ledCtrl == 0);

  nav.up();     // -> LED On(5)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up();     // -> LED Off(6)
  nav.enter();
  assert(action::ledOffCount == 1);

  nav.up();     // -> selMenu(7)
  nav.enter();
  nav.up();
  nav.enter();
  assert(selTest == 1);

  nav.up();     // -> chooseMenu(8)
  nav.enter();
  nav.up();
  nav.enter();
  assert(chooseTest == 2);

  nav.up();     // -> Alert test(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active());
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1);

  nav.up();     // -> Hex(10)
  nav.enter();
  nav.down();
  assert(buf1[0] == '0');
  nav.esc();

  nav.up();     // -> birthDate(11)
  nav.enter();
  assert(nav.level() == 1);
  nav.enter();
  nav.up();
  assert(year == 2018);
  nav.enter();
  nav.esc();
  assert(nav.level() == 0);

  assert(idleStartCount == 0);
  int guard = 0;
  while(!Run::active() && guard++<200) { Run::run(); hw::delay_ms(1); }
  assert(Run::active() && "IdleTimeout did not auto-trigger idleOn");
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1);

  printf("OK: stm32.ino compat-macro port (STM32F103 Serial3 real hardware "
         "proven separately, see .RnD/AM4check/stm32) verified\n");
  return 0;
}
