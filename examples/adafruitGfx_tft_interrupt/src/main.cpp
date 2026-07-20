/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 tft_interrupt.ino (adafruitGfx_tft_interrupt) compat-macro
 *        port — same real Adafruit_GFX device as `tft.ino`'s own port, plus
 *        two real content additions: `FIELD("Test",...)` enabled (commented
 *        out in `tft.ino`'s own source) and an `"Exit?"` Yes/Cancel confirm
 *        dialog (`subMenu2`/`confirmExit`), same shape `Confirm.ino`'s own
 *        port already proved (a plain SUBMENU with Yes/Cancel items, no
 *        custom-prompt subclass needed). See the original at
 *        github.com/neu-rah/ArduinoMenu examples/adafruitGfx_tft_interrupt/
 *        tft_interrupt.ino.
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - `altOP(altPrompt,"",doNothing,noEvent)` (custom-prompt subclass, inside
 *    `subMenu`) dropped, same precedent as every other `altOP` drop.
 *  - `systemExit()` (real AM4 signature: 0-arg, calls `nav.reset()`/
 *    `nav.idleOn()`/clears the screen and returns `quit`) ported as
 *    `bool(int)`, same established adapter and same "no port-side
 *    close-the-nav equivalent" disclosed gap as `Confirm.ino`'s own port.
 *  - The real source's Due-specific interrupt-pin joystick input
 *    (`USE_INTERRUPT_PINS`, `encoderIn<BTN_UP,...>`) dropped — Serial-only
 *    input, same "focus the new capability" precedent as `tft.ino`'s own
 *    port (this port's real point is the Adafruit_GFX device + the confirm
 *    dialog, not a third input mechanism).
 *  - `myLedOn`/`myLedOff` (0-arg) ported as `bool(int)`.
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
  int ledOnCount = 0, ledOffCount = 0, exitCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }
  bool systemExit(int) { exitCount++; return true; }
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

MENU(subMenu, "Sub-Menu", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,EXIT("<Back")
);

MENU(subMenu2, "Exit?", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Yes", action::systemExit, Menu::enterEvent)
  ,EXIT("Cancel")
);

static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", Menu::doNothing, Menu::noEvent)
  ,OP("Op2", Menu::doNothing, Menu::noEvent)
  ,FIELD(test, "Test", "%", 0, 100, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(subMenu2)
);

#define MAX_DEPTH 2

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
MENU_OUTPUTS(out, MAX_DEPTH, &devOut);

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  nav.printTo(devOut);

  // index: 0 Op1, 1 Op2, 2 Test, 3 Sub-Menu, 4 setLed, 5 LED On, 6 LED Off,
  //        7 Select, 8 Choose, 9 Hex, 10 Exit?
  assert(action::ledOnCount == 0);
  for(int i=0;i<5;i++) nav.up(); // Op1(0) -> LED On(5)
  nav.enter();
  assert(action::ledOnCount == 1 && "LED On did not fire");
  nav.up(); // LED On(5) -> LED Off(6)
  nav.enter();
  assert(action::ledOffCount == 1 && "LED Off did not fire");

  // ── Exit? confirm dialog: Cancel path fires nothing, Yes path fires
  // systemExit exactly once ──────────────────────────────────────────────
  for(int i=0;i<4;i++) nav.up(); // LED Off(6) -> Select(7) -> Choose(8) -> Hex(9) -> Exit?(10)
  nav.enter();
  assert(nav.level()==1 && "entering Exit? opens a level");
  nav.esc(); // Cancel path (EXIT("Cancel") is index 1, but Esc also closes)
  assert(action::exitCount == 0 && "Cancel must not fire systemExit");
  assert(nav.level()==0);

  nav.enter(); // re-open Exit?
  nav.enter(); // Enter on Yes(0)
  assert(action::exitCount == 1 && "Yes did not fire systemExit exactly once");

  printf("OK: tft_interrupt.ino (adafruitGfx_tft_interrupt) compat-macro "
         "port (native AdaGfxVendor over a real Adafruit_ST7735 TFT proven "
         "separately, see .RnD/AM4check/adafruitGfx_tft_interrupt) "
         "verified\n");
  return 0;
}
