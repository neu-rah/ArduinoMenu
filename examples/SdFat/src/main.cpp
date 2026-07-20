/**
 * @file main.cpp
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief AM4 SdFat.ino compat-macro port — same real file browser as
 *        `SDCard.ino` (`CachedFSO<SdFat,32>` instead of `CachedFSO<SD,32>`),
 *        spliced into `mainMenu` via `SUBMENU(filePickMenu)`. See the
 *        original at github.com/neu-rah/ArduinoMenu examples/SdFat/SdFat/
 *        SdFat.ino.
 *
 * Same mechanism as `SDCard.ino`'s own port — `oneMenu::FileEntryBody`/
 * `BackNav`/`FolderEsc` (`OneMenu/include/oneMenu/menu/body/fileBrowser.h`)
 * composed on top of the unmodified `CacheBody`/`CArrayBody`/`JoinBody` — no
 * new machinery needed. The only real difference between the two AM4
 * examples, confirmed directly against both real vendored sources
 * (`src/plugin/SDMenu.h` vs `src/plugin/SdFatMenu.h`): `SD.h`'s own
 * `File::name()` returns `const char*` directly, while `SdFat.h`'s
 * `File::getName(char* buf, size_t size)` needs a caller-supplied buffer —
 * everything else (`dir.openNextFile()`, `file.isDirectory()`,
 * `dir.rewindDirectory()`) is identical, confirming SdFat's own `File` type
 * is deliberately close to drop-in compatible with `SD.h`'s. That one
 * difference is isolated entirely to the real AVR harness's own `Backing`
 * struct (see `.RnD/AM4check/SdFat`) — this native selftest (a fake
 * in-memory backing, no real vendor library at all) is otherwise identical
 * to `SDCard.ino`'s own.
 *
 * Disclosed simplification (not a bug, same as `SDCard.ino`'s own port):
 * AM4's real `entryIdx()`-based reselection re-finds and highlights the
 * folder you just popped out of; this port always lands on index 0 after
 * any folder change (both descending and popping).
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

// In-memory hierarchical Backing, standing in for a real SdFat directory —
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

  printf("OK: SdFat.ino compat-macro port (FileEntryBody/BackNav/FolderEsc "
         "folder browsing over a real SdFat library proven separately, see "
         ".RnD/AM4check/SdFat) verified\n");
  return 0;
}
