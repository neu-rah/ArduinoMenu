/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 ajoy.ino compat-macro port — "output: Serial, input: Analog
 *        Joystick + Serial". See the original at github.com/neu-rah/
 *        ArduinoMenu examples/ajoy/ajoy/ajoy.ino.
 *
 * Previously blocked ("OneInput has its own Joystick/BtnIn but no drop-in
 * AM4-shaped equivalent for analogAxis/keyIn"). The real gap turned out
 * narrower: the button half (`keyIn<1>`, one active-low pulled-up pin) was
 * already fully covered by the existing PCINT-driven `oneInput::BtnCapture`
 * + `Click`/`Hold`/`Debounce` + `avr::AvrBtnPin` + `oneMenu::BtnIn` stack
 * (same one `LiquidCrystal.ino`'s port uses for its encoder button) — only
 * the analog-axis half needed new machinery, since AM4's real `analogAxis
 * <pin,sensivity,inv,center,inertia,bufferSz,posCmd,negCmd>` (verified
 * directly against AM4's real `analogAxisIn.h`) uses different arithmetic
 * entirely from OneInput's existing 2-axis `Joystick` (bit-shift deadzone +
 * inverse-scaled repeat rate, not a linear deadzone) and is single-axis, not
 * 2-axis. Added natively (not bridged): `oneInput::AnalogAxis<Center,
 * Inertia,Sensivity,BufferSz>` (analogAxis.h, reproduces AM4's exact
 * getStep() arithmetic) + `oneInput::avr::AvrAdcAxis<Ch>` (avr/avrAdcAxis.h,
 * the 1-channel sibling of AvrAdcAxes) + `oneMenu::AxisIn<HW,PosCmd,NegCmd>`
 * (menu/IO/IOP/axisIn.h, modeled directly on JoyIn<HW>).
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `doNothing`/`noEvent` (real AM4 no-op shapes) kept as literal
 *    `Menu::doNothing`/`Menu::noEvent` for OP()/MENU() — nothing here needs
 *    a real handler. FIELD()'s own `fn` slot needs a plain `void()` (not
 *    `Menu::doNothing`, which is `bool(int)` — a genuine type mismatch,
 *    not AVR-specific, same as every other port's `action::noOp()`).
 *  - `menuSerialIn` (real Serial input, third `MENU_INPUTS` device) is
 *    dropped both here and in the real AVR harness — real serial input
 *    routes through `InDef::inBurst()`/`TreeNav::doCmd()`, held pending
 *    review since 2026-07-10 (see [[feedback_inburst_review_hold]]), same
 *    reason `concurrent.ino`'s `auxNav` avoids it too.
 *  - The real analog-axis + button hardware (PCINT-driven) is proven
 *    separately at `.RnD/AM4check/ajoy` (gitignored) — same "native
 *    selftest here, real device wiring there" split as `LiquidCrystal.ino`'s
 *    own port.
 *  - AM4's real `inv^field_mode` direction flip (analog axis reverses while
 *    editing a field) is a documented, explicitly-scoped gap in the new
 *    `AnalogAxis`/`AxisIn` machinery itself (see `analogAxis.h`'s own doc
 *    comment) — not exercised here, since `ajoy.ino` itself never combines
 *    the axis and a field-edit that depends on it.
 *
 * The selftest drives nav.up()/enter() directly, same style as every other
 * port in this compat layer.
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
  // FIELD()'s fn slot needs a plain void() (see am4.h's own doc comment on
  // FIELD() — Menu::doNothing is bool(int), a genuine type mismatch here).
  void noOp() {}
}

int test = 500;

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", Menu::doNothing, Menu::noEvent)
  ,OP("Op2", Menu::doNothing, Menu::noEvent)
  ,FIELD(test, "Test", "ms", 0, 1000, 100, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,EXIT("<Back")
);

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// real hardware input (PCINT-driven analog axis + button) is proven
// separately, see file header comment.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/1, &devOut);
NAVROOT(nav, mainMenu, /*maxDepth*/1, in, out);

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=Op1, 1=Op2, 2=Test, 3=<Back>
  assert(nav.sel() == 0);
  nav.up(); nav.up();  // Op1(0) -> Op2(1) -> Test(2)
  assert(nav.sel() == 2);

  nav.enter();  // opens edit mode on the FIELD
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  // edit mode: Up increases (natural mapping, item.h). FIELD()'s own
  // step/tune (100/1 here) are accepted but ignored — always steps by 1
  // (am4.h's own documented behavior, same as every other FIELD()-using
  // port in this compat layer).
  nav.up();
  assert(test == 501 && "FIELD() did not step the bound variable by 1");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  printf("OK: ajoy.ino compat-macro port (native AnalogAxis/AxisIn + PCINT "
         "button hardware proven separately, see .RnD/AM4check/ajoy) "
         "verified\n");
  return 0;
}
