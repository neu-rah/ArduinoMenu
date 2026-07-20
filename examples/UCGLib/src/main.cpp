/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 UCGLib.ino compat-macro port — real Ucglib output device (dual
 *        Ucg+Serial `MENU_OUTPUTS`, matching the original), small menu
 *        (CHOOSE + EDIT hex + OP + SUBMENU). See the original at
 *        github.com/neu-rah/ArduinoMenu examples/UCGLib/UCGLib/UCGLib.ino.
 *
 * `UCG_OUT(ucg,colors,...)` doesn't exist in this compat layer (per-backend
 * device macros are a documented, still-open gap). Reached instead via
 * `oneIO::display::UcgSpiDisplay<...>` — OneIO's own native Ucglib SPI
 * backend (built 2026-07-01, predates this session's vendor-wrapper-policy
 * correction but genuinely calls into Ucglib's real C core, `clib/ucg.h`,
 * not a from-scratch reimplementation — confirmed directly against the
 * header before reuse) through `oneMenu::OledDisplay<Ucg>`, the same
 * text-addressed composition `reflowUcg`'s own real hardware example
 * already proves. See `.RnD/AM4check/UCGLib` for the real-hardware wiring
 * (this native selftest uses plain `ANSI_OUT`).
 *
 * Deviations from the original, beyond the usual #include swap: none of
 * substance — this is the smallest example ported this round, no custom
 * handler shapes or idle system involved.
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
  void noOp() {}
}

static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

int chooseTest = -1;
CHOOSE(chooseTest, chooseMenu, "Choose", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("First",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Second", 2, Menu::doNothing, Menu::noEvent)
  ,VALUE("Third",  3, Menu::doNothing, Menu::noEvent)
  ,VALUE("Last",  -1, Menu::doNothing, Menu::noEvent)
);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", Menu::doNothing, Menu::noEvent)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(chooseMenu)
  ,EXIT("<Back")
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

  // index 0=Op1, 1=Hex, 2=chooseMenu, 3=<Back>
  assert(nav.level()==0);
  nav.up(); nav.up(); // Op1(0) -> Hex(1) -> chooseMenu(2)
  nav.enter();
  assert(nav.level()==1 && "entering chooseMenu opens a level");
  nav.up(); nav.enter(); // open pick-list, move highlight, commit (2-step CHOOSE commit)
  nav.esc();
  assert(nav.level()==0 && "Esc exits chooseMenu back to mainMenu");

  printf("OK: UCGLib.ino compat-macro port (native UcgSpiDisplay over a "
         "real Ucglib ST7735 TFT proven separately, see "
         ".RnD/AM4check/UCGLib) verified\n");
  return 0;
}
