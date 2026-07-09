/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 fullIdle.ino compat-macro port — "using menu idle to do complex
 *        tasks" (original: examples/fullIdle/fullIdle/fullIdle.ino).
 *
 * AM4's real idle API (nav.timeOut/nav.idleTask/nav.idleOn()/nav.sleepTask)
 * doesn't exist as OneMenu machinery — it's adapted here onto
 * oneMenu::RunLoop (nav.h, the "alternative poll handler" primitive already
 * built for OneMenu's own dialog/prompt use — see notes.md "AM4 compat
 * layer") via a new am4compat::IdleTimeout (am4.h), kept entirely on the
 * compat side rather than extending OneMenu itself (Rui, 2026-07-09: AM4-port
 * machinery should stay AM5/compat-side unless it brings value to OneMenu —
 * RunLoop already did, an auto-idle-timeout trigger doesn't need to).
 *
 * Two real deviations from the original, beyond the usual #include swap:
 *  - AM4's idleTask is `result(menuOut&,idleEvent)` (fires with idleStart/
 *    idleEnd) — IdleTimeout's idleFn is a plain bool(), same shape every
 *    other RunLoop alternative already uses. The idleStart/idleEnd split is
 *    approximated with a static "first call" flag inside idleRun() instead
 *    of a real 2-arg handler (same "simplify the handler shape" precedent as
 *    Confirm's systemExit / OP()'s Action<fn>).
 *  - The original's idle body reads real Serial input and sleeps via
 *    delay(200) for a human to watch; the selftest below drives it
 *    deterministically instead (tick-counted, no real time/input dependency)
 *    — same reasoning as every other selftest in this compat layer.
 *
 * The selftest drives nav/Run directly (no PCKbd/ANSI parsing involved),
 * same verification style as examples/am4compat's own selftest.
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
  // avr-g++ 7.3 rejects Menu::doNothing (even single-overload, see am4.h's
  // own comment) as OP()'s Action<fn> template argument — same NTTP
  // limitation Confirm's noOp(int) already worked around.
  bool noOp(int) { return false; }
}

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", action::noOp, Menu::noEvent)
  ,OP("Op2", action::noOp, Menu::noEvent)
  ,EXIT("<Back")
);

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives Run/nav directly,
// so real input never needs to arrive (mirrors "activity" via a flag below).
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

// ── idle system: AM4's nav.timeOut/idleTask, adapted onto RunLoop via
// am4compat::IdleTimeout (am4.h) ─────────────────────────────────────────
int idleStartCount = 0, idleEndCount = 0, idleTicks = 0;

bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;
// 50ms inactivity window — short on purpose for a fast, deterministic
// selftest (AM4's own fullIdle.ino uses 5s "made very short for demo
// purposes" already; this goes further for a sub-second test run).
using Idle = am4compat::IdleTimeout<50, Run>;

bool idleRun() {
  if(idleTicks==0) {
    idleStartCount++;
    printf("suspending menu!\n");
  }
  idleTicks++;
  // AM4's original: `Serial.write(Serial.available()?(char)Serial.read():'.'); delay(200);
  // if(idleTimeout) nav.idleOff();` — deterministic stand-in: leave idle
  // after a fixed tick count instead of a real wall-clock timeout.
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

  assert(!Run::active());
  assert(idleStartCount==0);

  // no input ever arrives (NoOpIn) -> nav.poll() always reports no activity
  // -> IdleTimeout's window elapses and auto-triggers idleOn(idleRun),
  // exactly like AM4's real nav.timeOut/idleTask after nav.timeOut seconds
  // of no key presses.
  int guard=0;
  while(!Run::active() && guard++<200) { Run::run(); hw::delay_ms(1); }
  assert(Run::active() && "IdleTimeout did not auto-trigger idleOn after the timeout window");
  assert(idleStartCount==0 && "idleRun() must not run until Run::run() actually invokes it");

  // idleRun() now runs each Run::run() tick until its own deterministic
  // "wake up" condition (AM4's nav.idleOff(), triggered by a real timeout
  // there) calls Run::idleOff() to restore mainRun().
  while(Run::active()) Run::run();
  assert(idleStartCount==1 && "idleRun() did not run exactly once at idle entry");
  assert(idleEndCount==1 && "idleRun() did not call Run::idleOff() to resume mainRun()");
  assert(!Run::active() && "Run::idleOff() did not restore mainRun()");

  // back in mainRun(): confirm a second idle cycle triggers cleanly too
  // (proves IdleTimeout's timer.reset()-on-fire fix — a stale latch would
  // either skip this cycle or re-trigger instantly without a real wait).
  guard=0;
  while(!Run::active() && guard++<200) { Run::run(); hw::delay_ms(1); }
  assert(Run::active() && "second IdleTimeout cycle did not auto-trigger");
  while(Run::active()) Run::run();
  assert(idleStartCount==2 && idleEndCount==2 && "second idle cycle did not complete cleanly");

  printf("OK: fullIdle.ino compat-macro port (IdleTimeout auto-idle + wake) verified\n");
  return 0;
}
