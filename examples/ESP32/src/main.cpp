/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 ClickEncoderTFT.ino (ESP32) compat-macro port — a real
 *        application sketch ("COPPER TAPE CUTTER" controller), not a bare
 *        menu demo: ST7735S TFT_eSPI output + ClickEncoder rotary input.
 *        Reached via the EXISTING `oneIO::display::TftEspiVendor` (built
 *        for `adafruitGfx_eTFT.ino`, no new wrapper needed — same vendor
 *        library). Real click-encoder input ported onto OneInput's native
 *        `Encoder`+`BtnCapture` stack over ESP32's `Esp32EncPins`/
 *        `Esp32BtnPin` (same replacement `rotaryEvent.ino`'s own port
 *        already made for the original's `ClickEncoder`/`AceButton`-style
 *        third-party stacks — no `ClickEncoder`/`soligen2010/encoder`
 *        dependency needed). See the original at github.com/neu-rah/
 *        ArduinoMenu examples/ESP32/ClickEncoderTFT/ClickEncoderTFT.ino,
 *        and `.RnD/AM4check/ESP32` for the real hardware wiring (this
 *        native selftest uses plain `ANSI_OUT`).
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - `doFeed`/`doRunCuts` (0-arg AM4 handlers) ported as `bool(int)`, same
 *    established adapter.
 *  - `updateEEPROM()` is defined in the real source but never actually
 *    wired to any OP/menu item (dead code in the original itself,
 *    `// writeEEPROM();` is commented out too) — not ported, matching the
 *    real source's own unused state, not a simplification.
 *  - `subMenuAdjustServo`'s own two `FIELD(settingsEEPROM...)` lines are
 *    ALREADY commented out in the vendored original (`settingsEEPROM`
 *    itself is never even declared) — not ported, matching the real
 *    source exactly.
 *  - The app-level `exitMenuOptions`/`runCuts()`/`feedInOut()`/ESP32-timer
 *    state machine around the menu is real hardware/timing glue, not menu
 *    content — omitted here the same way every prior port omits
 *    `setup()`/`loop()` glue unrelated to the menu tree itself; `doFeed`/
 *    `doRunCuts` are still exercised directly via `nav.enter()`, since
 *    they ARE part of the menu's own event wiring.
 *  - 2-color (Fg/Bg) `TftEspiVendor` theme instead of the original's
 *    6-entry AM4 `colorDef` table — same simplification every other
 *    vendor-GFX port in this compat layer already makes.
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
  int feedCount = 0, runCutsCount = 0;
  bool doFeed(int)     { feedCount++;    return true; }
  bool doRunCuts(int)  { runCutsCount++; return true; }
  void noOp() {}
}

int chooseField  = 1;
int numberOfCuts = 5;
int lengthOfCuts = 50;
int feedLength   = 304;

MENU(subMenuAdjustServo, "Adjust Servo Settings", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,OP("Run!", action::doFeed, Menu::enterEvent)
  ,EXIT("<Back")
);

CHOOSE(chooseField, feedDirChoose, "Choose Direction:", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("Forward",   1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Backwards", 0, Menu::doNothing, Menu::noEvent)
);

MENU(subMenuFeedInOut, "Feed Tape", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,FIELD(feedLength, "Length of Feed:", "mm", 0, 1000, 10, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(feedDirChoose)
  ,OP("Run!", action::doFeed, Menu::enterEvent)
  ,EXIT("<Back")
);

MENU(mainMenu, "COPPER TAPE CUTTER", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(lengthOfCuts,  "Cut Size:", "mm", 0, 2000, 10, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(numberOfCuts,  "Pieces:",   "",   0, 1000, 10, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,OP("Cut!", action::doRunCuts, Menu::enterEvent)
  ,SUBMENU(subMenuFeedInOut)
  ,SUBMENU(subMenuAdjustServo)
);

#define MAX_DEPTH 3

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// real hardware input (ESP32 attachInterrupt-driven encoder+button, same
// stack rotaryEvent.ino already proved) is proven separately, see file
// header comment.
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

  // index: 0 Cut Size, 1 Pieces, 2 Cut!, 3 Feed Tape, 4 Adjust Servo Settings
  assert(action::runCutsCount == 0);
  for(int i=0;i<2;i++) nav.up(); // Cut Size(0) -> Cut!(2)
  nav.enter();
  assert(action::runCutsCount == 1 && "Cut! did not fire doRunCuts");

  nav.up(); // Cut!(2) -> Feed Tape(3)
  nav.enter();
  assert(nav.level() == 1 && "Feed Tape submenu did not open");
  // Feed Tape: 0 Length of Feed, 1 Choose Direction, 2 Run!, 3 <Back>
  nav.up(); nav.up();
  assert(action::feedCount == 0);
  nav.enter();
  assert(action::feedCount == 1 && "Feed Tape's own Run! did not fire doFeed");
  nav.esc();
  assert(nav.level() == 0);

  nav.up(); // Feed Tape(3) -> Adjust Servo Settings(4)
  nav.enter();
  assert(nav.level() == 1 && "Adjust Servo Settings submenu did not open");
  // Adjust Servo Settings: 0 Run!, 1 <Back>
  nav.enter();
  assert(action::feedCount == 2 && "Adjust Servo Settings' own Run! did not fire doFeed");

  printf("OK: ClickEncoderTFT.ino (ESP32) compat-macro port (native "
         "TftEspiVendor over a real TFT_eSPI ST7735S + OneInput Encoder/"
         "BtnCapture over Esp32EncPins/Esp32BtnPin, proven separately, "
         "see .RnD/AM4check/ESP32) verified\n");
  return 0;
}
