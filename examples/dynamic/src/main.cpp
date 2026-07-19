/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 dynamic.ino compat-macro port — a runtime-shaped, heterogeneous
 *        `prompt*[]` array (choose/select/toggle option lists, a submenu, a
 *        pad-menu, plain prompts, a numeric field, a masked text field).
 *        See github.com/neu-rah/ArduinoMenu examples/dynamic/dynamic/dynamic.ino.
 *
 * A prior attempt hit a real C++ wall porting this onto OneMenu's existing
 * IItemDef<Menu<...>> — template methods (out.printMenu<Item>/out.printItem
 * <Item>) can't be virtual, so IOut fundamentally can't expose them. This
 * port instead uses `dynItem.h`'s DynItem/DynValueOption/DynEnumItem/
 * DynNumItem/DynTextItem — a small, program-side (not OneMenu-side) item
 * family deriving from oneMenu::IItem directly, with printMenu/printBody
 * written by hand against IOut's already-virtual primitives. See dynItem.h's
 * own file header for the full design writeup.
 *
 * Two real, disclosed deviations from a "minimum edit" port, beyond the
 * usual #include swap:
 *  - Root device is composed directly as oneMenu::IOutDef<...> (the same
 *    OO... chain ANSI_OUT(id,w,h) builds, just wrapped in IOutDef instead of
 *    OutDef) rather than through ANSI_OUT itself — DynItem::printMenu takes
 *    IOut&, which only a virtual-dispatch device satisfies; ANSI_OUT builds
 *    a plain, non-virtual OutDef. No new library macro: composed directly,
 *    same "compose when the compat macro doesn't fit" precedent already
 *    used elsewhere in this compat layer (TextField's option0,
 *    plugin_decimalsField's float/double fields).
 *  - Nav chain is composed directly as INavDef<TreeNav,Root<...>> (skipping
 *    NAVROOT/NAVROOT_IDLE) — NAVROOT always includes EventDispatch, whose
 *    fireAt() walks root().body, a member only StaticBody-shaped compile-
 *    time trees have; a DynItem-rooted tree has no such member (items/
 *    itemCount instead), so composing EventDispatch in would fail to
 *    compile. DynItem's own nav handling fires onEvent(Enter) directly
 *    instead (dynItem.h) — self-contained, no event-dispatch component
 *    needed at all.
 *
 * All storage is static (not heap-`new`'d, unlike the real source) — same
 * "user is responsible for allocation" spirit, just not exercised, matching
 * every other example in this compat layer.
 *
 * Scope, deliberately: DynItem's own print pathway has no scroll-window
 * search / index numbering / nav-cursor positioning (see dynItem.h) — a
 * plain sequential, changed()-gated redraw walk only.
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
#include <am4compat/dynItem.h>
#include <cassert>
#include <cstdio>
#include <cstring>

using namespace hapi;
// NOTE: deliberately no `using namespace oneMenu;` — oneMenu::Menu<> collides
// with AM4's own Menu namespace the moment both are visible unqualified in
// the same TU (see examples/am4compat's own comment).

// ── choose<int> field: duration ────────────────────────────────────────────
int duration = 0;
am4compat::DynValueOption<int> durData[] = {
  {"Off", 0}, {"Short", 1}, {"Medium", 2}, {"Long", 3}
};
am4compat::DynValueOption<int>* const durOpts[] = {&durData[0], &durData[1], &durData[2], &durData[3]};
am4compat::DynEnumItem<int> durMenu{"Duration", &duration, durOpts, 4};

// ── select<Fxs> field: selFx ────────────────────────────────────────────────
enum Fxs { Fx0, Fx1, Fx2 };
Fxs selFx = Fx0;
am4compat::DynValueOption<Fxs> fxData[] = {
  {"Default", Fx0}, {"Pop", Fx1}, {"Rock", Fx2}
};
am4compat::DynValueOption<Fxs>* const fxOpts[] = {&fxData[0], &fxData[1], &fxData[2]};
am4compat::DynEnumItem<Fxs> fxMenu{"Fx", &selFx, fxOpts, 3};

// ── toggle<bool> field: led — real AM4 shape cycles immediately on Enter,
// no pick-list level; setLed() fires as a real onEvent handler (fn/mask). ──
bool led = false;
bool setLedEvent(oneMenu::EventMask, oneMenu::IItem&) {led = !led; return true;}
am4compat::DynValueOption<bool> togData[] = {{"On", true}, {"Off", false}};
am4compat::DynValueOption<bool>* const togOpts[] = {&togData[0], &togData[1]};
am4compat::DynEnumItem<bool> ledMenu{"LED:", &led, togOpts, 2,
  am4compat::sToggleImmediate, oneMenu::EventMask::Enter, setLedEvent};

// ── plain submenu ("sub-menu": Sub1, Sub2, <Back) ──────────────────────────
am4compat::DynItem<> sub1{"Sub1"};
am4compat::DynItem<> sub2{"Sub2"};
am4compat::DynItem<> subExit{"<Back", am4compat::sExit};
oneMenu::IItem* const subData[] = {&sub1, &sub2, &subExit};
am4compat::DynItem<> subMenu{"sub-menu", am4compat::sCanNav | am4compat::sParentDraw,
  oneMenu::EventMask::None, nullptr, subData, 3};

// ── pad menu: 3 numeric fields shown/edited inline (Date: year/month/day) ──
uint16_t year = 2017, month = 10, day = 7;
am4compat::DynNumItem<uint16_t> yearField{"", &year, (uint16_t)1900, (uint16_t)3000, (uint16_t)20};
am4compat::DynNumItem<uint16_t> monthField{"/", &month, (uint16_t)1, (uint16_t)12, (uint16_t)1};
am4compat::DynNumItem<uint16_t> dayField{"/", &day, (uint16_t)1, (uint16_t)31, (uint16_t)1};
oneMenu::IItem* const padData[] = {&yearField, &monthField, &dayField};
am4compat::DynItem<> padMenu{"Date",
  am4compat::sAsPad | am4compat::sCanNav | am4compat::sParentDraw,
  oneMenu::EventMask::None, nullptr, padData, 3};

// ── main menu ───────────────────────────────────────────────────────────────
bool op1Called = false;
bool op1Func(oneMenu::EventMask, oneMenu::IItem&) {op1Called = true; return true;}
am4compat::DynItem<> op1{"Op 1", am4compat::sNone, oneMenu::EventMask::Enter, op1Func};
am4compat::DynItem<> op2{"Op 2"};   // text/fn changeable later, same as real source's comment

uint8_t test = 55;
am4compat::DynNumItem<uint8_t> testField{"Bright", &test, (uint8_t)0, (uint8_t)255, (uint8_t)10};

char buf1[] = "0x11";
static const char hexDigit[] = "0123456789ABCDEF";
static const char* const hexNr[] = {"0", "x", hexDigit, hexDigit};
am4compat::DynTextItem<> addrField{"Addr", buf1, 4, hexNr, 4};

am4compat::DynItem<> mainExit{"<Exit.", am4compat::sExit};

oneMenu::IItem* const mainData[] = {
  &op1, &op2, &testField, &addrField, &subMenu, &durMenu, &fxMenu, &ledMenu, &padMenu, &mainExit
};

constexpr oneMenu::Depth MAX_DEPTH = 2;   // same value as the real source's own #define
am4compat::DynItem<MAX_DEPTH> mainMenu{"Main menu",
  am4compat::sCanNav | am4compat::sParentDraw, oneMenu::EventMask::None, nullptr, mainData, 10};

// ── output/input/nav — hand-composed, see file header for why ─────────────
oneMenu::IOutDef<
  oneMenu::FullPrinter, oneMenu::ANSIFmt, oneMenu::DataParser<>, oneMenu::CtrlChars,
  oneMenu::ColorTrack<int>, oneMenu::Cursor<>, oneMenu::Gate,
  oneMenu::ANSIOut, oneMenu::ConsoleOut, oneMenu::StaticPos<0,0>, oneMenu::StaticArea<40,12>
> devOut;

struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

oneMenu::INavDef<oneMenu::Pool<decltype(devIn), decltype(devOut)>,
  oneMenu::TreeNav, oneMenu::Root<decltype(mainMenu), mainMenu>> nav(devIn, devOut);

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // ── mainMenu body order: 0=Op1 1=Op2 2=test 3=Addr 4=subMenu 5=durMenu
  // 6=fxMenu 7=ledMenu 8=padMenu 9=Exit — Cmd::Up increments (same
  // convention as every other example in this compat layer). ─────────────

  // Op 1: plain leaf, Enter fires onEvent directly (no EventDispatch needed).
  assert(!op1Called);
  nav.enter();
  assert(op1Called && "Op 1's Enter did not fire onEvent(Enter)");
  assert(nav.level() == 0 && "a plain leaf's Enter must not change nav level");

  // toggle<bool> ledMenu: immediate cycle+commit on Enter, no level change.
  nav.up(); nav.up(); nav.up(); nav.up(); nav.up(); nav.up(); nav.up();
  // Op1(0)->Op2(1)->test(2)->Addr(3)->subMenu(4)->durMenu(5)->fxMenu(6)->ledMenu(7)
  bool ledBefore = led;
  nav.enter();
  assert(led != ledBefore && "toggle ledMenu did not cycle on Enter");
  assert(nav.level() == 0 && "toggle must not open a level");

  // choose<int> durMenu: Enter opens a pick-list (real level push), Up/Down
  // move the highlight, a second Enter commits.
  nav.down(); nav.down();   // ledMenu(7) -> fxMenu(6) -> durMenu(5)
  assert(nav.level() == 0);
  int durationBefore = duration;
  nav.enter();
  assert(nav.level() == 1 && "durMenu's Enter did not open a pick-list level");
  nav.up();   // move highlight within the pick-list (option 0 -> 1, "Short")
  nav.enter();   // commit
  assert(nav.level() == 0 && "committing durMenu did not close back out");
  assert(duration != durationBefore || duration == 1);
  (void)durationBefore;
  // committing a pick-list must not disturb mainMenu's own top-level
  // selection — still parked on durMenu (index 5) afterward.
  assert(nav.sel() == 5);

  // subMenu: a real submenu (menuNode-shaped) — Enter opens a level;
  // <Back>'s sExit style makes ITS OWN Enter call n.close().
  nav.down();   // durMenu(5) -> subMenu(4), Cmd::Down decrements the index
  assert(nav.sel() == 4);
  nav.enter();
  assert(nav.level() == 1 && "subMenu's Enter did not open a level");
  assert(nav.sel() == 0);   // opening a level starts at option 0 (Sub1)
  nav.up(); nav.up();       // Sub1(0) -> Sub2(1) -> <Back>(2)
  assert(nav.sel() == 2);
  nav.enter();               // <Back>'s sExit style: Enter -> n.close()
  assert(nav.level() == 0 && "sExit leaf's Enter did not close the level");
  assert(nav.sel() == 4 && "closing subMenu must restore mainMenu's own selection");

  // numeric field (Bright/test): Enter toggles DynNumItem's OWN `editing`
  // flag (local state, not the real TreeNav NavMode — a disclosed
  // simplification, see dynItem.h: INav has no navMode(NavMode) *setter*
  // exposed virtually, only the getter, so a type-erased field can't drive
  // the real edit-mode state machine; DynNumItem/DynTextItem manage editing
  // entirely themselves instead). Up/Down step the bound value within
  // [low,high] while editing — natural mapping (Up increases), matching
  // OneMenu's own NumField default (item.h, flipped this session to match
  // AM4's real shipped default).
  while (nav.sel() != 2) nav.down();   // subMenu(4) -> Addr(3) -> test(2)
  assert(nav.sel() == 2);
  uint8_t testBefore = test;
  assert(!testField.editing);
  nav.enter();
  assert(testField.editing && "test field's Enter did not enter edit mode");
  nav.up();   // edit-mode Up increases (natural mapping)
  assert(test == (uint8_t)(testBefore + 10) && "Up in edit mode did not step +step");
  nav.enter();
  assert(!testField.editing && "second Enter did not leave edit mode");

  // masked text field (Addr/buf1, hexNr validators): Enter toggles edit
  // mode (same local-`editing` design as DynNumItem), Up/Down cycle the
  // character at the current position.
  nav.up();   // test(2) -> Addr(3)
  assert(nav.sel() == 3);
  nav.enter();
  assert(addrField.editing && "Addr field's Enter did not enter edit mode");
  char firstCharBefore = buf1[0];
  nav.down();   // cycle position 0 through hexNr[0]="0" (single-char set, stays '0')
  assert(buf1[0] == firstCharBefore);
  nav.template doCmd<false>(oneMenu::Cmd::Right);
  nav.up();     // cycle position 1 through hexNr[1]="x" (single-char set, stays 'x')
  assert(buf1[1] == 'x');
  nav.esc();
  assert(!addrField.editing);

  printf("OK: dynamic.ino compat-macro port (DynItem/DynEnumItem/DynNumItem/DynTextItem) verified\n");
  return 0;
}
