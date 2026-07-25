/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 customField.ino compat-macro port — a numeric field that
 *        reverts to its pre-edit value on Esc instead of keeping the live
 *        edit. See github.com/neu-rah/ArduinoMenu
 *        examples/customField/customField/customField.ino.
 *
 * NOT a macro port. AM4's own customField.ino builds this by subclassing
 * `menuField<T>` (cancelField) and overriding its virtual doNav() — there is
 * no OneMenu counterpart to auto-convert (compile-time component chains
 * aren't virtual-dispatch subclassable), and no `altFIELD()` macro exists in
 * am4.h for this reason. Confirmed no blocker for the *behavior* itself
 * though: `oneMenu::CancelOnEsc` (item.h) is a real, general-purpose OneMenu
 * component reproducing cancelField's exact semantics — save the value on
 * entering edit mode, restore it if edit mode is left via Esc, leave it
 * alone on a real Enter-commit. It lives in item.h (not am4.h) because it
 * has standalone OneMenu value beyond this one AM4 example, same category as
 * OnChange/OnUpdate right above it.
 *
 * "Custom edit" below is therefore hand-composed (bypassing FIELD()'s
 * NumFieldDef, which has no hook before its own fixed EditField slot) —
 * CancelOnEsc, EditField, NumField<...> — same ItemDef-component-list style
 * already used by examples/fields' own Date field (see OneMenu's own
 * examples/fields/src/main.cpp). "Original edit" alongside it is a plain,
 * unmodified FIELD() for direct comparison, matching the original .ino.
 *
 * Deliberate deviations from the original, beyond the usual #include swap:
 *  - FIELD()'s fn slot wants a real, non-overloaded void() — Menu::doNothing
 *    is bool(int), doesn't fit — same constraint every prior FIELD() port
 *    already needed (see examples/serialio's own action::noOp).
 *  - SERIAL_OUT(Serial)/serialIn(Serial) swapped for ANSI_OUT + a
 *    deterministic NoOpIn — no real Serial under `platform=native`, same
 *    convention as every other port in this directory.
 *
 * The selftest drives nav.up()/enter()/esc() directly, same verification
 * style as examples/{am4compat,serialio,fullIdle}'s own selftests.
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

int test=55;

void noOp() {}

// "Custom edit" — hand-composed, reproduces cancelField's revert-on-Esc.
using CustomField = ::oneMenu::ItemDef<
  ::oneMenu::AsEditMode<>,
  ::oneMenu::AsLabel<::oneData::Text>,
  ::oneMenu::CancelOnEsc,
  ::oneMenu::EditField,
  ::oneMenu::NumField<
    ::oneData::StaticNumRange<::oneData::StaticRange<0,100>>,
    ::oneMenu::AsField<::oneData::Watch<::oneData::DataRef<&test>>>
  >,
  ::oneMenu::AsUnit<::oneData::Text>
>;

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,CustomField{"Custom edit","%"}
  ,FIELD(test,"Original edit","%",0,100,10,1,noOp,Menu::noEvent,Menu::wrapStyle)
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
  devOut.clear();
  nav.printTo(devOut);

  // index 0 = Custom edit, 1 = Original edit
  assert(test == 55);
  nav.enter();  // open Custom edit
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.up();     // Up increases in edit mode
  nav.up();
  assert(test == 57 && "CancelOnEsc must not interfere with live editing");
  nav.esc();    // cancel — must revert to the pre-edit value
  assert(nav.navMode() != oneMenu::NavMode::Edit);
  assert(test == 55 && "CancelOnEsc did not revert the value on Esc");

  nav.enter();  // open Custom edit again
  nav.up();
  assert(test == 56);
  nav.enter();  // commit — real Enter, must NOT revert
  assert(nav.navMode() != oneMenu::NavMode::Edit);
  assert(test == 56 && "CancelOnEsc must not revert on a real commit");

  nav.up();     // Custom edit(0) -> Original edit(1)
  nav.enter();  // open — plain FIELD(), no revert behavior
  nav.up();
  assert(test == 57);
  nav.esc();    // plain FIELD() keeps the live-edited value on Esc
  assert(test == 57 && "plain FIELD() must keep the live edit on Esc (no CancelOnEsc)");

  printf("OK: customField.ino compat-macro port — CancelOnEsc revert-on-Esc verified\n");
  return 0;
}
