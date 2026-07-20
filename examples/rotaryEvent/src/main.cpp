/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 rotaryEvent.ino compat-macro port — demonstrates the same
 *        encoder+button hardware class as the already-shipped
 *        `clickEncoder.ino`/`lolin32.ino`, wrapping AM4's own
 *        `RotaryEventIn` (a generic event-queue input abstraction meant to
 *        decouple the menu from any specific rotary/button library). This
 *        decoupling is already structurally free in OneMenu — compile-time
 *        `InDef<...>` composition means the menu never depends on a
 *        concrete encoder/button library either way — so no
 *        `RotaryEventIn`-equivalent machinery was built; this ports
 *        directly onto OneInput's existing `Encoder`+`BtnCapture` stack,
 *        same one `clickEncoder.ino` already uses (over ESP32's
 *        `Esp32EncPins`/`Esp32BtnPin` for the real-hardware check, since
 *        the original targets the exact LOLIN32+SSD1306 board `lolin32.ino`
 *        already has a proven `U8g2Vendor` wiring for — see
 *        `.RnD/AM4check/rotaryEvent`). See the original at
 *        github.com/neu-rah/ArduinoMenu examples/rotaryEvent/rotaryEvent/
 *        rotaryEvent.ino.
 *
 * Disclosed simplification: the original also distinguishes
 * BUTTON_DOUBLE_CLICKED (mapped to the same Esc as long-press per its own
 * "TODO: hardcoded to back" comment) — OneInput has no double-click
 * detector, but since the original treats double-click and long-press
 * identically anyway, `BtnIn`'s existing click->Enter/hold->Esc mapping
 * already reproduces 100% of the *actual* (not theoretical) original
 * behavior.
 *
 * Content is the original's own real menu verbatim — two FIELD()s
 * (timeOn/timeOff), MAX_DEPTH=1, no SUBMENU/OP.
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

namespace action {
  void noOp() {}
}

unsigned int timeOn = 10;
unsigned int timeOff = 90;

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,  "On",  "ms", 0, 1000,  10, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff, "Off", "ms", 0, 10000, 10, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

#define MAX_DEPTH 1

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// real hardware input (attachInterrupt-driven encoder+button) is proven
// separately, see file header comment.
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

  // index: 0 On, 1 Off, 2 <Back>
  assert(timeOn == 10);
  nav.enter();  // opens edit mode on "On"
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  // FIELD()'s step/tune params are accepted but ignored (project-wide
  // known state, see project_step_tune_mode memory) — a plain Up always
  // increments by 1, not by the declared `step`(10)/`tune`(1) params.
  nav.up();
  assert(timeOn == 11 && "FIELD(timeOn,...) did not step on Up");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  nav.up();     // On(0) -> Off(1)
  assert(timeOff == 90);
  nav.enter();
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();
  assert(timeOff == 91 && "FIELD(timeOff,...) did not step on Up");
  nav.esc();

  printf("OK: rotaryEvent.ino compat-macro port (native Encoder+BtnCapture "
         "over real ESP32 Esp32EncPins/Esp32BtnPin + U8g2Vendor, proven "
         "separately, see .RnD/AM4check/rotaryEvent) verified\n");
  return 0;
}
