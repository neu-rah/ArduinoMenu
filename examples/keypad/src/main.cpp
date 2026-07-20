/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 keypad.ino compat-macro port — "Digital keypad (4x4 matrix)
 *        using the menu keypad driver". See the original at
 *        github.com/neu-rah/ArduinoMenu examples/keypad/keypad/keypad.ino.
 *
 * AM4's real `keypadIn` (menuIO/keypadIn.h) does no matrix-to-command
 * translation itself — it's a bare passthrough over a real `Keypad&`
 * object's own `getKey()`, delegating 100% of key-to-char mapping to the
 * vendor library's own `keys[ROWS][COLS]` table (confirmed directly
 * against both the real AM4 source and the real Keypad library — the
 * table genuinely supports mapping any physical key to any arbitrary
 * char, not just nav command codes, as this example's own hex-digit
 * keymap demonstrates). Ported the same way as `U8g2Vendor`/`AdaGfxVendor`
 * (this session's own established policy for vendor libraries): a thin
 * `oneMenu::KeypadVendor<Keypad,customKeypad>` calling the real vendor
 * object directly, in the same InDef chain slot as `UartIn`/`SerialIn` —
 * a raw char source sitting below `PCKbd`, not a new parsing mechanism.
 *
 * Real hardware (4x4 matrix, rows=6/7/8/9, cols=2/3/4/5, matching the
 * original's own pins) is proven separately at `.RnD/AM4check/keypad`
 * (gitignored) — same "native selftest here, real device wiring there"
 * split as every other port this session.
 *
 * The menu content itself needs no new machinery: `EDIT("Hex",buf1,
 * hexNr,...)` is a 4-*distinct*-position mask (`{"0","x",hexDigit,
 * hexDigit}` — fixed "0x" prefix then 2 free hex digits), the exact same
 * shape `clickEncoder.ino`'s own Hex field already proved. `CHOOSE`/`OP`/
 * `EXIT` are all already-proven macros.
 *
 * The selftest drives nav.up()/enter() directly, same style as every
 * other port in this compat layer.
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

// Hex field: 4 *distinct* positions ("0","x",hexDigit,hexDigit — a fixed
// "0x" prefix then 2 free hex digits), same shape as clickEncoder.ino's
// own Hex field.
static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

namespace action {
  // EDIT()'s fn slot needs a plain void() (see am4.h's own doc comment on
  // FIELD()/EDIT() — Menu::doNothing is bool(int), a genuine type mismatch
  // here, same fix as every other port's FIELD()).
  void noOp() {}
}

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

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// real hardware input (a real 4x4 Keypad matrix via KeypadVendor) is
// proven separately, see file header comment.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);
NAVROOT(nav, mainMenu, /*maxDepth*/2, in, out);

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=Op1, 1=Hex, 2=chooseMenu, 3=<Back>
  assert(nav.sel() == 0);
  nav.up();  // Op1(0) -> Hex(1)
  assert(nav.sel() == 1);

  // EDIT() Hex — 4-distinct-position mask, same shape as clickEncoder.ino's
  // own IP/Hex fields (fixed "0x" prefix, 2 free hex digits)
  assert(strcmp(buf1, "0x11") == 0);
  nav.enter();  // opens edit mode
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();     // cycle position 0 — fixed to "0", must stay unchanged
  assert(buf1[0] == '0' && "EDIT()'s fixed position-0 validator ({\"0\"}) must not change");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  // CHOOSE submenu — enter opens the pick-list, up moves the highlight,
  // second enter commits, same 3-step sequence every other CHOOSE()-using
  // port in this compat layer already proved.
  nav.up();     // Hex(1) -> chooseMenu(2)
  nav.enter();
  nav.up();
  nav.enter();
  assert(chooseTest == 2 && "CHOOSE() did not commit the highlighted VALUE()");

  printf("OK: keypad.ino compat-macro port (native KeypadVendor over a real "
         "Keypad matrix proven separately, see .RnD/AM4check/keypad) "
         "verified\n");
  return 0;
}
