/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 SSD1306Ascii.ino compat-macro port — fast text-only SSD1306
 *        OLED + Serial output, driven from real Serial input. See the
 *        original at github.com/neu-rah/ArduinoMenu examples/SSD1306Ascii/
 *        SSD1306Ascii/SSD1306Ascii.ino.
 *
 * Rui's own read on this one turned out to be exactly right, and more:
 * SSD1306Ascii is a fast text-only OLED driver, and OneIO already has a
 * complete native equivalent — `oneIO::display::Ssd1306<Transport,W,H>`
 * (`OneIO/include/oneIO/display/ssd1306.h`, its own text mode built on
 * `font5x8.h`, with `charWidth()`/`lineSpacing()` hooks already wired for
 * OneMenu's `Cursor<>`) plus a ready-made `oneMenu::OledDisplay<Oled>`
 * `OutDef` (`FullPrinter`+`GfxFmt<>`+`OledOut<Oled>`, inverted-video
 * selection, already documented in `docs/oneMenu.md`) — no vendor
 * `SSD1306Ascii`/`SSD1306AsciiWire` library needed at all. Real hardware
 * proven separately at `.RnD/AM4check/SSD1306Ascii` (gitignored), same
 * "native selftest here, real device wiring there" split as every other
 * port this session.
 *
 * Font, checked directly against the real font sources (not assumed):
 * OneIO's built-in 5x8 font is close to, but not byte-identical with,
 * SSD1306Ascii's own `System5x7` (most glyphs match, F/G/W/Y differ) — and
 * the real `.ino`'s own default font is actually `X11fixed7x14`, a
 * completely different 6x14 font stored as 12-byte 2-page glyphs, nothing
 * like `Ssd1306`'s current single-page 5x8 renderer. Matching it exactly
 * would mean vendoring that font table into OneIO and adding real
 * 2-page-glyph rendering support — confirmed with Rui to skip this and
 * keep OneIO's existing 5x8 font instead (disclosed cosmetic difference,
 * same class of simplification as other ports' font/color deviations).
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `action1(eventMask)` is genuinely 1-arg in this source (unlike
 *    `action2`) — ported as `bool(EventMask,IItem&)` (2-arg, unused
 *    `IItem&`), same "trim up to the nearest supported shape for real
 *    event dispatch" precedent as `codeCtrl.ino`'s own `action2`.
 *  - `action2`/`showEvent` (real 3-arg) kept as `bool(EventMask,INav&,
 *    IItem&)` directly, same precedent as `btnNav`/`LiquidCrystal`.
 *  - `showEvent` doubles as `subMenu`'s own `MENU()`-level title fn in the
 *    original — same `titleEvent` (1-arg `bool(EventMask)`) substitution
 *    `ansiSerial`/`LiquidCrystal`/`clickEncoder` already needed (`MENU()`'s
 *    fn slot only auto-dispatches 1-arg).
 *  - `myLedOn`/`myLedOff` (0-arg) ported as `bool(int)`, established
 *    adapter shape.
 *  - `altOP(altPrompt,"",showEvent,anyEvent)` inside `subMenu` dropped —
 *    custom-prompt subclass, same precedent as every other port.
 *  - `idle(menuOut&,idleEvent)`'s real per-device branching (`&o==
 *    &outOLED`, different idle text for the OLED vs Serial) is a real,
 *    disclosed simplification: the compat layer's own idle mechanism
 *    (`Run`/manual `idleOn()`) is a plain `bool()` callback with no
 *    per-device parameter to branch on — this port shows one generic idle
 *    message covering both devices instead, same class of simplification
 *    as `LiquidCrystal.ino`'s own 3-state idle approximation.
 *  - Real Serial input (`Menu::serialIn` — the *only* input device in the
 *    original) is dropped from this native selftest — routes through the
 *    held `InDef::inBurst()` path (see [[feedback_inburst_review_hold]]),
 *    same as `concurrent`/`ajoy`'s own precedent.
 *
 * The selftest drives nav.up()/enter() directly, same style as every other
 * port in this compat layer.
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
  int op1Count = 0;
  bool action1(oneMenu::EventMask, oneMenu::IItem&) { op1Count++; return true; }

  int op2Count = 0;
  bool action2(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { op2Count++; return true; }

  int subCount = 0;
  bool showEvent(oneMenu::EventMask, oneMenu::INav&, oneMenu::IItem&) { subCount++; return true; }

  int titleCount = 0;
  bool titleEvent(oneMenu::EventMask) { titleCount++; return true; }

  int ledOnCount = 0, ledOffCount = 0;
  bool myLedOn(int)  { ledOnCount++;  return true; }
  bool myLedOff(int) { ledOffCount++; return true; }

  // FIELD()'s fn slot needs a plain void() (see am4.h's own doc comment on
  // FIELD() — Menu::doNothing is bool(int), a genuine type mismatch here).
  void noOp() {}
}

int test = 55;

int ledCtrl = -1;
TOGGLE(ledCtrl, setLed, "Led: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
  ,VALUE("On",  1, Menu::doNothing, Menu::noEvent)
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

bool doAlert(oneMenu::EventMask, oneMenu::IItem&);

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
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
  ,EXIT("<Back")
);

ANSI_OUT(devOut, 40, 10);

// deterministic zero-op input source — the selftest drives nav directly;
// real Serial input is held per the inBurst review hold, see file header
// comment; real OLED+Serial output hardware is proven separately.
struct NoOpIn {
  template<typename In> struct Part : In {
    static bool available() { return false; }
    static oneMenu::CKE cmd() { return {}; }
  };
};
oneMenu::InDef<NoOpIn> devIn;

MENU_INPUTS(in, &devIn);
MENU_OUTPUTS(out, /*maxDepth*/2, &devOut);

bool mainRun();
using Run = oneMenu::RunLoop<mainRun>;

NAVROOT_IDLE(nav, mainMenu, /*maxDepth*/2, in, out, Run);

int alertStartCount = 0, alertEndCount = 0, alertTicks = 0;
bool alertRun() {
  if(alertTicks==0) {
    alertStartCount++;
    printf("suspending menu!\n");
  }
  printf("alert test\n[select] to continue...\n");
  alertTicks++;
  if(alertTicks>=3) {
    alertEndCount++;
    printf("resuming menu.\n");
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

  // index 0=Op1,1=Op2,2=Test,3=subMenu,4=setLed,5=LEDOn,6=LEDOff,
  // 7=selMenu,8=chooseMenu,9=Alert,10=<Back>

  // ── Op1 (2-arg trim of a real 1-arg AM4 handler) / Op2 (real 3-arg) ──────
  assert(action::op1Count == 0);
  nav.enter();  // index 0 = Op1
  assert(action::op1Count == 1 && "action1's 2-arg trim did not fire on Enter");

  assert(action::op2Count == 0);
  nav.up();     // 0 -> 1 (Op2)
  nav.enter();
  assert(action::op2Count == 1 && "action2's real 3-arg shape did not fire on Enter");

  // ── subMenu: titleEvent (MENU()-level) + showEvent (OP()-level) ──────────
  nav.up(); nav.up();  // Op2(1) -> Test(2) -> subMenu(3)
  int titleAtLanding = action::titleCount;
  assert(titleAtLanding > 0 && "subMenu's own MENU()-level titleEvent did not observe Focus on landing");
  nav.enter();
  assert(nav.level() == 1 && "subMenu did not open");
  assert(action::titleCount > titleAtLanding &&
         "subMenu's own MENU()-level titleEvent did not additionally fire on Enter");
  nav.enter();  // Sub1 (index 0 inside subMenu)
  assert(action::subCount == 1 && "Sub1's real 3-arg showEvent did not fire on Enter");
  nav.esc();
  assert(nav.level() == 0);

  // ── TOGGLE ────────────────────────────────────────────────────────────
  assert(ledCtrl == -1);
  nav.up();     // subMenu(3) -> setLed(4)
  nav.enter();
  assert(ledCtrl == 0 && "TOGGLE() did not cycle to and sync the next VALUE()");

  // ── LED On/Off (0-arg AM4 handlers ported as bool(int)) ─────────────────
  assert(action::ledOnCount == 0);
  nav.up();     // setLed(4) -> LEDOn(5)
  nav.enter();
  assert(action::ledOnCount == 1);
  nav.up();     // LEDOn(5) -> LEDOff(6)
  nav.enter();
  assert(action::ledOffCount == 1);

  // ── Alert test: manual-only idle with a generic idle message (real
  // per-device idle text is a disclosed simplification, see file header
  // comment) ───────────────────────────────────────────────────────────────
  nav.up(); nav.up(); nav.up();  // LEDOff(6) -> selMenu(7) -> chooseMenu(8) -> Alert(9)
  assert(!Run::active());
  nav.enter();
  assert(Run::active() && "doAlert did not call nav.idleOn(alertRun)");
  while(Run::active()) Run::run();
  assert(alertStartCount == 1 && alertEndCount == 1 &&
         "alertRun did not run to completion and hand control back via Run::idleOff()");

  printf("OK: SSD1306Ascii.ino compat-macro port (native Ssd1306+OledDisplay "
         "OLED hardware proven separately, see .RnD/AM4check/SSD1306Ascii) "
         "verified\n");
  return 0;
}
