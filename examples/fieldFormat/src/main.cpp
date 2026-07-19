/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 fieldFormat.ino compat-macro port — a custom-formatted numeric
 *        field ("Custom edit", printed with leading zeros, "%03d") next to a
 *        plain field ("Original edit") bound to the SAME variable. See the
 *        original at github.com/neu-rah/ArduinoMenu examples/fieldFormat/
 *        fieldFormat/fieldFormat.ino.
 *
 * Real AM4's whole point here is `leadsField<T>:public menuField<T>` — a
 * hand-subclassed field overriding `printTo()` to `sprintf("%03d",value)`
 * instead of the plain decimal print. `altFIELD(leadsField,...)` is
 * structurally out of scope for this macro-compat layer, same reasoning as
 * `Confirm`'s `altMENU`/`CustomPrompt`'s `altPrompt` — a macro can't
 * synthesize an arbitrary user subclass inside a nested expression.
 *
 * The underlying *capability* (printf-style field formatting) doesn't need
 * that subclass at all: `oneData::RuntimePrintf<W,sz>` (added this session,
 * completing the `Decimals`/`RuntimeDecimals`/`Printf` family with a runtime
 * format string) is exactly "wrap a Data value, reformat its display" —
 * composed directly into the field's `AsField<...>` slot instead of going
 * through `FIELD()` (which always binds `AsField<Watch<DataRef<&var>>>`, no
 * formatting hook). Same "compose directly when the compat macro doesn't
 * fit" precedent as `TextField`'s `option0`/`plugin_decimalsField`'s
 * float/double fields — spliced into a real `MENU()` body via `OBJ()`,
 * matching `targetSel`'s own `OBJ(targetsMenu)` precedent.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `leadsField`'s real AM4 constructor also takes `tunning`-mode
 *    accel/style params it shares with plain `menuField`; those are already
 *    documented as accepted-but-ignored throughout this compat layer
 *    (`FIELD()`'s own `step`/`tune`), so no new gap here.
 *  - `doNothing`/`enterEvent` (the custom field's own handler/mask) ported
 *    as a real `void()` (`EventCall`'s shape), same adapter every other
 *    port in this compat layer already uses for AM4's 0-arg handlers.
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

int test = 55; // the one variable both fields edit — same as the real source

void noOpEdit() {} // AM4's doNothing, void()-shaped for EventCall

// minimal fake device for direct printItem() inspection in the selftest below
// (checking rendered TEXT content, not just nav/state side effects) — same
// shape as OneData's own test.cpp StrOut.
struct StrOut {
  char buf[64]{};
  int len=0;
  void put(char c) { if(len<63) { buf[len++]=c; buf[len]=0; } }
  void put(const char* s) { for(;*s;++s) put(*s); }
  void put(long v) {
    if(v<0) { put('-'); v=-v; }
    char tmp[16]; int n=0;
    if(v==0) tmp[n++]='0';
    while(v>0) { tmp[n++]=(char)('0'+(v%10)); v/=10; }
    while(n>0) put(tmp[--n]);
  }
  void put(int v) { put((long)v); }
  void put(double v) { put((long)v); }
  template<oneMenu::Fmt tag> void fmtStart(const oneMenu::Ctx&) {}
  template<oneMenu::Fmt tag> void fmtStop(const oneMenu::Ctx&) {}
};

// ── the custom-formatted field — replaces AM4's leadsField<T> subclass ────
// Label/unit are StaticText (compile-time, matching how most examples in
// this codebase already declare fixed labels), not runtime Text — sidesteps
// a real, separate fragility in oneData::Data<T>'s own dual constructor
// (a plain-value-consuming template and a fully-forwarding template): with
// two runtime Text components sandwiching RuntimePrintf's own one extra arg,
// overload resolution across the whole chain doesn't reliably pick "each
// Text consumes exactly one string, RuntimePrintf's fmt lands in between" —
// confirmed via direct, isolated repro, not assumed. Since the label/unit
// here are fixed strings anyway (never runtime-set), StaticText removes two
// of the three ambiguous slots entirely, leaving only RuntimePrintf's fmt
// as a real runtime constructor argument.
static constexpr CText customLabel{"Custom edit"};
static constexpr CText percentUnit{"%"};

using CustomEditField = oneMenu::NumFieldDef<
  oneMenu::AsLabel<StaticText<&customLabel>>,
  oneMenu::NumField<
    StaticNumRange<StaticRange<0,100>>,
    oneMenu::AsField<RuntimePrintf<Watch<DataRef<&test>>>>
  >,
  oneMenu::AsUnit<StaticText<&percentUnit>>,
  oneMenu::EventCall<oneMenu::EventMask::Enter, noOpEdit>
>;
CustomEditField customField{"%03d"};

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OBJ(customField)
  ,FIELD(test, "Original edit", "%", 0, 100, 10, 1, noOpEdit, Menu::noEvent, Menu::noStyle)
);

ANSI_OUT(devOut, 40, 6);

// deterministic zero-op input source — the selftest drives nav directly.
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

  // index 0 = Custom edit (OBJ()'d), 1 = Original edit (FIELD())

  // ── rendered output: the custom field must show leading-zero "%03d",
  // the plain field must show the SAME underlying value unformatted —
  // exactly the comparison fieldFormat.ino itself demonstrates. ───────────
  {
    StrOut out0; oneMenu::Ctx ctx0{};
    customField.printItem(out0, ctx0);
    assert(strstr(out0.buf, "055") && "custom field did not render leading-zero \"055\"");
  }

  // ── both fields edit the SAME `test` variable — editing one must be
  // visible through the other. ─────────────────────────────────────────
  int before = test;
  nav.enter();                 // Custom edit: Enter -> edit mode
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();                    // step +10 (StaticNumRange<0,100>, step from FIELD-equivalent range)
  assert(test != before && "editing the custom field did not change the shared variable");
  nav.enter();                 // leave edit mode
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  int afterCustomEdit = test;
  nav.down();                  // Custom edit(0) -> Original edit(1)
  nav.enter();                 // Original edit: Enter -> edit mode
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();                    // step again, via the PLAIN FIELD() field this time
  assert(test != afterCustomEdit &&
         "editing the plain FIELD() field did not change the shared variable");
  nav.enter();

  // ── re-render the custom field after the plain field's own edit —
  // confirms RuntimePrintf picks up externally-changed values, not just
  // its own field's own edits. ─────────────────────────────────────────
  {
    StrOut out1; oneMenu::Ctx ctx1{};
    customField.printItem(out1, ctx1);
    char expected[8];
    snprintf(expected, sizeof(expected), "%03d", test);
    assert(strstr(out1.buf, expected) &&
           "custom field did not reflect a value changed via the plain field");
  }

  printf("OK: fieldFormat.ino compat-macro port (RuntimePrintf custom field + shared plain field) verified\n");
  return 0;
}
