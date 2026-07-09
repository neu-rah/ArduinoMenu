/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 targetSel.ino compat-macro port — "user defined array menu
 *        (userMenu plugin)": N editable records, each opened through a
 *        shared-scratch edit form. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/targetSel/targetSel/
 *        targetSel.ino.
 *
 * AM4's `UserMenu` plugin backs this with a genuinely *runtime*-sized
 * `prompt*[]` array plus a nav-tree reflection hack (`nav.root->
 * path[nav.root->level-1].sel`) to recover which record a Save button is
 * editing. Neither is needed here: `dataSz` is a compile-time constant
 * (`constexpr int dataSz=3`, same in the real source), so this is a
 * zero-overhead `oneMenu::CArrayBody<T,arr,N>` (docs/oneMenu.md's own
 * "same-type C array, no virtual functions" body) — one item TYPE, N
 * runtime instances, no `UserMenu` port needed at all. Rui's own
 * observation ("OneMenu has a body aggregator, is that what you need?")
 * is what unblocked this — see notes.md "AM4 compat layer" for the full
 * design writeup, incl. a standalone prototype that validated this shape
 * before this file was written.
 *
 * Real deviations from the original, beyond the usual #include swap:
 *  - `target` (AM4's shared scratch record) is flattened into separate
 *    top-level globals (`targetName`/`targetValid`/`targetSelTest`/
 *    `targetChooseTest`) instead of one `struct TargetRecord target;` instance.
 *    Real, load-bearing reason, not stylistic: `DataRef<&target.selTest>`
 *    (a pointer to a non-static member subobject reached via `.`) is not a
 *    valid C++ template argument — confirmed via an isolated minimal
 *    repro (`struct S{int x;}; S s; template<auto p> struct T{}; T<&s.x>`
 *    fails: "not a valid template argument ... is not a variable or
 *    function") even as a plain `constexpr` initializer, let alone an
 *    NTTP — a real standard-C++ restriction, not a codebase bug, and
 *    (unlike the function-reference-NTTP quirks hit earlier this session)
 *    naming the pointer first doesn't rescue it either. `oneData::DataFn
 *    <Src>` (a getter/setter *type*, no address NTTP involved at all) was
 *    prototyped and confirmed as one working fix, but `EDIT()`'s own
 *    buffer binding (`TextBufRef<buf,sz>`) needs a compile-time buffer
 *    *address* NTTP regardless, hitting the identical member-address
 *    restriction for `target.name` specifically — so flattening was
 *    needed for the Name field no matter what; the other three fields were
 *    flattened alongside it for consistency rather than mixing two binding
 *    strategies for one logical "scratch record."
 *  - `saveTarget` (real AM4 signature: `result(eventMask,navNode&)`, used
 *    only to reach the reflection hack above) is ported as `bool(int)` —
 *    the plain `Action<fn>` shape — since `CopyInOnEnter` (below) already
 *    tracks `lastEditedIdx` as ordinary state, no nav-tree reflection or
 *    richer handler signature is needed at all.
 *  - Each record's own edit-form title always reads "Target edit" in real
 *    AM4 (a fixed header, distinct from the list row's own per-record
 *    name); this port reuses the per-record name for both the list row
 *    *and* the header — cosmetic simplification, not a functional gap.
 *  - `targetsMenu` doesn't carry its own inline "<Back" option (AM4's
 *    `TargetMenu targetsMenu("Targets",dataSz,"<Back",...)` overload) —
 *    `CArrayBody<T,arr,N>` is a fixed-shape homogeneous array; adding one
 *    extra heterogeneous item would need a different Body entirely. Not
 *    needed anyway: `mainMenu`'s own `EXIT("<Back")` already lets Esc
 *    leave `targetsMenu` and return, same reachability, one Esc away.
 *
 * The selftest drives nav.up()/down()/enter()/esc() directly, same style
 * as every other port in this compat layer.
 */

#include <menu.h>
#include <oneMenu/menu/IO/ansiOut.h>
#include <oneMenu/menu/fmt/textFmt.h>
#include <oneMenu/menu/fmt/ansiFmt.h>
#include <oneMenu/menu/IO/streamOut.h>
#include <oneMenu/menu/in.h>
#include <oneMenu/menu/body/cArrayBody.h>
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

constexpr int dataSz = 3;
constexpr int nameSz = 20;

// ── the real records (myTargets[]) — a genuine struct, copied by value on
// Enter/Save; no NTTP address of any of ITS members is ever taken, so it
// doesn't hit the member-address restriction the flattened scratch below
// exists to avoid. ────────────────────────────────────────────────────────
// NOTE: named TargetRecord, not Data — `using namespace oneData;` (above)
// already brings oneData::Data<T> into scope; a same-named struct here
// would be ambiguous at every unqualified use.
struct TargetRecord {
  char name[nameSz+1] = "<name>";
  bool valid = false;
  int selTest = 0;      // SelTest: 0=Zero,1=One,2=Two
  int chooseTest = 1;   // ChooseTest: 1=First,2=Second,3=Third,-1=Last
};
TargetRecord myTargets[dataSz];

// ── the shared scratch (AM4's `Data target;`, here `TargetRecord`) — flattened, see file header
// comment. Every EDIT()/TOGGLE()/SELECT()/CHOOSE() below binds to these the
// ordinary way (plain top-level globals), no DataFn/DataRef distinction
// needed anywhere in this file.
char targetName[nameSz+1] = "";
bool targetValid = false;
int targetSelTest = 0;
int targetChooseTest = 1;

int lastEditedIdx = -1;

// ── CopyInOnEnter: fires exactly once, right as THIS array slot's own
// Enter opens its edit-form submenu (path.len==0) — copies myTargets[idx]
// into the flattened scratch before the submenu's own fields (bound to the
// scratch globals) are ever read/edited, and stashes idx for saveTarget.
struct CopyInOnEnter {
  template<typename I>
  struct Part : I {
    using Base = I;
    int idx{};
    template<typename... OO>
    constexpr Part(int i, OO&&... oo) : Base{std::forward<OO>(oo)...}, idx(i) {}

    template<bool isKbd, typename Nav>
    bool nav(Nav& n, const oneMenu::CKE& cke, const oneMenu::Path& path) {
      if(cke.cmd==oneMenu::Cmd::Enter && !path.len) {
        strncpy(targetName, myTargets[idx].name, nameSz);
        targetName[nameSz] = '\0';
        targetValid = myTargets[idx].valid;
        targetSelTest = myTargets[idx].selTest;
        targetChooseTest = myTargets[idx].chooseTest;
        lastEditedIdx = idx;
      }
      return Base::template nav<isKbd>(n, cke, path);
    }
  };
};

bool saveTarget(int) {
  strncpy(myTargets[lastEditedIdx].name, targetName, nameSz);
  myTargets[lastEditedIdx].name[nameSz] = '\0';
  myTargets[lastEditedIdx].valid = targetValid;
  myTargets[lastEditedIdx].selTest = targetSelTest;
  myTargets[lastEditedIdx].chooseTest = targetChooseTest;
  return true;
}

// characters allowed on the Name field — single shared entry, cycled via
// CharMask::PosSet's own pos%N design (am4.h's EDIT() doc comment).
static const char* alphaNum = " 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,+-";
static const char* alphaNumMask[] = {alphaNum};

void noOp() {}

// ── the shared edit-form shape, rebuilt fresh per array slot — TOGGLE/
// SELECT/CHOOSE expand to local `auto id = ...;` declarations (fine inside
// a function body), then SUBMENU() moves them into the returned body. All 3
// calls produce byte-for-byte the SAME type (all fields bind to the SAME
// scratch globals), which is exactly what CArrayBody<T,arr,N> needs — T
// must be one type shared by all N array slots.
auto makeTargetEditBody() {
  TOGGLE(targetValid, editValid, "Valid: ", Menu::doNothing, Menu::noEvent, Menu::noStyle
    ,VALUE("On", true, Menu::doNothing, Menu::noEvent)
    ,VALUE("Off", false, Menu::doNothing, Menu::noEvent)
  );
  SELECT(targetSelTest, selMenu, "Select", Menu::doNothing, Menu::noEvent, Menu::noStyle
    ,VALUE("Zero", 0, Menu::doNothing, Menu::noEvent)
    ,VALUE("One",  1, Menu::doNothing, Menu::noEvent)
    ,VALUE("Two",  2, Menu::doNothing, Menu::noEvent)
  );
  CHOOSE(targetChooseTest, chooseMenu, "Choose", Menu::doNothing, Menu::noEvent, Menu::noStyle
    ,VALUE("First",  1, Menu::doNothing, Menu::noEvent)
    ,VALUE("Second", 2, Menu::doNothing, Menu::noEvent)
    ,VALUE("Third",  3, Menu::doNothing, Menu::noEvent)
    ,VALUE("Last",  -1, Menu::doNothing, Menu::noEvent)
  );
  return oneMenu::staticBody(
    EDIT("Name", targetName, alphaNumMask, noOp, Menu::noEvent, Menu::noStyle)
    ,SUBMENU(editValid)
    ,SUBMENU(selMenu)
    ,SUBMENU(chooseMenu)
    ,OP("Save", saveTarget, Menu::enterEvent)
    ,EXIT("<Back")
  );
}

// Menu<T,B,...>'s T (title) must itself be a fully composed chain (e.g.
// ItemDef<Text>), not the bare component descriptor Text — same shape
// every MENU()/menuDef<>() call site already uses.
using TargetTitle = oneMenu::ItemDef<Text>;
using TargetEditBody = decltype(makeTargetEditBody());
using TargetItem = oneMenu::ItemDef<CopyInOnEnter, oneMenu::Menu<TargetTitle, TargetEditBody>>;

TargetItem targetItems[dataSz] = {
  TargetItem{0, TargetTitle{myTargets[0].name}, makeTargetEditBody()},
  TargetItem{1, TargetTitle{myTargets[1].name}, makeTargetEditBody()},
  TargetItem{2, TargetTitle{myTargets[2].name}, makeTargetEditBody()},
};

using TargetsBody = oneMenu::CArrayBody<TargetItem, targetItems, dataSz>;
auto targetsMenu = oneMenu::menuDef<>(
  oneMenu::ItemDef<Text>{"Targets"},
  TargetsBody{}
);

// ── OBJ() splices the hand-declared targetsMenu into a real MENU() body —
// same mechanism this compat layer already proved (am4compat, 2026-07-09).
MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OBJ(targetsMenu)
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
MENU_OUTPUTS(out, /*maxDepth*/4, &devOut);
NAVROOT(nav, mainMenu, /*maxDepth*/4, in, out);

int main() {
  devOut.lockMode(oneMenu::LockMode::None);
  devOut.setColors(WHITE, BLACK);
  devOut.clear();
  nav.printTo(devOut);

  strcpy(myTargets[0].name, "Alpha");
  myTargets[0].valid = true;
  myTargets[0].selTest = 1;
  myTargets[0].chooseTest = 2;
  strcpy(myTargets[1].name, "Beta");
  myTargets[1].selTest = 2;
  strcpy(myTargets[2].name, "Gamma");

  // index 0 in mainMenu's body = targetsMenu (spliced via OBJ()), index 1 = <Back>

  // ── dynamic per-slot title: printed content must track the live buffer ──
  assert(strcmp(targetItems[0].title.get(), "Alpha")==0);
  strcpy(myTargets[0].name, "Renamed");
  assert(strcmp(targetItems[0].title.get(), "Renamed")==0 &&
         "TargetTitle bound to a live char* must reflect buffer mutations, not a snapshot");
  strcpy(myTargets[0].name, "Alpha");

  // ── entering targetsMenu, then a record, must copy that record's fields
  // into the flattened scratch (CopyInOnEnter) ─────────────────────────
  nav.enter();  // opens targetsMenu (level 0 -> 1)
  assert(nav.level() == 1 && "OBJ()'d targetsMenu did not open");
  assert(targetName[0]=='\0' || strcmp(targetName,"Alpha")!=0);
  nav.enter();  // opens record 0's own edit form (level 1 -> 2) -> CopyInOnEnter fires
  assert(nav.level() == 2 && "record 0's own submenu did not open");
  assert(strcmp(targetName, "Alpha")==0 && "CopyInOnEnter did not copy myTargets[0].name");
  assert(targetValid == true && targetSelTest == 1 && targetChooseTest == 2 &&
         "CopyInOnEnter did not copy myTargets[0]'s other fields");

  // ── edit the scratch's Name field (EDIT(), single-entry validator cycling,
  // same PosSet pos%N shape already proven elsewhere in this compat layer) ──
  nav.enter();  // opens edit mode on Name (index 0 inside the edit form)
  assert(nav.navMode() == oneMenu::NavMode::Edit);
  nav.down();   // cycle position 0's char
  assert(targetName[0] != 'R' && "EDIT()'s validator did not cycle position 0");
  nav.esc();
  assert(nav.navMode() != oneMenu::NavMode::Edit);

  // ── TOGGLE(editValid): Enter cycles to the next VALUE() ─────────────────
  nav.up();     // Name(0) -> editValid(1)
  bool validBefore = targetValid;
  nav.enter();
  assert(targetValid != validBefore && "TOGGLE() did not cycle targetValid");

  // ── Save: writes the (now-edited) scratch back into myTargets[0]
  // specifically, via lastEditedIdx — not myTargets[1]/[2]. ───────────────
  nav.up(); nav.up(); nav.up();  // editValid(1) -> selMenu(2) -> chooseMenu(3) -> Save(4)
  bool savedValid = targetValid;
  char savedName0 = targetName[0];
  nav.enter();  // fires saveTarget
  assert(myTargets[0].valid == savedValid && myTargets[0].name[0] == savedName0 &&
         "Save did not write the edited scratch back to myTargets[0] via lastEditedIdx");
  assert(myTargets[1].name[0]=='B' && myTargets[1].valid==false &&
         "Save must not touch a different record");

  nav.esc();  // leave record 0's edit form (level 2 -> 1)
  assert(nav.level() == 1);

  // ── a DIFFERENT record (index 1) must independently copy ITS OWN fields —
  // proves each CArrayBody slot is genuinely indexed, not aliased. ────────
  nav.up();     // record 0 -> record 1 (Cmd::Up increments, same convention
                 // as every other example in this compat layer)
  nav.enter();  // opens record 1's edit form -> CopyInOnEnter fires with idx=1
  assert(nav.level() == 2);
  assert(strcmp(targetName, "Beta")==0 && targetSelTest == 2 &&
         "record 1's CopyInOnEnter did not read myTargets[1], got a stale/aliased value");
  nav.esc();
  assert(nav.level() == 1);

  nav.esc();  // leave targetsMenu back to mainMenu
  assert(nav.level() == 0 && "esc() did not close back out of the OBJ()'d targetsMenu");

  printf("OK: targetSel.ino compat-macro port (CArrayBody + per-slot copy-in/Save-writeback) verified\n");
  return 0;
}
