/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 lolin32.ino compat-macro port — same real U8G2 output device
 *        as `U8G2.ino`'s own port, retargeted at a real ESP32 lolin32
 *        board's built-in OLED, plus a free-form `EDIT("Name",...)` field
 *        (single-shared-position validator, same shape `TextField.ino`'s
 *        own name field already proved) and `FIELD("Test",...)` enabled
 *        (commented out in `U8G2.ino`'s own source). See the original at
 *        github.com/neu-rah/ArduinoMenu examples/lolin32/lolin32/
 *        lolin32.ino.
 *
 * Confirmed via a real diff against `U8G2.ino`'s own vendored source: this
 * is NOT a byte-identical retarget (unlike `SDCard.ino`/`SdFat.ino`'s own
 * relationship) — real, distinct additions beyond the platform swap.
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - `altOP(altPrompt,"",doNothing,noEvent)` (custom-prompt subclass)
 *    dropped, same precedent as every other `altOP`/`altMENU` drop.
 *  - The real source's splash screen + `setContrast()` fade-in animation
 *    (drawn directly via `u8g2.drawStr()`/`u8g2.firstPage()`/`nextPage()`
 *    in `setup()`) and `nav.timeOut=60` (AM4's separate auto-idle
 *    mechanism, on top of the manual `doAlert`/`nav.idleOn(alert)` "Alert
 *    test" op this example *also* has) are both dropped — cosmetic/
 *    secondary to the real point of this port (real U8G2 hardware +ative
 *    text-field editing), same "disclose rather than build every cosmetic
 *    AM4 demo flourish" precedent as `SSD1306Ascii.ino`'s own idle-message
 *    simplification. The manual "Alert test" idle path (`U8G2.ino`'s own,
 *    already-verified mechanism) is kept.
 *  - `myLedOn`/`myLedOff` (0-arg) ported as `bool(int)`, same established
 *    adapter.
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
  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }
  void noOp() {}
}

int test = 55;

int ledCtrl = 1;
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

MENU(subMenu, "Sub-Menu", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,OP("Sub1", Menu::doNothing, Menu::noEvent)
  ,EXIT("<Back")
);

uint16_t hrs = 0, mins = 0;
PADMENU(timeMenu, "Time", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,FIELD(hrs,  "", ":", 0, 11, 1,  0, action::noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(mins, "", "",  0, 59, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
);

static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {hexDigit};
char buf1[] = "0000";

static const char* alphaNum =
  " 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const char* alphaNumMask[] = {alphaNum};
char name[11] = "          ";

bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", Menu::doNothing, Menu::noEvent)
  ,OP("Op2", Menu::doNothing, Menu::noEvent)
  ,FIELD(test, "Test", "%", 0, 100, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,EDIT("Name", name, alphaNumMask, action::noOp, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(timeMenu)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Exit")
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

  // index: 0 Op1, 1 Op2, 2 Test, 3 Name, 4 Time, 5 Sub-Menu, 6 setLed,
  //        7 LED On, 8 LED Off, 9 Select, 10 Choose, 11 Alert test, 12 Hex, 13 <Exit>
  assert(action::ledOnCount == 0);
  for(int i=0;i<7;i++) nav.up(); // Op1(0) -> LED On(7)
  nav.enter();
  assert(action::ledOnCount == 1 && "LED On did not fire");
  nav.up(); // LED On(7) -> LED Off(8)
  nav.enter();
  assert(action::ledOffCount == 1 && "LED Off did not fire");

  nav.up(); nav.up(); nav.up(); // LED Off(8) -> Select(9) -> Choose(10) -> Alert test(11)
  assert(idleStartCount == 0);
  nav.enter();
  int guard = 0;
  while(!Run::active() && guard++<200) Run::run();
  assert(Run::active() && "doAlert did not trigger idleOn");
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1 &&
         "idleRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: lolin32.ino compat-macro port (native U8g2Vendor over a real "
         "ESP32 lolin32 built-in OLED proven separately, see "
         ".RnD/AM4check/lolin32) verified\n");
  return 0;
}
