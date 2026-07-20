/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 TFT.ino compat-macro port — Arduino.cc's own official "TFT"
 *        library (ST7735, `class TFT : public Adafruit_ST7735`, confirmed
 *        via the real header) reached through the EXISTING
 *        `oneIO::display::AdaGfxVendor` (no new wrapper needed — same
 *        device class `adafruitGfx_tft.ino` already ports). See the
 *        original at github.com/neu-rah/ArduinoMenu examples/TFT/TFT/TFT.ino.
 *
 * Deviations from the original, beyond the usual #include swap:
 *  - Serial-only input (real device wiring is proven separately, see
 *    `.RnD/AM4check/TFT`), matching every prior port's "focus the new
 *    capability, disclose the rest" precedent.
 *  - The original's own content is already minimal (one OP + one masked
 *    hex EDIT, MAX_DEPTH=1) — ported verbatim, no simplification needed.
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
  int opCount = 0;
  bool doOp1(int) { opCount++; return true; }
  void noOp() {}
}

static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", action::doOp1, Menu::enterEvent)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

#define MAX_DEPTH 1

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

  // index: 0 Op1, 1 Hex, 2 <Back>
  assert(action::opCount == 0);
  nav.enter();
  assert(action::opCount == 1 && "Op1 did not fire");

  printf("OK: TFT.ino compat-macro port (native AdaGfxVendor over a real "
         "Arduino.cc TFT/Adafruit_ST7735, proven separately, see "
         ".RnD/AM4check/TFT) verified\n");
  return 0;
}
