/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 serialio.ino compat-macro port — the "everything" showcase:
 *        TOGGLE/SELECT/CHOOSE, a submenu with real event handlers, a
 *        PADMENU date-entry field group, two masked EDIT() text fields, and
 *        the idle system (auto-timeout + a manually-triggered "alert").
 *        See the original at github.com/neu-rah/ArduinoMenu
 *        examples/serialio/serialio/serialio.ino.
 *
 * Pure assembly — unlike most of this compat layer's earlier ports, every
 * macro/mechanism this file needs already exists in am4.h (TOGGLE/SELECT/
 * CHOOSE/VALUE, EDIT, FIELD, PADMENU, MENU, OP's 1/2/3-arg auto-dispatch,
 * NAVROOT_IDLE, am4compat::IdleTimeout). No new compat-layer machinery was
 * added for this port.
 *
 * Deliberate deviations from the original, beyond the usual #include swap:
 *  - `altOP(altPrompt,"",showEvent,anyEvent)` (a custom-prompt subclass,
 *    virtual printTo() override) is dropped, not faked — same precedent as
 *    handlers.ino's own altOP drop.
 *  - `showEvent` keeps its real, unmodified 3-arg AM4 signature
 *    (bool(EventMask,INav&,IItem&)) everywhere it's used as OP()'s fn
 *    (Sub1/Sub2/Sub3) — today's new EventActionItemNav branch handles this
 *    directly, a fidelity upgrade over handlers.ino's own port (which had
 *    to trim showEvent to 1-arg since 3-arg didn't exist yet). subMenu's
 *    own *title*-level fn slot (MENU()'s own fn/mask) only recognizes
 *    bool(EventMask) (am4compat::menuDefStyle) — handlers.ino already hit
 *    this exact situation for this exact function; same fix here: a
 *    separate 1-arg `titleEvent()` wrapper sharing showEvent's event
 *    counters, used only in that one slot.
 *  - `action1` (real AM4 signature: 1-arg, `result(eventMask)`) is ported as
 *    `bool(EventMask,IItem&)` (2-arg, unused IItem&) — the *existing*
 *    EventActionItem branch already gives this full anyEvent dispatch with
 *    zero functional loss; same "trim the handler shape, keep the
 *    semantics" precedent as Confirm's systemExit.
 *  - `myLedOn`/`myLedOff`/`zZz` (real AM4 signature: 0-arg) ported as
 *    bool(int) (myLedOn/myLedOff, unused param — same Confirm precedent) or
 *    left genuinely 0-arg (zZz — mainMenu's title mask is noEvent anyway, so
 *    menuDefStyle's arity mismatch costs nothing; no adapter needed).
 *  - The idle system (nav.timeOut/idleTask, nav.idleOn(alert) from doAlert)
 *    is adapted onto RunLoop/am4compat::IdleTimeout/NAVROOT_IDLE, exactly
 *    like examples/fullIdle's already-established pattern: idleRun()/
 *    alertRun() are plain bool() functions using a tick counter as a
 *    deterministic stand-in for real elapsed time/keypresses. One real,
 *    documented gap: AM4's idle() calls nav.reset() on wake — OneMenu's nav
 *    has no equivalent method (confirmed: no nav.h nav type exposes one);
 *    dropped, not faked.
 *  - LEDPIN/pinMode/digitalWrite (real Arduino GPIO calls) are dropped —
 *    no real hardware in this native-only selftest, same convention as
 *    every other port in this directory. HIGH/LOW become literal 1/0.
 *
 * The selftest drives nav.up()/down()/enter()/esc() and Run::run() directly
 * (no PCKbd/ANSI parsing involved), same verification style as
 * examples/{am4compat,handlers,fullIdle}'s own selftests.
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
  // ── OP()'s 1-arg-real-AM4-shape trim (action1) ──────────────────────────
  int op1Count = 0;
  bool action1(oneMenu::EventMask, oneMenu::IItem&) { op1Count++; return true; }

  // ── OP()'s real 3-arg shape (action2) — no trim needed ──────────────────
  int op2Count = 0;
  bool action2(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { op2Count++; return true; }

  // ── showEvent (real 3-arg, used by Sub1/Sub2/Sub3's OP()) + titleEvent
  // (1-arg trim, used only by subMenu's own MENU() title fn slot) — share
  // one set of counters so either path is observable the same way.
  int enterCount = 0, exitCount = 0, focusCount = 0, blurCount = 0;
  void countEvent(oneMenu::EventMask e) {
    if(e & oneMenu::EventMask::Enter) enterCount++;
    if(e & oneMenu::EventMask::Exit)  exitCount++;
    if(e & oneMenu::EventMask::Focus) focusCount++;
    if(e & oneMenu::EventMask::Blur)  blurCount++;
  }
  bool showEvent(oneMenu::EventMask e, oneMenu::INav&, oneMenu::IItem&) { countEvent(e); return true; }
  bool titleEvent(oneMenu::EventMask e) { countEvent(e); return true; }

  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }

  bool zZz() { return true; }  // real AM4 signature: 0-arg; mainMenu's mask is noEvent anyway.

  // FIELD()/EDIT()'s fn slot wants a real, non-overloaded void() — same
  // constraint every prior FIELD()/EDIT() port already needed (Menu::doNothing
  // is bool(int), doesn't fit).
  void noOp() {}
}

float test = 55;

// ── TOGGLE/SELECT/CHOOSE — direct port, already fully supported ───────────
int ledCtrl = -1;  // -1: neither VALUE() below — proves SyncValue actually wrote something.
TOGGLE(ledCtrl, setLed, "Led: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("On", 1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Off", 0, Menu::doNothing, Menu::noEvent)
);

int selTest = 0;
SELECT(selTest, selMenu, "Select", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("Zero", 0, Menu::doNothing, Menu::noEvent)
  ,VALUE("One",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Two",  2, Menu::doNothing, Menu::noEvent)
);

int chooseTest = -1;
CHOOSE(chooseTest, chooseMenu, "Choose", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("First",  1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Second", 2, Menu::doNothing, Menu::noEvent)
  ,VALUE("Third",  3, Menu::doNothing, Menu::noEvent)
  ,VALUE("Last",  -1, Menu::doNothing, Menu::noEvent)
);

// altOP(altPrompt,"",showEvent,anyEvent) dropped — custom-prompt subclass,
// see file header comment.
MENU(subMenu, "Sub-Menu", action::titleEvent, Menu::anyEvent, Menu::noStyle
  ,OP("Sub1", action::showEvent, Menu::anyEvent)
  ,OP("Sub2", action::showEvent, Menu::anyEvent)
  ,OP("Sub3", action::showEvent, Menu::anyEvent)
  ,EXIT("<Back")
);

// ── PADMENU date-entry group ────────────────────────────────────────────
uint16_t year = 2017, month = 10, day = 7;
PADMENU(birthDate, "Birth", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,FIELD(year,  "", "/", 1900, 3000, 20, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(month, "", "/", 1,    12,   1,  0, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,FIELD(day,   "", "",  1,    31,   1,  0, action::noOp, Menu::noEvent, Menu::wrapStyle)
);

// ── EDIT() masked text fields ───────────────────────────────────────────
// NOTE: declared WITHOUT the trailing pointee-const (see EDIT()'s own doc
// comment) — CharMask::PosSet<CText*>'s NTTP type is const char**.
static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

// single-entry validator array, cycled across every position via
// CharMask::PosSet's own `pos % N` design (am4.h's EDIT() doc comment
// already names this exact shape, from TextField.ino's identical case).
static const char* alphaNum = " 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,+-";
static const char* alphaNumMask[] = {alphaNum};
char name[21] = "                    ";

// forward-declared: doAlert's body needs `nav` (declared further below via
// NAVROOT_IDLE), but its address is needed here for OP()'s NTTP — same
// trick fullIdle.ino/navHandler already use for mainFn/idleFn.
bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", action::zZz, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", action::action1, Menu::anyEvent)
  ,OP("Op2", action::action2, Menu::enterEvent)
  ,FIELD(test, "Test", "%", 0, 100, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EDIT("Name", name, alphaNumMask, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EDIT("Hex",  buf1, hexNr,        action::noOp, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(birthDate)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav/Run directly.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, MAX_DEPTH, &devOut);

// ── idle system: AM4's nav.timeOut/idleTask/idleOn(alert), adapted onto
// RunLoop via am4compat::IdleTimeout + NAVROOT_IDLE ────────────────────────
bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;
// 50ms inactivity window — short on purpose for a fast, deterministic
// selftest (AM4's own nav.timeOut=3 seconds, same "shortened for a fast
// test" reasoning as fullIdle.ino's own Idle window).
using Idle = am4compat::IdleTimeout<50, Run>;

NAVROOT_IDLE(nav, mainMenu, MAX_DEPTH, in, out, Run);

int idleStartCount = 0, idleEndCount = 0, idleTicks = 0;
bool idleRun() {  // mirrors AM4's idle() (nav.idleTask, auto-timeout path)
  if(idleTicks==0) { idleStartCount++; printf("suspending menu!\n"); }
  idleTicks++;
  printf(".");
  if(idleTicks>=3) {
    printf("\n");
    idleEndCount++;
    printf("resuming menu.\n");
    // AM4's idle() also calls nav.reset() here — no OneMenu equivalent
    // exists (see file header comment) — dropped, not faked.
    idleTicks=0;
    Run::idleOff();
  }
  return true;
}

int alertStartCount = 0, alertEndCount = 0, alertTicks = 0;
bool alertRun() {  // mirrors AM4's alert() (manually triggered via doAlert)
  if(alertTicks==0) {
    alertStartCount++;
    printf("alert test\npress [select]\nto continue...\n");
  }
  alertTicks++;
  // AM4's real alert() waits for a real keypress ("press [select] to
  // continue"); deterministic stand-in, same reasoning as idleRun() above.
  if(alertTicks>=3) {
    alertEndCount++;
    alertTicks=0;
    Run::idleOff();
  }
  return true;
}

bool mainRun() {
  bool activity = nav.poll();
  Idle::tick(activity, idleRun);
  return true;
}

bool doAlert(oneMenu::EventMask e, oneMenu::IItem&) {
  if(e & oneMenu::EventMask::Enter) nav.idleOn(alertRun);
  return true;
}

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=Op1,1=Op2,2=Test,3=subMenu,4=setLed,5=LEDOn,6=LEDOff,7=selMenu,
  // 8=chooseMenu,9=Alert,10=Name,11=Hex,12=birthDate,13=<Back>
  // (Cmd::Up increments the selected index, Cmd::Down decrements.)

  // ── Op1/Op2 (OP()'s 1-arg-trim and real-3-arg branches) ─────────────────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = Op1
  assert(action::op1Count == 1 && "action1's 2-arg trim did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (Op2)
  nav.enter();
  assert(action::op2Count == 1 && "action2's real 3-arg shape did not fire on Enter");

  // ── subMenu: title-level titleEvent + OP()'s real 3-arg showEvent ───────
  nav.up(); nav.up();  // 1 -> 2(Test) -> 3(subMenu)
  assert(action::focusCount == 1 && "subMenu's titleEvent did not observe Focus on landing");
  nav.enter();
  assert(nav.level() == 1 && action::enterCount == 1 &&
         "subMenu's titleEvent did not observe Enter on open");
  nav.enter();  // Sub1 (index 0 inside subMenu)
  assert(action::enterCount == 2 && "Sub1's real 3-arg showEvent did not observe Enter");
  nav.esc();
  assert(nav.level() == 0 && action::exitCount == 1 &&
         "subMenu's titleEvent did not observe Exit on close");

  // ── TOGGLE ────────────────────────────────────────────────────────────
  assert(ledCtrl == -1);
  nav.up();     // subMenu(3) -> setLed(4)
  nav.enter();  // cycles away from index 0/"On" (SyncValue's own m_sel-starts-at-0 semantics)
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to and sync the next VALUE()");

  // ── LED On/Off (0-arg AM4 handlers ported as bool(int)) ─────────────────
  assert(action::ledOnCount == 0);
  nav.up();     // setLed(4) -> LEDOn(5)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up();     // LEDOn(5) -> LEDOff(6)
  nav.enter();
  assert(action::ledOffCount == 1);

  // ── SELECT: Enter opens an inline picker (real level+1, rendered inline);
  // Up/Down move the highlighted option (same Up-increments/Down-decrements
  // convention as top-level nav); a second Enter commits path.sel() into
  // SelectBehave's own m_sel, which SyncValue then writes into the bound var.
  nav.up();     // LEDOff(6) -> selMenu(7)
  assert(selTest == 0);
  nav.enter();  // opens the inline picker
  assert(nav.level() == 1 && nav.sel() == 0 && "SELECT() did not open at its own m_sel (0)");
  nav.up();     // highlight moves to index 1 ("One")
  assert(nav.sel() == 1);
  nav.enter();  // commits
  assert(nav.level() == 0 && selTest == 1 &&
         "SELECT()'s second Enter did not commit the highlighted VALUE() into the bound var");

  // ── CHOOSE: Enter opens a real nested level; entering an option commits
  // it and closes back out (RecallNavPos, no inline-vs-nested distinction). ─
  nav.up();     // selMenu(7) -> chooseMenu(8)
  assert(chooseTest == -1);
  nav.enter();
  assert(nav.level() == 1 && "CHOOSE() did not open a nested level");
  nav.up();     // highlight moves to index 1 ("Second", val=2)
  nav.enter();  // commits
  assert(nav.level() == 0 && chooseTest == 2 &&
         "CHOOSE()'s commit did not write the highlighted VALUE() into the bound var and close");

  // ── Alert test: doAlert's Enter triggers nav.idleOn(alertRun); alertRun
  // terminates deterministically and calls Run::idleOff() itself ──────────
  nav.up();     // chooseMenu(8) -> Alert test(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  // ── EDIT(): Name (single shared validator, cycled pos%N) + Hex (4 distinct
  // per-position validators) — zero-copy buffer editing, same pattern
  // examples/am4compat already covers for EDIT() in general; here confirming
  // the single-entry-validator cycling case specifically (TextField.ino's
  // own shape). ─────────────────────────────────────────────────────────
  nav.up();     // Alert test(9) -> Name(10)
  assert(strcmp(name, "                    ") == 0);
  nav.enter();  // opens edit mode on Name
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.down();   // cycle position 0's char (TextField's Down=cycle-up convention)
  assert(name[0] != ' ' && "EDIT()'s single-entry validator did not cycle position 0");
  assert(name[1] == ' ' && "EDIT() must only change the position actually cycled");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  nav.up();     // Name(10) -> Hex(11)
  assert(strcmp(buf1, "0x11") == 0);
  nav.enter();  // opens edit mode on Hex
  nav.down();   // cycle position 0 (validator "0" — single char, wraps back to itself)
  assert(buf1[0] == '0' && "Hex's fixed '0' position must stay '0' (single-char validator)");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  // ── PADMENU: birthDate opens a real nested level holding 3 independent
  // FIELD()s (year/month/day) ──────────────────────────────────────────
  nav.up();     // Hex(11) -> birthDate(12)
  assert(year == 2017 && month == 10 && day == 7);
  nav.enter();
  assert(nav.level() == 1 && "PADMENU() did not open a nested level");
  nav.enter();  // year field (index 0) enters edit mode
  // NumField inverts Up/Down while in edit mode (Cmd::Up -> Base::down()) —
  // same documented quirk examples/am4compat's own digitMenu test hit — and
  // FIELD()'s step/tune are accepted but ignored (am4.h's own doc comment:
  // "value always steps by 1"), so Down here increments year by exactly 1.
  nav.down();
  assert(year == 2018 && "birthDate's year FIELD did not edit independently");
  nav.enter();  // leave edit mode
  nav.esc();
  assert(nav.level() == 0 && "esc() did not close back out of birthDate");

  // ── Idle system: IdleTimeout auto-triggers idleRun after inactivity,
  // same assertion style as examples/fullIdle's own selftest ──────────────
  assert(idleStartCount == 0);
  int guard=0;
  while(!Run::active() && guard++<200) { Run::run(); hw::delay_ms(1); }
  assert(Run::active() && "IdleTimeout did not auto-trigger idleOn after the timeout window");
  while(Run::active()) Run::run();
  assert(idleStartCount == 1 && idleEndCount == 1 &&
         "idleRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: serialio.ino compat-macro port — TOGGLE/SELECT/CHOOSE/subMenu/EDIT/PADMENU/idle verified\n");
  return 0;
}
