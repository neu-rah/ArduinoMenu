/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 compat-macro proof: same MENU/FIELD/OP/EXIT/SUBMENU call syntax as
 *        AM4's own README example, expanded through this branch's own <menu.h>
 *        forward (see src/menu.h) — the same header a real AM4 sketch includes,
 *        not OneMenu's internal oneMenu/compat/am4.h directly.
 *        I/O + nav wiring uses the AM4-syntax device macros
 *        (ANSI_OUT/MENU_INPUTS/MENU_OUTPUTS/NAVROOT) instead of hand-wired
 *        InDef/OutDef/INavDef — `nav.poll()` below is AM4's own device-fanout
 *        call shape, and `devOut` is declared via `ANSI_OUT(id,w,h)`
 *        (am4.h's per-backend output macro, same spirit as AM4's own
 *        `SERIAL_OUT`/`ANSISERIAL_OUT`, adapted for OneMenu's ANSI/console
 *        backend — not byte-for-byte AM4 syntax since AM4 has no matching
 *        native-console backend to mirror; see am4.h's ANSI_OUT doc comment).
 *        `devIn` is still pre-declared the native OneMenu way — MENU_INPUTS
 *        just wants a pointer to an already-built device, same as AM4.
 *
 * Moved here from OneMenu's own examples/am4compat, 2026-07-09 — this is the
 * AM5 "drop-in package" repo real AM4-migrant sketches actually depend on,
 * so the compat-layer proof belongs here, not just in OneMenu's own tree.
 *
 * The selftest drives nav.up()/nav.enter() directly (no PCKbd/ANSI parsing
 * involved) — same verification style as OneMenu's own `fields` example.
 * `devIn` is a deterministic no-op input source (see NoOpIn below): real
 * interactive input isn't exercised here, only MENU_INPUTS'/NAVROOT's fanout.
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
#include <type_traits>

using namespace hapi;
using namespace oneData;
// NOTE: deliberately no `using namespace oneMenu;` here — oneMenu::Menu<T,B,OO...>
// (the menu class template) unavoidably collides with AM4's `Menu` namespace as
// soon as both are visible unqualified in the same TU, *even when Menu:: members
// are referenced fully qualified* (Menu::noStyle still needs unqualified lookup
// of "Menu" itself to resolve the `::`, and that's what's ambiguous). Verified
// empirically — see notes.md "AM4 compat layer". Fix: qualify the handful of
// native OneMenu identifiers below with oneMenu:: instead of a blanket using-directive.

// ── variables edited directly by FIELD() via DataRef (AM4's edit-by-reference) ──
unsigned int timeOn = 10;
unsigned int timeOff = 90;

namespace action {
  int op1Count = 0;
  // OP()'s fn is now EventActionItem-shaped (bool(EventMask,IItem&) — see
  // am4.h's OP() doc comment). Masked to enterEvent (not anyEvent, unlike
  // serialio.ino's own OP("Op1",action1,anyEvent) — that handler only prints,
  // this one counts, and anyEvent would also count every Focus/Blur passing
  // over the item as nav.up()/down() moves through it, before the real
  // nav.enter() below — enterEvent keeps the count meaning "activated once").
  bool op1(oneMenu::EventMask, oneMenu::IItem&) { op1Count++; return true; }
  // FIELD()'s fn must be a real, non-overloaded void() (EventCall, item.h) —
  // Menu::doNothing (bool(int)) doesn't fit that shape; see am4.h's
  // Menu::doNothing comment for why it's kept single-overload on purpose.
  void noField() {}

  int op2LegacyCount = 0;
  // bool(int) — the original v1 shape. OP()'s fn auto-dispatches on its own
  // signature (am4.h's am4compat::opItem): this handler isn't event-shaped,
  // so it falls back to the original zero-cost Action<fn> binding, not
  // IItemDef — proves "events optional" (Rui, 2026-07-09) end to end, not
  // just at the type level (see the static_asserts right below mainMenu).
  bool op2Legacy(int) { op2LegacyCount++; return true; }
}

// ── menu tree, verbatim AM4 call syntax ─────────────────────────────────────
MENU(subMenu, "Sub-Menu", Menu::doNothing, Menu::anyEvent, Menu::noStyle
  ,OP("Sub1", action::op1, Menu::enterEvent)
  ,EXIT("<Back")
);

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,  "On",  "ms", 0, 1000,  10, 1, action::noField, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff, "Off", "ms", 0, 10000, 10, 1, action::noField, Menu::noEvent, Menu::noStyle)
  ,OP("Op1", action::op1, Menu::enterEvent)
  ,SUBMENU(subMenu)
  ,OP("Op2Legacy", action::op2Legacy, Menu::noEvent)
  ,EXIT("<Back")
);

// Type-level proof that OP()'s auto-dispatch (am4.h) actually picks
// differently based on fn's signature, not just "always the cheap path" or
// "always the virtual one" by accident.
static_assert(!std::is_polymorphic_v<decltype(am4compat::opItem<oneMenu::EventMask::None, action::op2Legacy>("x"))>,
              "bool(int) OP() handler must stay the cheap, non-virtual Action<fn> binding");
static_assert(std::is_polymorphic_v<decltype(am4compat::opItem<oneMenu::EventMask::Enter, action::op1>("x"))>,
              "bool(EventMask,IItem&) OP() handler must get the real IItemDef binding");

// ── PADMENU() coverage: fn/mask auto-dispatch (am4compat::padDefStyle, am4.h,
// 2026-07-09) — same auto-dispatch principle as MENU()'s (menuDefStyle), but
// deliberately without any style/WrapNav involvement — PadDraw is a pure
// rendering tag, wrap is an orthogonal nav-boundary concern a pad has no use
// for (see padDefStyle's own doc comment). Standalone nav tree (same pattern
// as regMenu/regNav below), not spliced into mainMenu, so it doesn't disturb
// mainMenu's existing index-based selftest sequence above.
namespace padtest {
  int enterCount = 0, exitCount = 0;
  bool onEv(oneMenu::EventMask e) {
    if(e & oneMenu::EventMask::Enter) enterCount++;
    if(e & oneMenu::EventMask::Exit)  exitCount++;
    return true;
  }
}

PADMENU(padMenu, "Pad", padtest::onEv, Menu::anyEvent, Menu::noStyle
  ,EXIT("<Back")
);

MENU(padParent, "PadParent", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,SUBMENU(padMenu)
  ,EXIT("<Back")
);

oneMenu::INavDef<
  oneMenu::EventDispatch,
  oneMenu::TreeNav,
  oneMenu::Root<decltype(padParent), padParent>
> padNav;

// ── I/O + nav: AM4-syntax device wiring (ANSI_OUT/MENU_INPUTS/MENU_OUTPUTS/NAVROOT) ──
// devIn is still pre-declared the native OneMenu way (see file comment); devOut now
// comes from ANSI_OUT(id,w,h), am4.h's own per-backend device macro (AM4-syntax
// equivalent of SERIAL_OUT/ANSISERIAL_OUT for OneMenu's ANSI/console backend).
ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — this example's selftest drives nav
// directly (nav.up()/nav.enter()), so MENU_INPUTS just needs something that
// satisfies InDef's doInput() shape without ever producing a command.
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

// ── Library-level regression checks, 2026-07-09: two EventDispatch fixes (nav.h,
// notes.md "AM4 compat layer") — (1) Enter/Exit now gate on the target item's
// enabled() (Focus/Blur deliberately don't — a disabled item stays selectable), and
// (2) events fire via the real input-driven poll()/in() path, not just direct
// nav.up()/down()/enter()/esc() calls. Native OneMenu component syntax, not AM4
// macros — kept separate from the AM4-syntax proof above (same pattern as
// .RnD/fields' extra selftestInBurst()/selftestTextRoll() functions).
namespace regtest {
  int enterCount = 0, exitCount = 0, focusCount = 0, blurCount = 0;
  bool onEv(oneMenu::EventMask e) {
    if(e & oneMenu::EventMask::Enter) enterCount++;
    if(e & oneMenu::EventMask::Exit)  exitCount++;
    if(e & oneMenu::EventMask::Focus) focusCount++;
    if(e & oneMenu::EventMask::Blur)  blurCount++;
    return true;
  }
}

// item 0 starts disabled (Watch<EnDis<false>>, same idiom as examples/fields' op3);
// item 1 is a plain second item so Up/Down has somewhere to move the selection to —
// it also carries EventAction so a Focus landing on it is actually observable.
auto regMenu = oneMenu::menuDef<>(
  oneMenu::ItemDef<Text>{"Reg"},
  oneMenu::staticBody(
    oneMenu::ItemDef<oneMenu::Id<0>, Watch<oneMenu::EnDis<false>>,
                      oneMenu::EventAction<oneMenu::EventMask::Any, regtest::onEv>>{},
    oneMenu::ItemDef<oneMenu::EventAction<oneMenu::EventMask::Any, regtest::onEv>>{}
  )
);

// Scripted input source: replays a fixed queue of CKE commands, one per cmd() call.
// Driving it through InDef::inBurst() exercises the real Pool::poll() -> doInput() ->
// inBurst() -> nav.in(*this) path, proving EventDispatch is reached from real input
// dispatch and not just from direct nav.up()/down()/enter()/esc() calls.
struct ScriptedIn {
  template<typename In> struct Part : In {
    static inline oneMenu::CKE queue[4]{};
    static inline int head = 0, tail = 0;
    static bool available() { return head < tail; }
    static oneMenu::CKE cmd() { return head < tail ? queue[head++] : oneMenu::CKE{}; }
    static void push(oneMenu::Cmd c) { if(tail < 4) queue[tail++] = oneMenu::CKE{c}; }
  };
};
oneMenu::InDef<ScriptedIn> regIn;

// Same composition NAVROOT/AM4Nav actually builds (EventDispatch directly above
// TreeNav, no IndexGo) — the real-world-relevant shape for this check.
oneMenu::INavDef<
  oneMenu::EventDispatch,
  oneMenu::TreeNav,
  oneMenu::Root<decltype(regMenu), regMenu>
> regNav;

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // index 0=On, 1=Off, 2=Op1, 3=subMenu, 4=<Back>  (Cmd::Up increments, Down decrements)
  assert(action::op1Count == 0);
  nav.up(); nav.up();           // 0 -> 1 -> 2 (Op1)
  nav.enter();                  // fires action::op1 via OP()'s EventActionItem
  assert(action::op1Count == 1 && "OP() did not fire on Enter");

  nav.up();                     // 2 -> 3 (subMenu, spliced in via SUBMENU(subMenu))
  nav.enter();                  // opens the submenu (level 0 -> 1)
  assert(nav.level() == 1 && "SUBMENU(subMenu) did not open a nested level");
  nav.enter();                  // submenu index 0 = Sub1 -> fires action::op1 again
  assert(action::op1Count == 2 && "SUBMENU()'d menu's OP() did not fire");
  nav.esc();
  assert(nav.level() == 0 && "esc() did not close back out of the submenu");

  // FIELD() edits the real external variable directly via DataRef, zero-copy.
  // Selection is at index 3 (subMenu) after esc() — close() doesn't restore sel.
  // Cmd::Down decrements: 3 -> 2 (Op1) -> 1 (Off field).
  assert(timeOff == 90);
  nav.down(); nav.down();       // index 3 -> 1 (Off field)
  nav.enter();                  // enters edit mode on the field
  nav.up();                     // NumField::Part::nav inverts Up/Down while in Edit mode
                                 // (Cmd::Up -> Base::down()) — see item.h; not a compat-layer bug
  assert(timeOff == 89 && "FIELD() did not edit the bound variable via DataRef");
  nav.enter();                  // leave edit mode

  // ── Regression: OP()'s auto-dispatch fallback for a non-event-shaped fn ─────────
  // Selection is at index 1 (Off) here. index 0=On,1=Off,2=Op1,3=subMenu,4=Op2Legacy,5=<Back>.
  assert(action::op2LegacyCount == 0);
  nav.up(); nav.up(); nav.up();  // 1 -> 2 -> 3 -> 4 (Op2Legacy)
  nav.enter();
  assert(action::op2LegacyCount == 1 &&
         "OP()'s bool(int) fallback (plain Action<fn>) did not fire");

  // ── Regression 1: Enter/Exit gate on enabled(); Focus/Blur don't ────────────────
  assert(regtest::enterCount == 0 && regtest::focusCount == 0);
  regNav.enter();                // item 0 starts disabled -> Enter must NOT fire
  assert(regtest::enterCount == 0 && "Enter fired on a disabled item");
  regNav.up();                   // 0 -> 1 (Cmd::Up increments): must still fire Focus/Blur
  assert(regtest::blurCount == 1 && regtest::focusCount == 1 &&
         "Focus/Blur must fire regardless of enabled() (disabled items stay selectable)");
  regNav.down();                 // back to item 0
  auto& regItem0 = regMenu.find<SameAs<oneMenu::Id<0>>>();
  regItem0.enable(true);
  regNav.enter();
  assert(regtest::enterCount == 1 && "Enter did not fire once the item was enabled");

  // ── Regression 2: events also fire via the real input-driven poll() path ───────
  // (Pool::poll() -> InDef::doInput() -> inBurst() -> nav.in(*this)), not just direct
  // nav.up()/down()/enter()/esc() calls — the actual fix for the in()/doCmd
  // static-dispatch gap (TreeNav::Part::in(), nav.h).
  regItem0.enable(false);
  regtest::enterCount = regtest::exitCount = regtest::focusCount = regtest::blurCount = 0;
  regIn.push(oneMenu::Cmd::Enter);  // sel is still item 0 (disabled) -> must not fire
  regIn.inBurst(regNav);
  assert(regtest::enterCount == 0 && "poll()-path Enter fired despite a disabled item");

  regIn.push(oneMenu::Cmd::Up);     // 0 -> 1
  regIn.inBurst(regNav);
  assert(regtest::focusCount == 1 && regtest::blurCount == 1 &&
         "poll()-path Focus/Blur did not fire (in()/doCmd static-dispatch gap not fixed?)");

  regItem0.enable(true);
  regIn.push(oneMenu::Cmd::Down);   // 1 -> 0, landing back on the now-enabled item
  regIn.push(oneMenu::Cmd::Enter);
  regIn.inBurst(regNav);
  assert(regtest::enterCount == 1 &&
         "poll()-path Enter did not fire through EventDispatch (in()/doCmd static-dispatch gap)");

  // ── PADMENU()'s fn/mask auto-dispatch (am4compat::padDefStyle, am4.h, 2026-07-09) ──
  // padMenu (index 0 in padParent's body) is focused by default — no nav call needed
  // to arrive there. Enter on a pad item routes through PadDraw's own padOpen(), not
  // the regular open() a plain submenu uses — EventDispatch::doCmd fires on cmd *type*
  // (Cmd::Enter/Esc) regardless of which one ran internally, so this also proves
  // EventAction dispatch is agnostic to PadDraw's different nav.h Enter handling.
  assert(padtest::enterCount == 0);
  padNav.enter();
  assert(padtest::enterCount == 1 && "PADMENU()'s EventAction did not fire on Enter");
  assert(padtest::exitCount == 0);
  padNav.esc();
  assert(padtest::exitCount == 1 && "PADMENU()'s EventAction did not fire on Exit");

  printf("OK: MENU/FIELD/OP/EXIT/SUBMENU compat macros all verified\n");
  printf("OK: EventDispatch enabled()-gating + real poll()-path dispatch verified\n");
  printf("OK: MENU()/PADMENU() fn/mask auto-dispatch (menuDefStyle/padDefStyle) verified\n");
  return 0;
}
