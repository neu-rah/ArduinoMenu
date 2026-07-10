/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 CustomPrompt.ino compat-macro port — "self update custom
 *        field": a live seconds-counter display that redraws itself
 *        automatically as its value changes. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/CustomPrompt/CustomPrompt/
 *        CustomPrompt.ino.
 *
 * Pure assembly — every macro/mechanism this file needs already exists in
 * am4.h (`MENU`/`OP`/`EXIT`, `IdleTimeout`+`NAVROOT_IDLE`). No new
 * compat-layer machinery needed.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - AM4's `altPrompt` (a hand-subclassed `prompt` overriding `printTo()`
 *    and `changed()`) is dropped, but *not* just as a cosmetic trim like
 *    Confirm's `altMENU` — it's replaced with the equivalent OneMenu-native
 *    pattern: `oneData::Watch<oneData::DataFn<Src>>` already does exactly
 *    "print a live value, redraw when it changes" via the same `changed()`/
 *    `sync()` machinery every other dynamic field in this codebase already
 *    uses (e.g. `Watch<Default<Int,10>>`) — no custom prompt subclass
 *    needed at all, same "no dedicated component is needed" conclusion
 *    `Confirm`'s own port already reached for dialogs.
 *  - `myLedOn`/`myLedOff` (real AM4 signature: 0-arg) ported as `bool(int)`
 *    — same established adapter shape as `Confirm`'s `systemExit`.
 *  - AM4's own `nav.doNav(navCmd(idxCmd,1))`/`nav.doNav(navCmd(enterCmd))`
 *    "driving the menu via API calls" demo (in `setup()`) is translated to
 *    the OneMenu-native equivalent (`nav.up()`/`nav.enter()`) in the
 *    selftest below — same intent (exercise LED on/off via direct nav
 *    calls, not real input), not a literal API translation (OneMenu has no
 *    `doNav(navCmd)` equivalent).
 *  - The idle system (`nav.idleTask`) is adapted onto `RunLoop`/
 *    `am4compat::IdleTimeout`, same pattern `examples/fullIdle` already
 *    established.
 *
 * The selftest drives nav.up()/enter()/esc() and Run::run() directly, same
 * style as every other port in this compat layer. The live-clock check
 * does one real ~1.1s wait (hw::delay_ms) rather than a deterministic
 * stand-in — there's no faster way to prove "value changes after a real
 * second elapses" without diverging the tested behavior from the real one.
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
#include <oneChip/clock.h>
#include <cassert>
#include <cstdio>

using namespace hapi;
using namespace oneData;
// NOTE: deliberately no `using namespace oneMenu;` — see examples/am4compat's
// own comment for why (oneMenu::Menu<> collides with AM4's Menu namespace).

namespace action {
  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }
}

// ── the live seconds counter — replaces AM4's altPrompt subclass ──────────
struct SecondsAccessor {
  static int get() noexcept { return (int)(hw::millis()/1000); }
  static void set(int) noexcept {}  // read-only display
};
using ClockField = oneMenu::ItemDef<oneData::Watch<oneData::DataFn<SecondsAccessor>>, Text>;

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,ClockField{""}
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,EXIT("<Back")
);

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
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);

// ── idle system: AM4's nav.idleTask, adapted onto RunLoop via
// am4compat::IdleTimeout + NAVROOT_IDLE — same pattern examples/fullIdle
// already established. ───────────────────────────────────────────────────
bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;
using Idle = am4compat::IdleTimeout<50, Run>;

NAVROOT_IDLE(nav, mainMenu, /*maxDepth*/2, in, out, Run);

int idleStartCount = 0, idleEndCount = 0, idleTicks = 0;
bool idleRun() {
  if(idleTicks==0) { idleStartCount++; printf("suspending menu!\n"); }
  idleTicks++;
  printf(".");
  if(idleTicks>=3) {
    printf("\n");
    idleEndCount++;
    printf("resuming menu.\n");
    idleTicks=0;
    Run::idleOff();
  }
  return true;
}

bool mainRun() {
  bool activity = nav.poll();
  Idle::tick(activity, idleRun);
  return true;
}

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=clock, 1=LED On, 2=LED Off, 3=<Back>

  // ── live seconds counter: real time must actually elapse and the field
  // must report itself changed once it does (Watch<>'s own get()!=watched
  // detection, same machinery every other dynamic field in this codebase
  // already relies on — no custom prompt subclass needed). ────────────────
  auto& clock = mainMenu.find<SameAs<oneData::Watch<oneData::DataFn<SecondsAccessor>>>>();
  clock.sync();
  assert(!clock.changed() && "freshly-synced clock must report unchanged");
  hw::delay_ms(1100);
  assert(clock.changed() && "live clock did not detect a real elapsed second");
  clock.sync();
  assert(!clock.changed());

  // ── LED On/Off (0-arg AM4 handlers ported as bool(int)) — driven via
  // direct nav calls, same intent as AM4's own doNav(navCmd) demo ─────────
  assert(action::ledOnCount == 0);
  nav.up();     // clock(0) -> LED On(1)
  nav.enter();
  assert(action::ledOnCount == 1 && "LED On did not fire");
  nav.up();     // LED On(1) -> LED Off(2)
  nav.enter();
  assert(action::ledOffCount == 1 && "LED Off did not fire");

  // ── Idle system: IdleTimeout auto-triggers idleRun after inactivity,
  // same assertion style as examples/fullIdle's own selftest ──────────────
  assert(idleStartCount == 0);
  int guard=0;
  while(!Run::active() && guard++<200) { Run::run(); hw::delay_ms(1); }
  assert(Run::active() && "IdleTimeout did not auto-trigger idleOn after the timeout window");
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1 &&
         "idleRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: CustomPrompt.ino compat-macro port (live Watch<>-based field + idle) verified\n");
  return 0;
}
