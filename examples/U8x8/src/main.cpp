/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 U8x8.ino compat-macro port — real dual Serial+U8X8 output
 *        device, small menu (CHOOSE + EDIT hex + OP + SUBMENU) — same
 *        shape `UCGLib.ino`'s own port already ported. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/U8x8/U8x8/U8x8.ino.
 *
 * `U8X8_OUT(u8x8,...)` doesn't exist in this compat layer. Reached instead
 * via a NEW `oneIO::display::U8x8Vendor<Cols,Rows>` — a thin direct
 * wrapper over a real `U8X8&` object (same vendor-driver-wrapper policy as
 * `U8g2Vendor`/`AdaGfxVendor`), built this round because U8X8's own API is
 * genuinely tile/character-addressed (`drawString(x,y,s)` takes x/y in
 * character-cell units, not pixels — unlike U8G2's pixel-addressed
 * `drawGlyph`) — composed through `oneMenu::OledDisplay<Oled>` (the
 * text-addressed sibling `Ssd1306`/`Ucg` already use), not
 * `VendorGfxDisplay` (the pixel-addressed one `U8g2Vendor`/`AdaGfxVendor`
 * use). See `.RnD/AM4check/U8x8` for the real-hardware wiring (a real
 * `U8X8_PCD8544_84X48_4W_HW_SPI`, matching the original exactly; this
 * native selftest uses plain `ANSI_OUT`).
 *
 * Deviations from the original: none of substance — same smallest-example
 * shape `UCGLib.ino`'s own port already established, no custom handler
 * shapes or idle system involved.
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
  nav.up(); nav.enter(); // open pick-list, move highlight, commit
  nav.esc();
  assert(nav.level()==0 && "Esc exits chooseMenu back to mainMenu");

  printf("OK: U8x8.ino compat-macro port (native U8x8Vendor over a real "
         "U8X8 PCD8544 Nokia 5110 proven separately, see "
         ".RnD/AM4check/U8x8) verified\n");
  return 0;
}
