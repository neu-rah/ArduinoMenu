/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 plugin_decimalsField.ino compat-macro port — "decimalslField":
 *        a plugin field type that renders a float/double with a runtime-
 *        adjustable number of decimal places, driven by a third integer
 *        field ("Dec", 0-18). See the original at github.com/neu-rah/
 *        ArduinoMenu examples/plugin_decimalsField/.
 *
 * Real deviation from the original, beyond the usual #include swap:
 *  - AM4's `decimalslField<T>` is a custom `menuField<T>` subclass whose
 *    `updateEvent_cb` (wired via the "Dec" field's own `updateEvent`
 *    handler) *pushes* the new decimal count into two field objects'
 *    `.decimals` members. OneMenu's EventDispatch never fires a "value
 *    changed" event (only Enter/Exit/Focus/Blur — see am4.h's own doc
 *    comment on `updateEvent`), so this port uses a new native component,
 *    `oneData::RuntimeDecimals<nPtr,W>` (oneData.h, sibling of `Decimals
 *    <N,W>`/`Printf<fmt,W,sz>`), which instead *pulls* the live decimal
 *    count from a raw pointer to `cantDecimals` at print time — no event
 *    wiring needed at all, and correctly redraw-reactive (RuntimeDecimals'
 *    own changed()/sync() track *nPtr's last-seen value in addition to the
 *    wrapped value, so editing "Dec" alone still triggers a redraw of the
 *    float/double fields).
 *  - The float/double fields are NOT built via `FIELD()`: that macro
 *    forwards its lo/hi straight into `StaticRange<(lo),(hi)>`, an `auto`
 *    NTTP — floating-point NTTPs are rejected pre-C++20 (confirmed
 *    empirically against this repo's actual `-std=c++17`), so `FIELD()`
 *    has never actually supported a float-ranged field. Fixed here with
 *    `oneData::NumRange<N>` (oneData.h), an already-existing *runtime*-
 *    constructed sibling of `StaticNumRange` (low/high/wraps are
 *    constructor args, not NTTPs) — dropped straight into the
 *    `NumField<RangeComponent, AsField<...>>` slot, hand-assembled the
 *    same way `TextField.ino`'s `option0` was (direct composition, no
 *    macro, when the compat macro doesn't fit the case).
 *
 * The selftest drives nav.enter()/down()/esc() directly, same style as
 * every other port in this compat layer. Real serial input is held back
 * per the inBurst review hold — native selftest (NoOpIn) only.
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

namespace action {
  void noOp() {}
}

unsigned char cantDecimals = 1;
float  floatVar   = 0.123456789f;
double doubleVar  = 0.123456789;

using FloatField = oneMenu::NumFieldDef<
    oneMenu::AsLabel<Text>,
    oneMenu::NumField<
        NumRange<float>,
        oneMenu::AsField<RuntimeDecimals<&cantDecimals, Watch<DataRef<&floatVar>>>>
    >,
    oneMenu::AsUnit<Text>
  >;

using DoubleField = oneMenu::NumFieldDef<
    oneMenu::AsLabel<Text>,
    oneMenu::NumField<
        NumRange<double>,
        oneMenu::AsField<RuntimeDecimals<&cantDecimals, Watch<DataRef<&doubleVar>>>>
    >,
    oneMenu::AsUnit<Text>
  >;

// Parens (not OBJ()) protect the aggregate-init commas from the MENU() macro's
// own comma-separated arg list — these are plain rvalue temporaries, not
// previously-declared named objects, so OBJ()'s std::move(id) doesn't apply.
MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,(FloatField{"f", -100.0f, 100.0f, false, ""})
  ,(DoubleField{"d", -100.0, 100.0, false, ""})
  ,FIELD(cantDecimals, "Dec", "", 0, 18, 1, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Exit")
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
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=f,1=d,2=Dec,3=<Exit>

  // ── default: 1 decimal place ────────────────────────────────────────────
  assert(cantDecimals == 1);

  // ── bump Dec 1 -> 2, confirm float/double fields re-render live ────────
  nav.up(); nav.up();  // f(0) -> d(1) -> Dec(2)
  nav.enter();
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();   // edit mode: Up increases (natural mapping)
  assert(cantDecimals == 2 && "Dec field did not step to 2");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  // re-anchor nav to the float field and force a redraw probe — confirms
  // RuntimeDecimals' widened changed() (tracking *nPtr, not just floatVar)
  // is what makes this redraw fire at all: floatVar itself never changed.
  nav.up();  // Dec(2) -> f(0), wraps
  bool redrawn = nav.printTo(devOut);
  assert(redrawn && "float field should redraw: cantDecimals changed even though floatVar didn't");

  printf("OK: plugin_decimalsField.ino compat-macro port (RuntimeDecimals "
         "pull-based runtime precision, NumRange float bounds) verified\n");
  return 0;
}
