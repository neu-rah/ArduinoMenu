/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 SDCard.ino compat-macro port — a real file browser, not just a
 *        flat cached list: `CachedSDMenu<32>` spliced into `mainMenu` via
 *        `SUBMENU(filePickMenu)`. See the original at
 *        github.com/neu-rah/ArduinoMenu examples/SDCard/SDCard/SDCard.ino.
 *
 * AM4's real mechanism (`src/plugin/SDMenu.h`: `CachedFSO<SDC,maxSz>` +
 * `SDMenuT<FS>`) is ported here not by translating that class directly, but
 * by composing three new, backing-store-agnostic OneMenu-core pieces on top
 * of the already-shipped, unmodified `CacheBody`/`CArrayBody`/`JoinBody`:
 * `oneMenu::FileEntryBody` (the windowed, folder-aware entry listing),
 * `oneMenu::BackNav`/`FolderEsc` (the "[..]" row and the real Esc key both
 * routing through one shared pop-a-folder-or-close-the-level decision), and
 * the `fileBrowserDef<...>()` factory tying them together — see
 * `OneMenu/include/oneMenu/menu/body/fileBrowser.h`.
 *
 * `filePickMenu` is built via `fileBrowserDef<...>()` directly (a native
 * call, not a macro — this is a pure OneMenu-core capability, not something
 * am4.h's own macro layer knows about) and spliced into `mainMenu` via the
 * ordinary `SUBMENU(id)` macro, which is just `std::move(id)` — works with
 * any moveable `ItemDef<Menu<...>>`, not only ones built by `MENU()`.
 *
 * Real backing store: this native selftest uses a small in-memory
 * hierarchical `FakeFsBacking` (root: a.txt, sub/; sub/: b.txt, c.txt) — a
 * real SD-card `Backing` (Arduino's own bundled `SD` library) is proven
 * separately, see `.RnD/AM4check/SDCard`.
 *
 * Disclosed simplification (not a bug): AM4's real `entryIdx()`-based
 * reselection re-finds and highlights the folder you just popped out of;
 * this port always lands on index 0 after any folder change (both
 * descending and popping) — same "disclose rather than chase 1:1 fidelity"
 * precedent as this compat layer's other ports.
 */

#include <menu.h>
#include <oneMenu/menu/body/fileBrowser.h>
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

constexpr oneMenu::Sz MaxSz = 4;
char nameBuf[MaxSz][16];
oneMenu::ItemDef<Text> fileItems[MaxSz] = {
  oneMenu::ItemDef<Text>{nameBuf[0]}, oneMenu::ItemDef<Text>{nameBuf[1]},
  oneMenu::ItemDef<Text>{nameBuf[2]}, oneMenu::ItemDef<Text>{nameBuf[3]},
};

struct Entry { const char* name; bool dir; };
constexpr Entry rootEntries[] = {{"a.txt",false},{"sub",true}};
constexpr Entry subEntries[]  = {{"b.txt",false},{"c.txt",false}};

// In-memory hierarchical Backing, standing in for a real SD directory —
// same count()/populate() contract as CacheBody's own, plus atRoot()/
// descend()/up()/currentFolder() for FileEntryBody's folder logic.
struct FakeFsBacking {
  inline static char folder[64] = "/";

  static const Entry* entries(oneMenu::Sz& n) {
    if(strcmp(folder,"/sub/")==0) { n=2; return subEntries; }
    n=2; return rootEntries;
  }
  static oneMenu::Sz count() { oneMenu::Sz n; entries(n); return n; }
  static void populate(oneMenu::Sz idx, oneMenu::ItemDef<Text>& slot) {
    oneMenu::Sz n; const Entry* e = entries(n);
    oneMenu::Sz bufIdx = oneMenu::Sz(&slot - fileItems);
    if(idx<n) snprintf(nameBuf[bufIdx],16,"%s%s",e[idx].name,e[idx].dir?"/":"");
    else nameBuf[bufIdx][0]=0;
    slot.set(nameBuf[bufIdx]);
  }
  static bool atRoot() { return strcmp(folder,"/")==0; }
  static void descend(const char* name) {
    strncat(folder,name,sizeof(folder)-strlen(folder)-1);
  }
  static void up() { strcpy(folder,"/"); }
  static const char* currentFolder() { return folder; }
};

namespace action {
  bool noOp(int) { return false; }  // OP()'s slot needs bool(int)
  char pickedFolder[64]{};
  char pickedFile[64]{};
  int pickCount = 0;
  void filePick(const char* folder,const char* file) {
    strncpy(pickedFolder,folder,63);
    strncpy(pickedFile,file,63);
    pickCount++;
  }
}

auto filePickMenu = oneMenu::fileBrowserDef<oneMenu::ItemDef<Text>,fileItems,MaxSz,
  FakeFsBacking,action::filePick>(oneMenu::ItemDef<Text>{"SD Card"});

MENU(mainMenu, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,SUBMENU(filePickMenu)
  ,OP("Something else...", action::noOp, Menu::noEvent)
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
  nav.printTo(devOut);

  assert(nav.level()==0);

  // index 0 = filePickMenu (SUBMENU), 1 = "Something else...", 2 = "<Back"
  nav.enter(); // open the SD Card browser
  assert(nav.level()==1 && "entering the file browser opens a level");
  assert(FakeFsBacking::atRoot());

  // sel 0="[..]", sel 1="a.txt", sel 2="sub/"
  nav.up(); nav.up(); // sel -> 2 ("sub/")
  assert(nav.sel()==2);
  nav.enter(); // descend into sub/
  assert(nav.level()==1 && "descending a folder stays at the same nav level");
  assert(!FakeFsBacking::atRoot());
  assert(strcmp(FakeFsBacking::folder,"/sub/")==0);
  assert(nav.sel()==0);

  // sel 0="[..]", sel 1="b.txt", sel 2="c.txt" — pick b.txt
  nav.up(); // sel -> 1 ("b.txt")
  nav.enter();
  assert(action::pickCount==1 && strcmp(action::pickedFile,"b.txt")==0);
  assert(strcmp(action::pickedFolder,"/sub/")==0);
  assert(nav.level()==1 && "picking a file only pops one folder, doesn't exit the browser");
  assert(FakeFsBacking::atRoot());

  // a real Esc key at root falls through to a genuine level-close, exiting
  // the file browser and returning to mainMenu.
  nav.esc();
  assert(nav.level()==0 && "Esc at root exits the file browser back to mainMenu");

  printf("OK: SDCard.ino compat-macro port (FileEntryBody/BackNav/FolderEsc "
         "folder browsing over a real SD card proven separately, see "
         ".RnD/AM4check/SDCard) verified\n");
  return 0;
}
