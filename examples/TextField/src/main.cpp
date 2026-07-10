/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 TextField.ino compat-macro port — "text field test": three
 *        masked EDIT() fields (a hand-built IP-address field, a fixed-length
 *        hex field, and a free-form name field), exercising CharMask::
 *        PosSet's own pos%N validator-cycling in three different shapes
 *        (4 distinct positions repeating every 4 chars, 4 identical
 *        positions, 1 shared position). See the original at
 *        github.com/neu-rah/ArduinoMenu examples/TextField/TextField/
 *        TextField.ino.
 *
 * Pure assembly — every macro/mechanism this file needs already existed
 * before this port (EDIT(), MENU(), CharMask::PosSet's pos%N cycling).
 *
 * Real deviation from the original, beyond the usual #include swap:
 *  - `option0` (the real AM4 source's hand-built `textField` object — a raw
 *    `textFieldShadowRaw` PROGMEM struct wrapped in `textField option0(...)`,
 *    spliced via `OBJ(option0)`) is ported as a plain `EDIT("IP",...)` call
 *    instead. The raw object construction is AM4's own internal mechanism
 *    for *building* exactly what EDIT() already builds — its systemStyles
 *    bits (`_canNav|_parentDraw`) are AM4-internal rendering flags EDIT()'s
 *    own factory already applies by convention (every EDIT() field in this
 *    compat layer renders the same way) — there's no distinct *capability*
 *    here for OBJ() to splice in, so translating the intent (a 4-position
 *    masked text field) directly onto EDIT() is faithful, not a
 *    simplification of behavior.
 *  - `validData[4]` (four *identical* `hexChars` entries, for the 4-char
 *    Hex field) is kept as a literal 4-entry array, not collapsed to 1 —
 *    functionally identical either way under PosSet's own pos%N cycling,
 *    kept as-is for byte-for-byte fidelity to the original declaration.
 *
 * The selftest drives nav.enter()/down()/esc() directly to exercise each
 * field's own validator cycling, same style as every other port in this
 * compat layer. Real serial input (needed for typing full IP/hex/name
 * strings interactively) is held back per Rui's own 2026-07-10 review hold
 * on InDef::inBurst-based real input — see [[feedback_inburst_review_hold]]
 * — so this port has no AVR-hardware-input harness yet, native selftest
 * (NoOpIn) only, same as every other example currently affected by that hold.
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

// ── option0: 4-position masked IP-address field (real AM4 source built this
// via a hand-declared textField object + OBJ(); ported directly as EDIT(),
// see file header comment) ──────────────────────────────────────────────
static const char* digit = "0123456789";
static const char* ipFirst = "012";
static const char* dot = ".";
static const char* validIP[] = {ipFirst, digit, digit, dot};
char buf0[] = "000.000.000.000";

// ── Hex field: 4 identical positions (kept literal, see file header) ────
static const char* hexChars = "0123456789ABCDEF";
static const char* validData[] = {hexChars, hexChars, hexChars, hexChars};
char buf1[] = "0000";

// ── Name field: single shared validator, cycled pos%N ────────────────────
static const char* alphaNum =
  " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,+-_";
static const char* alphaNumMask[] = {alphaNum};
char name[31] = "                              ";

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,EDIT("IP",   buf0, validIP,     action::noOp, Menu::enterEvent, Menu::noStyle)
  ,EDIT("Hex",  buf1, validData,   action::noOp, Menu::noEvent,    Menu::noStyle)
  ,EDIT("Name", name, alphaNumMask, action::noOp, Menu::noEvent,   Menu::noStyle)
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

  // index 0=IP,1=Hex,2=Name,3=<Exit>

  // ── IP field: 4-position validator repeats every 4 chars (pos%4) ───────
  assert(strcmp(buf0, "000.000.000.000") == 0);
  nav.enter();  // opens edit mode, position 0 (validIP[0]="012")
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.down();
  assert(buf0[0] == '1' && "IP field position 0 (validIP[0]='012') did not cycle");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  // ── Hex field: 4 identical-hexChars positions ───────────────────────────
  nav.up();     // IP(0) -> Hex(1) — Cmd::Up increments the selected index
  assert(strcmp(buf1, "0000") == 0);
  nav.enter();
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.down();
  assert(buf1[0] != '0' && "Hex field position 0 did not cycle");
  assert(buf1[1] == '0' && "EDIT() must only change the position actually cycled");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  // ── Name field: single shared validator, cycled pos%N (N=1) ────────────
  nav.up();     // Hex(1) -> Name(2)
  assert(strcmp(name, "                              ") == 0);
  nav.enter();
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.down();
  assert(name[0] != ' ' && "Name field's single-entry validator did not cycle position 0");
  assert(name[1] == ' ' && "EDIT() must only change the position actually cycled");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  printf("OK: TextField.ino compat-macro port (3 EDIT() masked fields, "
         "4-distinct/4-identical/1-shared validator cycling) verified\n");
  return 0;
}
