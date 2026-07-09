/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 handlers.ino compat-macro port — the "events" showcase. See the
 *        original at github.com/neu-rah/ArduinoMenu examples/handlers/handlers/
 *        handlers.ino: a single showEvent() handler observing every raised
 *        event across a submenu, a plain op, and a field.
 *
 * Moved here from OneMenu's own examples/handlers, 2026-07-09 — same reason
 * as examples/am4compat's own move: this is the AM5 "drop-in package" repo
 * real AM4-migrant sketches actually depend on. Now goes through this
 * branch's own <menu.h> forward (src/menu.h), not OneMenu's internal
 * oneMenu/compat/am4.h directly — the header a real sketch would include.
 *
 * Deliberately trimmed vs. the original — real AM4 features it also
 * demonstrates that this compat layer doesn't support yet, dropped rather
 * than faked:
 *  - altOP's custom prompt subclass (virtual printTo() override) — a
 *    different paradigm than OneMenu's compile-time components.
 *  - EDIT()'s masked/pattern text editing (name/hex fields) — not ported.
 *  - the idle system (nav.idleOn/idleTask) — not built yet (see notes.md
 *    "idle can be a Nav component...").
 *  - altFIELD's custom decimal-places field type — plain FIELD() covers the
 *    event wiring already; nothing new to show by porting the custom type.
 *
 * The LED TOGGLE *is* ported (2026-07-09, once TOGGLE()'s fn/mask got the
 * same auto-dispatch OP() has — see am4.h) — real coverage for that macro,
 * reusing showEvent() below unmodified (already bool(EventMask)-shaped).
 *
 * subMenu below now goes through the MENU() macro directly (2026-07-09, once
 * MENU()'s own fn/mask got the same auto-dispatch — am4compat::menuDefStyle,
 * am4.h) — real coverage for MENU()'s title-level events too.
 *
 * The selftest drives nav.up()/down()/enter()/esc() directly (same style as
 * examples/am4compat's own selftest; the real input-driven poll()/in() path
 * is already covered by that example's own regression checks, not repeated
 * here).
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
  int enterCount = 0, exitCount = 0, focusCount = 0, blurCount = 0;

  const char* eventName(oneMenu::EventMask e) {
    switch(e) {
      case oneMenu::EventMask::Enter: return "enterEvent";
      case oneMenu::EventMask::Exit:  return "exitEvent";
      case oneMenu::EventMask::Focus: return "focusEvent";
      case oneMenu::EventMask::Blur:  return "blurEvent";
      default: return "?";
    }
  }
  // AM4's real showEvent(eventMask,navNode&,prompt&) — trimmed to the
  // bool(EventMask) shape EventAction supports (no nav/path context yet;
  // see notes.md "AM4 compat layer" open question on a nav-context variant).
  bool showEvent(oneMenu::EventMask e) {
    if(e & oneMenu::EventMask::Enter) enterCount++;
    if(e & oneMenu::EventMask::Exit)  exitCount++;
    if(e & oneMenu::EventMask::Focus) focusCount++;
    if(e & oneMenu::EventMask::Blur)  blurCount++;
    printf("event: %s\n", eventName(e));
    return true;
  }
  bool op1(int) { printf("Op1 executed\n"); return true; }

  // FIELD()'s fn slot wires EventCall (void(), opaque to *which* event fired
  // — see am4.h's FIELD() doc comment), a different, narrower handler shape
  // than showEvent's. Masked to enterEvent only (not anyEvent, unlike the
  // AM4 original) so Focus/Blur passing over this field don't also count as
  // a "field updated" — a deliberate deviation, not a limitation of the fix.
  int fieldEvents = 0;
  void onFieldEvent() { fieldEvents++; printf("event: field updated\n"); }
}

float test = 55;

// subMenu's own Enter/Exit/Focus/Blur now wire through MENU()'s fn/mask
// (am4compat::menuDefStyle splices EventAction<mask,fn> into Menu<T,B,MM...>'s
// own MM... pack — the only chain-reachable slot, since the title itself is a
// plain data member HAPI traversal never reaches). Sub1 stays hand-composed
// (not OP()) specifically to keep demonstrating two independent layers on the
// same item: Action<op1> (the plain execute-on-Enter primitive, op1 is
// bool(int) so OP()'s own auto-dispatch would give it the cheap path anyway)
// and EventAction<Any,showEvent> (real event dispatch) firing side by side —
// not two APIs for the same thing.
MENU(subMenu, "Sub-Menu", action::showEvent, Menu::anyEvent, Menu::noStyle
  // wrapped in parens: MENU()'s __VA_ARGS__ goes through the preprocessor
  // first, which splits on every top-level comma — including ones inside
  // <...> template-argument lists, since the preprocessor doesn't parse C++
  // template syntax. Parens make the whole expression one macro argument.
  ,(oneMenu::ItemDef<oneMenu::Action<action::op1>,
                      oneMenu::EventAction<oneMenu::EventMask::Any, action::showEvent>,
                      Text>{"Sub1"})
  ,EXIT("<Back")
);

int ledCtrl = -1;  // -1: neither VALUE() below — proves SyncValue actually wrote something.
TOGGLE(ledCtrl, setLed, "Led: ", action::showEvent, Menu::anyEvent, Menu::noStyle
  ,VALUE("On", 1, Menu::doNothing, Menu::noEvent)
  ,VALUE("Off", 0, Menu::doNothing, Menu::noEvent)
);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(test, "Test", "%", 0, 100, 1, 0, action::onFieldEvent, Menu::enterEvent, Menu::noStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
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
NAVROOT(nav, mainMenu, /*maxDepth*/2, in, out);

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=Test(field), 1=Sub-Menu, 2=<Back>  (Cmd::Up increments, Down decrements)
  assert(action::fieldEvents == 0);
  nav.enter();  // enters edit mode on the Test field -> EventCall fires (Enter)
  assert(action::fieldEvents == 1 && "FIELD()'s EventCall did not fire entering edit mode");
  nav.enter();  // leaves edit mode -> EventCall fires again (documented FIELD() deviation)
  assert(action::fieldEvents == 2 && "FIELD()'s EventCall did not fire leaving edit mode");

  assert(action::focusCount == 0 && action::blurCount == 0);
  nav.up();     // 0 -> 1 (Test -> Sub-Menu): Blur has no handler on Test, Focus does on Sub-Menu
  assert(action::focusCount == 1 && action::blurCount == 0 &&
         "Sub-Menu's own EventAction did not observe Focus");

  assert(action::enterCount == 0);
  nav.enter();  // opens Sub-Menu (level 0 -> 1) -> Enter fires on the Sub-Menu item itself
  assert(nav.level() == 1 && action::enterCount == 1 &&
         "Sub-Menu's own EventAction did not observe Enter on open");
  nav.enter();  // Sub1 (index 0 inside Sub-Menu) -> its own EventAction + Action<op1> both fire
  assert(action::enterCount == 2 && "Sub1's EventAction did not observe Enter");

  assert(action::exitCount == 0);
  nav.esc();    // closes Sub-Menu (level 1 -> 0) -> Exit fires on the Sub-Menu item
  assert(nav.level() == 0 && action::exitCount == 1 &&
         "Sub-Menu's own EventAction did not observe Exit on close");

  // ── Regression: TOGGLE()'s auto-dispatch (am4.h, 2026-07-09) — bool(EventMask)
  // fn gets a real EventAction<mask,fn> spliced onto the TOGGLE item itself,
  // same mechanism as OP()'s (notes.md "list next targets" -> "#1"). Delta-based
  // (not absolute counts) so this doesn't need to track every prior assertion's
  // exact running total — sel is at index 1 (Sub-Menu) here, esc() doesn't move it.
  int enterBefore = action::enterCount;
  assert(ledCtrl == -1);
  nav.up();     // Sub-Menu(1) -> setLed(2)
  nav.enter();  // cycles the toggle (away from index 0/"On", per SyncValue's
                // documented m_sel-starts-at-0 semantics) and fires Enter on setLed
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to and sync the next VALUE()");
  assert(action::enterCount == enterBefore + 1 &&
         "TOGGLE()'s EventAction did not fire Enter");

  printf("OK: handlers.ino compat-macro port (showEvent/FIELD-EventCall/TOGGLE) verified\n");
  return 0;
}
