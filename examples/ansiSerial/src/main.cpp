/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 ansiSerial.ino compat-macro port — "menu to ANSI serial
 *        terminal": the color/ANSI-art showcase, structurally
 *        `serialio.ino`'s own shape (TOGGLE/SELECT/CHOOSE/PADMENU/EDIT/idle)
 *        plus one extra nesting level (`subSubMenu`/`bigMenu`, a depth-limit
 *        test) and two color/character demo screens. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/ansiSerial/ansiSerial/
 *        ansiSerial.ino.
 *
 * Pure assembly for the menu tree itself — every macro/mechanism it needs
 * already existed before this port (TOGGLE/SELECT/CHOOSE/VALUE, EDIT, FIELD,
 * PADMENU, MENU, OP's 1/2/3-arg auto-dispatch, NAVROOT_IDLE,
 * am4compat::IdleTimeout — see `serialio.ino`'s own port for the shared
 * precedents). The one piece that *did* need new compat-layer machinery —
 * a real-hardware ANSI-color-over-Serial output backend — is `am4.h`'s new
 * `ANSISERIAL_OUT(port,colors,panels...)` macro (2026-07-10, see notes.md
 * "AM4 compat layer"): `ansiSerialOut.h` is Rui's own AM4 driver (unlike the
 * third-party-contributed `menuIO/*.h` drivers this compat layer otherwise
 * bridges as-is), so it's reimplemented natively (`ANSIFmt`+`ANSIOut` over
 * the real `SerialOut` sink) rather than bridged — no licensing reason to
 * leave it untouched. The real hardware wiring (`ANSISERIAL_OUT` + a
 * translated `Color<int>::Table<...>` palette) lives only in the AVR-only
 * harness (`.RnD/AM4check/ansiSerial`), same split as every other
 * bridge/backend-touching port this session — this file's own selftest uses
 * `ANSI_OUT` (native/console, already proven).
 *
 * Deliberate deviations from the original, beyond the usual #include swap:
 *  - `action1`/`action2` are genuinely 3-arg in the real source (unlike
 *    `serialio.ino`'s own `action1`, which was 1-arg there) — both map
 *    directly onto the real `EventActionItemNav` branch, no trim needed.
 *    `action2` returns AM4's `quit` (request the nav actively close) —
 *    OneMenu's 3-arg handler has no equivalent "ask nav to exit" signal
 *    (confirmed: no such mechanism exists anywhere in am4.h/nav.h); ported
 *    returning `true` like every other handler, same "known semantic gap,
 *    document don't fake" precedent as `SUBMENU`'s move-only limitation.
 *  - `showEvent` (real 3-arg) keeps its real shape everywhere it's used as
 *    `OP()`'s fn (`subMenu`'s own `Sub1`/`Sub2`/`Sub3`); both `subMenu` AND
 *    `subSubMenu`'s own *title*-level fn slots reuse it in the real source,
 *    but `MENU()`'s title fn/mask only recognizes `bool(EventMask)`
 *    (`am4compat::menuDefStyle`) — same situation `serialio.ino`'s own
 *    `subMenu` already hit; fixed the same way, a shared 1-arg
 *    `titleEvent()` wrapper used in both title slots.
 *  - `myLedOn`/`myLedOff` (0-arg) ported as `bool(int)`, same established
 *    adapter. `subSubMenu`'s own `SSub1`/`SSub2` and every `bigMenu` "Op"
 *    item use `doNothing`/`noEvent` directly — no adapter needed.
 *  - `altOP(altPrompt,"custom",showEvent,anyEvent)` inside `subMenu` dropped
 *    — custom-prompt subclass, same precedent as every other `altOP` drop.
 *  - `birthDate`'s `altMENU(...,(systemStyles)(_asPad|...))` maps onto
 *    `PADMENU()` directly — pad-ness is already `PADMENU`'s own dedicated
 *    macro here, not a style flag; the extra AM4-internal rendering flags
 *    (`_menuData`/`_canNav`/`_parentDraw`) have no OneMenu equivalent to
 *    thread through and aren't needed (`PADMENU` already renders as a pad).
 *  - `EDIT("Name",...)` from `serialio.ino` isn't in this example — only
 *    `EDIT("Hex",...)` is (single distinct-per-position validator, same
 *    shape `serialio`'s own Hex field already proved).
 *  - **Show colors / Show characters demo screens, simplified.** AM4's
 *    `showColorDef`/`putColor`/`showChars` call `out.fill()`/`out.setColor()`
 *    directly from user code to hand-draw color swatches and box-drawing
 *    characters. Rui's own correction (2026-07-10): this isn't a missing
 *    *public* OutDef API to expose, it's a small *format-chain component* to
 *    write (same `Base::`-qualified-access pattern `ANSIOut`/`ANSIFmt`
 *    themselves already use) — not a blocker, but genuinely more work than
 *    this port's core lesson (ANSI-color menu navigation) needs. Ported as
 *    plain idle-triggered `printf()` screens instead (same "deterministic,
 *    simplified stand-in" precedent `alertRun`/`idleRun` already use
 *    throughout this compat layer) — a real, disclosed simplification, not
 *    a faithful pixel-for-pixel recreation of the original's ANSI art. A
 *    dedicated format-chain component for this is a reasonable follow-up if
 *    the faithful version is ever wanted.
 *  - `showCharsMap`/`showOutColors` (real AM4 signature:
 *    `void(eventMask,navNode&)`, calling `nav.root->idleOn(fn)` directly) —
 *    no OP() branch matches `void(EventMask,INav&)`; ported as `bool(
 *    EventMask,IItem&)` wrappers calling the already-in-scope `nav.idleOn(
 *    ...)` directly (`nav` is a file-scope global here, same forward-
 *    declaration trick `doAlert` below and `fullIdle`/`navHandler`'s own
 *    `mainFn`/`idleFn` already use).
 *  - The idle system (`nav.idleOn(alert)` from `doAlert`) is adapted onto
 *    `RunLoop`/`NAVROOT_IDLE`, same shape `serialio.ino`'s own idle system
 *    already uses; no `nav.timeOut`/auto-idle is wired in this original
 *    (manual-only, like `codeCtrl`/`btnNav`).
 *  - LEDPIN/pinMode/digitalWrite (real Arduino GPIO calls) dropped — no
 *    real hardware in this native-only selftest, same convention as every
 *    other port in this directory.
 *
 * The selftest drives nav.up()/down()/enter()/esc() and Run::run() directly,
 * same verification style as every other port in this compat layer.
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
  // ── action1/action2 — real 3-arg AM4 shape, no trim needed ──────────────
  int op1Count = 0;
  bool action1(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { op1Count++; return true; }

  int op2Count = 0;
  bool action2(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) {
    op2Count++;
    return true;  // real AM4 returns `quit` here — no port-side equivalent, see file header comment.
  }

  // ── showEvent (real 3-arg, used by subMenu's Sub1/Sub2/Sub3) + titleEvent
  // (1-arg trim, used by both subMenu's and subSubMenu's own title slots) —
  // share one set of counters so either path is observable the same way.
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

  void noOp() {}
}

int test = 55;

int ledCtrl = -1;
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

// ── depth-limit test: subMenu -> subSubMenu -> bigMenu (15 plain items) ──
MENU(bigMenu, "BigMenu", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
  ,OP("Op", Menu::doNothing, Menu::noEvent)
);

MENU(subSubMenu, "Sub-Sub-Menu", action::titleEvent, Menu::anyEvent, Menu::noStyle
  ,OP("SSub1", Menu::doNothing, Menu::noEvent)
  ,OP("SSub2", Menu::doNothing, Menu::noEvent)
  ,SUBMENU(bigMenu)
  ,EXIT("<Back")
);

// altOP(altPrompt,"custom",showEvent,anyEvent) dropped — custom-prompt
// subclass, see file header comment.
MENU(subMenu, "Sub-Menu", action::titleEvent, Menu::anyEvent, Menu::noStyle
  ,OP("Sub1", action::showEvent, Menu::anyEvent)
  ,OP("Sub2", action::showEvent, Menu::anyEvent)
  ,OP("Sub3", action::showEvent, Menu::anyEvent)
  ,SUBMENU(subSubMenu)
  ,EXIT("<Back")
);

// ── PADMENU date-entry group ────────────────────────────────────────────
uint16_t year = 2017, month = 10, day = 7;
PADMENU(birthDate, "Birth", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,FIELD(year,  "", "/", 1900, 3000, 20, 1, action::noOp, Menu::noEvent, Menu::noStyle)
  ,FIELD(month, "", "/", 1,    12,   1,  0, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,FIELD(day,   "", "",  1,    31,   1,  0, action::noOp, Menu::noEvent, Menu::wrapStyle)
);

// ── EDIT() masked hex field ──────────────────────────────────────────────
static const char* hexDigit = "0123456789ABCDEF";
static const char* hexNr[] = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

// forward-declared: bodies need `nav` (declared further below via
// NAVROOT_IDLE), same trick fullIdle.ino/navHandler already use.
bool doAlert(oneMenu::EventMask, oneMenu::IItem&);
bool showOutColors(oneMenu::EventMask, oneMenu::IItem&);
bool showCharsMap(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1", action::action1, Menu::anyEvent)
  ,OP("Op2", action::action2, Menu::enterEvent)
  ,FIELD(test, "Test", "%", 0, 100, 10, 1, action::noOp, Menu::noEvent, Menu::wrapStyle)
  ,SUBMENU(birthDate)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",  action::myLedOn,  Menu::enterEvent)
  ,OP("LED Off", action::myLedOff, Menu::enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Show colors", showOutColors, Menu::enterEvent)
  ,OP("Show characters", showCharsMap, Menu::enterEvent)
  ,OP("Alert test", doAlert, Menu::enterEvent)
  ,EDIT("Hex", buf1, hexNr, action::noOp, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

#define MAX_DEPTH 4

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

// manual-only idle (no nav.timeOut in the original).
bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;

NAVROOT_IDLE(nav, mainMenu, MAX_DEPTH, in, out, Run);

// ── Show colors / Show characters — simplified idle screens, see file
// header comment for why these aren't a faithful out.fill()/setColor()
// recreation. ──────────────────────────────────────────────────────────
int showColorsCount = 0, showCharsCount = 0;
bool showColorsRun() {
  showColorsCount++;
  printf("bg fg val unit cursor title (palette demo)\n");
  Run::idleOff();
  return true;
}
bool showCharsRun() {
  showCharsCount++;
  printf("box chars demo\n");
  Run::idleOff();
  return true;
}

bool showOutColors(oneMenu::EventMask e, oneMenu::IItem&) {
  if(e & oneMenu::EventMask::Enter) nav.idleOn(showColorsRun);
  return true;
}
bool showCharsMap(oneMenu::EventMask e, oneMenu::IItem&) {
  if(e & oneMenu::EventMask::Enter) nav.idleOn(showCharsRun);
  return true;
}

int alertStartCount = 0, alertEndCount = 0, alertTicks = 0;
bool alertRun() {
  if(alertTicks==0) {
    alertStartCount++;
    printf("alert test\npress [select]\nto continue...\n");
  }
  alertTicks++;
  if(alertTicks>=3) {
    alertEndCount++;
    alertTicks=0;
    Run::idleOff();
  }
  return true;
}

bool mainRun() { return nav.poll(); }

bool doAlert(oneMenu::EventMask e, oneMenu::IItem&) {
  if(e & oneMenu::EventMask::Enter) nav.idleOn(alertRun);
  return true;
}

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=Op1,1=Op2,2=Test,3=birthDate,4=subMenu,5=setLed,6=LEDOn,
  // 7=LEDOff,8=selMenu,9=chooseMenu,10=ShowColors,11=ShowChars,12=Alert,
  // 13=Hex,14=<Back>

  // ── Op1/Op2 (both real 3-arg) ─────────────────────────────────────────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = Op1
  assert(action::op1Count == 1 && "action1's real 3-arg shape did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (Op2)
  nav.enter();
  assert(action::op2Count == 1 && "action2's real 3-arg shape did not fire on Enter");

  // ── PADMENU: birthDate opens a real nested level holding 3 independent
  // FIELD()s (year/month/day) ──────────────────────────────────────────
  nav.up(); nav.up();  // Op2(1) -> Test(2) -> birthDate(3)
  assert(year == 2017 && month == 10 && day == 7);
  nav.enter();
  assert(nav.level() == 1 && "PADMENU() did not open a nested level");
  nav.enter();  // year field (index 0) enters edit mode
  nav.down();   // NumField inverts Up/Down in edit mode; step is always 1.
  assert(year == 2018 && "birthDate's year FIELD did not edit independently");
  nav.enter();  // leave edit mode
  nav.esc();
  assert(nav.level() == 0 && "esc() did not close back out of birthDate");

  // ── subMenu -> subSubMenu -> bigMenu depth-limit test ────────────────
  // Real finding, confirmed via an isolated minimal repro: entering an OP()
  // item whose mask is Menu::anyEvent (not Menu::enterEvent) auto-closes
  // the enclosing submenu level back to its parent as part of ordinary
  // navigation, in addition to firing its handler — invisible at the menu
  // root (nothing to close down to there), but real one level deep. This
  // is also true of serialio.ino's own already-shipped Sub1/2/3 (same
  // anyEvent OP() shape) — its own selftest just never asserted the level
  // stayed open after entering Sub1, so it never surfaced. Navigating PAST
  // Sub1/Sub2/Sub3 (via up(), never entering them) to reach subSubMenu
  // avoids the auto-close; Sub1's own firing+auto-close is verified
  // separately below instead.
  nav.up();     // birthDate(3) -> subMenu(4)
  assert(action::focusCount == 1 && "subMenu's titleEvent did not observe Focus on landing");
  nav.enter();
  assert(nav.level() == 1 && action::enterCount == 1 &&
         "subMenu's titleEvent did not observe Enter on open");
  nav.up(); nav.up(); nav.up();  // Sub1(0) -> Sub2(1) -> Sub3(2) -> subSubMenu(3), no entering
  nav.enter();
  assert(nav.level() == 2 && "subSubMenu did not open at the 2nd nesting level");
  nav.up(); nav.up();  // SSub1 -> SSub2 -> bigMenu
  nav.enter();
  assert(nav.level() == 3 && "bigMenu did not open at the 3rd nesting level");
  nav.esc();
  assert(nav.level() == 2);
  nav.esc();
  assert(nav.level() == 1 && "esc() did not close subSubMenu back to subMenu");
  nav.esc();
  assert(nav.level() == 0 && "esc() did not close subMenu back to mainMenu");

  // ── Sub1's real 3-arg showEvent: fires, then auto-closes (see finding
  // above) — sel is back on subMenu's own position (4) after the esc chain.
  // Asserting the *delta*, not an absolute count: the depth traversal above
  // already moved enterCount/exitCount/focusCount/blurCount off zero. ─────
  int enterCountBefore = action::enterCount;
  nav.enter();  // reopen subMenu
  assert(nav.level() == 1 && nav.sel() == 0);
  nav.enter();  // Sub1 (index 0)
  assert(action::enterCount == enterCountBefore + 2 &&
         "subMenu's titleEvent Enter + Sub1's real 3-arg showEvent Enter did not both fire");
  assert(nav.level() == 0 && "OP() with anyEvent did not auto-close back to mainMenu");

  // ── TOGGLE ────────────────────────────────────────────────────────────
  assert(ledCtrl == -1);
  nav.up();     // subMenu(4) -> setLed(5)
  nav.enter();
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to and sync the next VALUE()");

  // ── LED On/Off ────────────────────────────────────────────────────────
  assert(action::ledOnCount == 0);
  nav.up();     // setLed(5) -> LEDOn(6)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up();     // LEDOn(6) -> LEDOff(7)
  nav.enter();
  assert(action::ledOffCount == 1);

  // ── SELECT / CHOOSE ───────────────────────────────────────────────────
  nav.up();     // LEDOff(7) -> selMenu(8)
  nav.enter();
  nav.up();
  nav.enter();
  assert(selTest == 1 && "SELECT() did not commit the highlighted VALUE()");

  nav.up();     // selMenu(8) -> chooseMenu(9)
  nav.enter();
  nav.up();
  nav.enter();
  assert(chooseTest == 2 && "CHOOSE() did not commit the highlighted VALUE()");

  // ── Show colors / Show characters — nav.idleOn() only *arms* the idle
  // system; the handler itself only runs on a later Run::run() tick, same
  // "pump until done" pattern the Alert test below already uses. ─────────
  nav.up();     // chooseMenu(9) -> Show colors(10)
  assert(showColorsCount == 0);
  nav.enter();
  assert(Run::active() && "showOutColors did not call nav.idleOn(showColorsRun)");
  while(Run::active()) Run::run();
  assert(showColorsCount == 1 && "showOutColors did not trigger the idle screen");

  nav.up();     // Show colors(10) -> Show characters(11)
  assert(showCharsCount == 0);
  nav.enter();
  assert(Run::active() && "showCharsMap did not call nav.idleOn(showCharsRun)");
  while(Run::active()) Run::run();
  assert(showCharsCount == 1 && "showCharsMap did not trigger the idle screen");

  // ── Alert test: manual-only idle ─────────────────────────────────────
  nav.up();     // Show characters(11) -> Alert test(12)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  // ── EDIT(): Hex (4 distinct per-position validators) ─────────────────
  nav.up();     // Alert test(12) -> Hex(13)
  assert(strcmp(buf1, "0x11") == 0);
  nav.enter();
  nav.down();   // cycle position 0 (validator "0" — single char, wraps to itself)
  assert(buf1[0] == '0' && "Hex's fixed '0' position must stay '0' (single-char validator)");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  printf("OK: ansiSerial.ino compat-macro port — TOGGLE/SELECT/CHOOSE/PADMENU/"
         "subMenu-subSubMenu-bigMenu depth/EDIT/idle/color-screens verified\n");
  return 0;
}
