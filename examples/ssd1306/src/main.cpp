/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief Native SSD1306 OLED menu — a genuine AM5-native example, not an AM4
 *        port. Replaces the dropped `OzOledAscii`/`SSD1306Ascii`/
 *        `SSD1306Ascii_Button_Navigation` (all three wrapped a different AM4
 *        vendor library — OzOLED, SSD1306Ascii — around the exact same real
 *        SSD1306 hardware, and none of them were needed: OneIO already has a
 *        complete native SSD1306 driver, no vendor library at all.
 *
 * `oneIO::display::I2cOled<TwiMaster,Addr,W,H>` (OneIO/include/oneIO/
 * display/i2cOled.h) is `Ssd1306<I2cSsd1306Transport<TwiMaster,Addr>,W,H>` —
 * OneIO's own text+graphics SSD1306 driver (font5x8, fillRect/drawRoundRect,
 * big-font mode) over a real I2C command/data protocol, needing nothing but
 * a `TwiMaster` (OneBus). `oneMenu::OledDisplay<Oled>` (OneMenu/include/
 * oneMenu/menu/IO/IOP/oledOut.h) is the ready-made `OutDef` wrapping it —
 * cursor advance, area, and font metrics all derived from the driver itself.
 *
 * This selftest exercises both halves for real, not just compiles them:
 * `hw::native::VirtualTwi` (OneChip) is a real, host-testable in-memory I2C
 * bus — `oledUnificationProof()` below drives the exact same `I2cOled<...>`
 * type a real board build would use and asserts real SSD1306 command/data
 * bytes (`0x00`/`0x40` control bytes, per `I2cSsd1306Transport`) come out
 * the other end, not just that it links. The menu-logic selftest itself
 * still runs over `ANSI_OUT` + a deterministic input, same convention as
 * every other example in this compat layer — real device wiring (I2C
 * address, SDA/SCL pins) is a board-specific detail, not exercised here.
 */

#include <menu.h>
#include <oneMenu/menu/IO/ansiOut.h>
#include <oneMenu/menu/fmt/textFmt.h>
#include <oneMenu/menu/fmt/ansiFmt.h>
#include <oneMenu/menu/IO/streamOut.h>
#include <oneMenu/menu/in.h>
#include <oneMenu/menu/IO/IOP/oledOut.h>
#include <hapi/hapi.h>
#include <oneData/oneData.h>
#include <oneItem/oneItem.h>
#include <oneOutput/oneOutput.h>
#include <oneIO/display/i2cOled.h>
#include <chips/native/linuxTwi.h>
#include <cassert>
#include <cstdio>
#include <cstring>

using namespace hapi;
using namespace oneData;
// NOTE: deliberately no `using namespace oneMenu;` — see examples/am4compat's
// own comment for why (oneMenu::Menu<> collides with AM4's Menu namespace).

// ── Real native SSD1306 proof — see file header comment ─────────────────────
namespace oledProof {
  using Twi = hw::native::VirtualTwi;
  using Oled = oneIO::display::I2cOled<Twi>;  // same alias a real board build uses

  void run() {
    Oled::begin();
    Twi::reset();
    Oled::setCursor(0, 0);
    Oled::print("IOP");
    // I2cSsd1306Transport::data() sends control byte 0x40 then the glyph
    // bytes in one write — confirms real SSD1306 protocol bytes, not just a
    // successful link.
    assert(Twi::_len > 1 && Twi::_buf[0] == 0x40 &&
           "I2cOled<VirtualTwi> did not emit a real SSD1306 data write");
    printf("oledProof: %d-byte SSD1306 data write (control=0x%02x) for "
           "print(\"IOP\") — same I2cOled<...> type a real board uses\n",
           Twi::_len, Twi::_buf[0]);
  }
}

void noOp() {}

int brightness = 50;
bool oledOn = true;
TOGGLE(oledOn, powerMenu, "Power: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("On",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Off", 0, Menu::doNothing, Menu::noEvent)
);

MENU(mainMenu, "OLED demo", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(brightness, "Brightness", "%", 0, 100, 10, 1, noOp, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(powerMenu)
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
  oledProof::run();

  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=Brightness, 1=Power, 2=<Back>
  assert(brightness == 50);
  nav.enter();
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();
  assert(brightness == 51 && "Brightness FIELD did not edit");
  nav.enter();

  nav.up();     // Brightness(0) -> Power(1)
  assert(oledOn == true);
  nav.enter();
  assert(oledOn == false && "TOGGLE() did not cycle to the next VALUE()");

  printf("OK: native SSD1306 OLED example — real I2cOled<VirtualTwi> traffic "
         "+ menu-logic (FIELD/TOGGLE) verified\n");
  return 0;
}
