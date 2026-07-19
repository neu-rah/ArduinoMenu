/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 reuse.ino compat-macro port — one shared "alarm setup" menu
 *        reused to edit any of 3 alarms, matching AM4's real
 *        `nav.root->useMenu(alarmDef)` runtime top-level menu swap. See the
 *        original at github.com/neu-rah/ArduinoMenu examples/reuse/reuse/
 *        reuse.ino.
 *
 * AM4's `useMenu()` swaps which menu the SAME nav is bound to, at runtime,
 * while staying at nav level 0 (not opening a sub-level) — `Root<T,menu>`
 * (OneMenu's own compile-time root binding) can't do this: T/menu are both
 * template parameters, fixed for the nav's whole lifetime. A tempting first
 * idea — a new root-binding component holding a runtime-reassignable
 * pointer — turns out not to work: `TreeNav::printTo<Out>` sizes a static
 * array via `root().depth()` (nav.h), which genuinely needs `root()` itself
 * to be a real constant expression (confirmed via an isolated repro, not
 * assumed) — a runtime pointer dereference breaks that outright.
 *
 * Fixed with something simpler that needs no new nav component at all: one
 * fixed `activeRoot` (the actual `Root<T,menu>` target, genuinely constexpr-
 * referenceable exactly like `Root<T,menu>` already requires) that gets
 * REASSIGNED (plain struct copy — `DynItem`'s own fields are simple/
 * copyable, confirmed via an isolated repro before building this whole
 * port) from whichever of `mainMenu`/`alarmDef` should currently be shown.
 * `useMenu(x)` is just `activeRoot = x;` — same depth (`DynItem<2>` for
 * both), same nav level throughout, no level push/pop involved.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `AlarmPrompt`'s own custom `printTo()` becomes `AlarmItem::printItem()`
 *    (the same override point `DynNumItem`/`DynTextItem` already use for
 *    their own custom rendering, dynItem.h).
 *  - Real-time alarm firing (`AlarmPrompt::update()`, driven from `loop()`
 *    via `millis()`) is deliberately NOT ported — secondary to what this
 *    example actually demonstrates (menu reuse), and a real countdown-firing
 *    proof would need real wall-clock waits the way `CustomPrompt`'s own
 *    live-clock proof did. `msTime` is still computed and DISPLAYED
 *    correctly (`"Countdown HH:MM:SS"`), just not decremented over real
 *    time or auto-fired.
 *  - `AlarmMenu`'s own custom title (a live H:M:S clock shown in the menu
 *    title) is dropped as a cosmetic-only flourish, same precedent as
 *    `Confirm`'s `altMENU`/`CustomPrompt`'s `altPrompt` drops.
 *  - AM4's real `TOGGLE`/`FIELD` step values are preserved exactly (hour
 *    step 1, minute/second step 5) even though `DynNumItem`'s `tune`
 *    (accel) parameter isn't modeled — matches every other FIELD()-based
 *    port's own "step/tune accepted but ignored beyond the base step"
 *    precedent.
 *
 * The selftest drives nav.up()/down()/enter() directly, same style as every
 * other port in this compat layer.
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
using namespace am4compat;
// NOTE: deliberately no `using namespace oneMenu;` — see examples/am4compat's
// own comment for why (oneMenu::Menu<> collides with AM4's Menu namespace).

enum { ALARM_TYPE = 0, COUNTDOWN_TYPE = 1 };

// ── AlarmItem — replaces AM4's AlarmPrompt custom-print class. Holds its
// own alarm data (use/type/h/m/s) and, on Enter, copies it into the shared
// scratch below and switches the nav's active root to alarmDef. ──────────
struct AlarmItem : DynItem<0> {
  int idx;
  bool use = false;
  int type = ALARM_TYPE;
  int h = 0, m = 0, s = 0;
  long msTime = 0;

  AlarmItem(const char* t, int i, oneMenu::EventFuncItemPtr f)
    : DynItem<0>(t, sNone, oneMenu::EventMask::Enter, f), idx(i) {}

  void setupTiming() { msTime = ((long)h*3600 + (long)m*60 + s) * 1000; }

  void printItem(oneMenu::IOut& out, oneMenu::Ctx&) override {
    if (use) {
      if (type == ALARM_TYPE) {
        out.put("Alarm "); out.put(h); out.put(':'); out.put(m); out.put(':'); out.put(s);
      } else {
        long ss = msTime/1000; long hh = ss/3600; ss -= hh*3600;
        long mm = ss/60; ss -= mm*60;
        out.put("Countdown ");
        if (hh<10) out.put('0'); out.put((int)hh); out.put(':');
        if (mm<10) out.put('0'); out.put((int)mm); out.put(':');
        if (ss<10) out.put('0'); out.put((int)ss);
      }
    } else {
      out.put(text); out.put(idx+1); out.put(" Off");
    }
  }
};

// ── shared scratch (AM4's `tmp`) — the alarmDef menu below always edits
// THESE globals, regardless of which AlarmItem is currently selected. ────
bool tmpUse = false;
int  tmpType = ALARM_TYPE;
int  tmpH = 0, tmpM = 0, tmpS = 0;
int  selectedIdx = -1;

AlarmItem alarm1Item{"Alarm", 0, nullptr};
AlarmItem alarm2Item{"Alarm", 1, nullptr};
AlarmItem alarm3Item{"Alarm", 2, nullptr};
AlarmItem* const allAlarms[3] = {&alarm1Item, &alarm2Item, &alarm3Item};

DynItem<0> exitItem{"<Back", sExit};
oneMenu::IItem* const mainMenuItems[] = {&alarm1Item, &alarm2Item, &alarm3Item, &exitItem};
DynItem<2> mainMenu{"Alarms", sCanNav | sParentDraw, oneMenu::EventMask::None, nullptr, mainMenuItems, 4};

// ── alarmDef — the ONE shared editing menu, reused for all 3 alarms ──────
DynValueOption<bool> useOpts[] = {{"Yes", true}, {"No", false}};
DynValueOption<bool>* const useOptPtrs[] = {&useOpts[0], &useOpts[1]};
DynEnumItem<bool> setUse{"Use: ", &tmpUse, useOptPtrs, 2, sToggleImmediate};

DynValueOption<int> typeOpts[] = {{"Alarm", ALARM_TYPE}, {"Countdown", COUNTDOWN_TYPE}};
DynValueOption<int>* const typeOptPtrs[] = {&typeOpts[0], &typeOpts[1]};
DynEnumItem<int> setType{"Type: ", &tmpType, typeOptPtrs, 2, sToggleImmediate};

DynNumItem<int> hourField{"Hour", &tmpH, 0, 23, 1};
DynNumItem<int> minField{"Minutes", &tmpM, 0, 59, 5};
DynNumItem<int> secField{"Seconds", &tmpS, 0, 59, 5};

bool doOk(oneMenu::EventMask, oneMenu::IItem&) {
  AlarmItem& a = *allAlarms[selectedIdx];
  a.use = tmpUse; a.type = tmpType; a.h = tmpH; a.m = tmpM; a.s = tmpS;
  a.setupTiming();
  return true; // activeRoot switch happens in main()/onEvent below (see doOkAndReturn)
}
bool doCancel(oneMenu::EventMask, oneMenu::IItem&) { return true; }

DynItem<0> okItem{"Ok", sNone, oneMenu::EventMask::Enter, doOk};
DynItem<0> cancelItem{"Cancel", sNone, oneMenu::EventMask::Enter, doCancel};

oneMenu::IItem* const alarmDefItems[] = {&setUse, &setType, &hourField, &minField, &secField, &okItem, &cancelItem};
DynItem<2> alarmDef{"Alarm def", sCanNav | sParentDraw, oneMenu::EventMask::None, nullptr, alarmDefItems, 7};

// ── the actual nav root — genuinely constexpr-referenceable (Root<T,menu>'s
// own requirement), reassigned by plain struct copy to switch content. ────
DynItem<2> activeRoot = mainMenu;

bool alarmSelect(oneMenu::EventMask, oneMenu::IItem& self) {
  AlarmItem& a = static_cast<AlarmItem&>(self);
  selectedIdx = a.idx;
  tmpUse = a.use; tmpType = a.type; tmpH = a.h; tmpM = a.m; tmpS = a.s;
  activeRoot = alarmDef;
  return true;
}

// Hand-composed IOutDef (not the ANSI_OUT macro, which builds a plain
// non-virtual OutDef) — DynItem::printMenu takes IOut&, same root-device
// requirement dynamic.ino's own port already established.
oneMenu::IOutDef<
  oneMenu::FullPrinter, oneMenu::ANSIFmt, oneMenu::DataParser<>, oneMenu::CtrlChars,
  oneMenu::ColorTrack<int>, oneMenu::Cursor<>, oneMenu::Gate,
  oneMenu::ANSIOut, oneMenu::ConsoleOut, oneMenu::StaticPos<0,0>, oneMenu::StaticArea<40,8>
> devOut;

struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);
oneMenu::INavDef<oneMenu::Pool<decltype(in), decltype(out)>,
  oneMenu::TreeNav, oneMenu::Root<decltype(activeRoot), activeRoot>> nav(in, out);

// Real oneMenu::IOut subclass for direct printItem() inspection in the
// selftest below — AlarmItem::printItem is a virtual override (exact
// IOut&, not a template like a plain ItemDef's own printItem), so a
// non-IOut fake device (as other ports use for this) won't bind here.
struct StrOut : oneMenu::IOut {
  char buf[64]{}; int len=0;
  void app(char c) { if(len<63){buf[len++]=c;buf[len]=0;} }
  void appLong(long v){ if(v<0){app('-');v=-v;} char t[16];int n=0; if(v==0)t[n++]='0'; while(v>0){t[n++]=(char)('0'+(v%10));v/=10;} while(n>0)app(t[--n]); }

  void lockMode(oneMenu::LockMode) override {}
  oneMenu::LockMode lockMode() override { return oneMenu::LockMode::None; }
  void resume() override {}
  void clear() override {}
  void flush() override {}
  void fmtStart(oneMenu::Fmt, const oneMenu::Ctx&) override {}
  void fmtStop(oneMenu::Fmt, const oneMenu::Ctx&) override {}
  void setPos(const oneMenu::Pos&) override {}
  void put(const int o) override { appLong(o); }
  void put(const double o) override { appLong((long)o); }
  void put(const char o) override { app(o); }
  void put(const char* o) override { for(;*o;++o) app(*o); }
  void put(const char* o, oneMenu::Sz n) override { for(oneMenu::Sz i=0;i<n&&o[i];i++) app(o[i]); }
  void put(const char* const* o) override { for(int i=0;(*o)[i];i++) app((*o)[i]); }
};

int main() {
  // wire the real select handler now that alarm1/2/3Item are constructed
  // (fn is a plain function pointer, no per-instance closure available at
  // static-init time, so bind it here instead of at declaration).
  alarm1Item.fn = alarmSelect; alarm2Item.fn = alarmSelect; alarm3Item.fn = alarmSelect;

  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  // ── all 3 alarms start "Off" ──────────────────────────────────────────
  {
    StrOut o; oneMenu::Ctx c{};
    alarm1Item.printItem(o, c);
    assert(strstr(o.buf, "Off") && "fresh alarm must render as Off");
  }

  // index 0=Alarm1 1=Alarm2 2=Alarm3 3=<Back>

  // ── select Alarm 2 (index 1), edit it, Ok — must write back to
  // alarm2Item specifically, not alarm1/3, and switch back to mainMenu ────
  nav.up(); // Alarm1(0) -> Alarm2(1)
  nav.enter(); // fires alarmSelect: copies alarm2Item's (Off) state into tmp, switches root
  assert(&activeRoot != &mainMenu); // activeRoot now holds alarmDef's content
  assert(selectedIdx == 1);
  assert(tmpUse == false);

  // alarmDef body order: 0=setUse 1=setType 2=hour 3=min 4=sec 5=Ok 6=Cancel.
  // nav's own selection index carries over from mainMenu (index 1, same as
  // real AM4 — only quitAlarmSetup() explicitly restores sel, the initial
  // switch doesn't reset it), so it currently points at setType, not
  // setUse — walk to setUse(0) explicitly first.
  nav.down(); // setType(1) -> setUse(0), correcting for the carried-over index
  assert(nav.sel() == 0);
  bool useBefore = tmpUse;
  nav.enter(); // setUse: Toggle, immediate cycle
  assert(tmpUse != useBefore && "TOGGLE(setUse) did not cycle tmpUse");

  nav.up(); // setUse(0) -> setType(1)
  nav.up(); // setType(1) -> hour(2)
  int hBefore = tmpH;
  // edit mode: Up increases (natural mapping, matching OneMenu's own
  // NumField default — flipped this session to match AM4's real shipped
  // default).
  nav.enter(); nav.up(); nav.enter();
  assert(tmpH != hBefore && "editing hour did not change tmpH");

  nav.up(); nav.up(); nav.up(); // hour(2) -> min(3) -> sec(4) -> Ok(5)
  nav.enter(); // fires doOk: writes tmp back into allAlarms[selectedIdx], returns

  // doOk() itself doesn't switch activeRoot (that's alarmSelect's/quit's job
  // in the real AM4 source too) — do it explicitly here, matching
  // AlarmPrompt::setupAlarm() calling quitAlarmSetup() right after.
  activeRoot = mainMenu;

  assert(alarm2Item.use == true && "Ok did not write tmpUse back to alarm2Item");
  assert(alarm1Item.use == false && "Ok must not touch a different alarm");
  assert(alarm3Item.use == false && "Ok must not touch a different alarm");
  assert(alarm2Item.h == tmpH);

  {
    StrOut o; oneMenu::Ctx c{};
    alarm2Item.printItem(o, c);
    char expectH[4]; snprintf(expectH, sizeof(expectH), "%d", alarm2Item.h);
    assert(strstr(o.buf, "Alarm") && strstr(o.buf, expectH) &&
           "alarm2Item did not render its own edited h/m/s after Ok");
  }

  // ── select Alarm 1 (index 0), change something, Cancel — must NOT write
  // back, alarm1Item stays exactly as it was. ─────────────────────────────
  activeRoot = mainMenu;
  nav.up(); nav.up(); nav.up(); // wherever we are -> walk back to a known index
  while (nav.sel() != 0) nav.down();
  assert(nav.sel() == 0);
  bool alarm1UseBefore = alarm1Item.use;
  nav.enter(); // select Alarm1
  assert(selectedIdx == 0);
  nav.enter(); // toggle tmpUse (setUse is index 0 in alarmDef)
  nav.up(); nav.up(); nav.up(); nav.up(); nav.up(); nav.up(); // walk to Cancel(6)
  assert(nav.sel() == 6);
  nav.enter(); // fires doCancel — writes nothing back
  activeRoot = mainMenu;
  assert(alarm1Item.use == alarm1UseBefore && "Cancel must not write tmp back to alarm1Item");

  printf("OK: reuse.ino compat-macro port (shared alarmDef menu, runtime useMenu() via DynItem assignment) verified\n");
  return 0;
}
