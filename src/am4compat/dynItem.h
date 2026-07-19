/**
 * @file dynItem.h
 * @author Rui Azevedo (ruihfazevedo@gmail.com)
 * @brief Shared AM4-compat virtual item family (program-side — not
 *        OneMenu-side) for any port needing a runtime-sized, heterogeneous
 *        item body. Lives in ArduinoMenu's own src/ (this library's real
 *        include root — every example here already sees it, no per-example
 *        copy needed), promoted here once a second real caller (`reuse.ino`,
 *        alongside the original `dynamic.ino`) needed the same types.
 *        Originally built unblocking `dynamic.ino` — see
 *        github.com/neu-rah/ArduinoMenu examples/dynamic/dynamic/dynamic.ino.
 *
 * A prior attempt to port this onto OneMenu's IItemDef<Menu<...>> hit a real
 * C++ wall: Menu::Part::printMenu/StaticBody::printBody call TEMPLATE methods
 * on Out (out.printMenu<Item>/out.printItem<Item>) — a template method can't
 * be virtual, so IOut can't expose them, and the moment a Menu-shaped item is
 * wrapped for type-erasure this fails to compile. Fix: the types below derive
 * from oneMenu::IItem directly and implement printMenu/printBody/printItem by
 * hand, driving only IOut's already-virtual primitives (fmtStart/fmtStop/put)
 * — mirroring what Menu::Part::printMenu + printers.h's FullPrinter chain do,
 * but written once, directly, instead of going through the templated OutAPI
 * contract at all. See notes.md/RESULTS.md "dynamic.ino" for the full history.
 *
 * A second, independent wall surfaced while building this: oneMenu::INav
 * (nav.h) only exposed read-only getters (level()/sel()/navMode()) — nothing
 * a type-erased item could call to actually open/close a level or move a
 * selection. That gap was real OneMenu core machinery missing (not a program
 * -side workaround), so — with explicit sign-off — INav/INavDef (nav.h) and
 * am4compat::NavRootDef (am4.h) gained four new forwarding virtuals (open/
 * close/padOpen/doNav), additive only, mirroring the idling()/idleOn()/
 * idleOff() pattern already there. Everything else in this file is ordinary
 * program-side code — no other OneMenu file is touched.
 *
 * Scope, deliberately: no scroll-window search, no index numbering, no nav-
 * cursor positioning (ScrollBodyPrinter/IndexPrinter/NavCursorPrinter's own
 * jobs) — IOut doesn't expose getPos()/free() to a type-erased caller, so
 * this is a plain sequential, changed()-gated redraw walk only. Every real
 * device already has to support that fallback path.
 */
#pragma once

#include <oneMenu/menu/item.h>
#include <oneMenu/menu/out.h>
#include <oneMenu/menu/nav.h>
#include <string.h>

namespace am4compat {

  enum SysStyle : uint8_t {
    sNone = 0,
    sCanNav = 1<<0,
    sParentDraw = 1<<1,
    sAsPad = 1<<2,
    sExit = 1<<3,
    sWraps = 1<<4,
    sToggleImmediate = 1<<5,   // DynEnumItem: Enter cycles+commits directly (Toggle),
                               // vs. Enter opens a pick-list (Select/Choose)
  };

  /// @brief models AM4's plain `prompt`/`Exit`/`menuNode` (submenu, pad) —
  /// the wall-breaking pair (printMenu/printBody) is hand-written against
  /// IOut, never calling out.printMenu(...)/out.printItem(...).
  template<oneMenu::Depth MaxDepth = 2>
  struct DynItem : oneMenu::IItem {
    const char* text;
    oneMenu::EventMask mask{oneMenu::EventMask::None};
    oneMenu::EventFuncItemPtr fn{nullptr};
    uint8_t style{sNone};
    bool enabledFlag{true};
    bool dirty{true};                       // mirrors AM4's own prompt::dirty 1:1
    oneMenu::IItem* const* items{nullptr};   // nullptr = leaf; matches prompt::sz()==0
    oneMenu::Sz itemCount{0};

    constexpr DynItem(const char* t, uint8_t st = sNone,
                       oneMenu::EventMask m = oneMenu::EventMask::None,
                       oneMenu::EventFuncItemPtr f = nullptr,
                       oneMenu::IItem* const* body = nullptr, oneMenu::Sz n = 0)
      : text(t), mask(m), fn(f), style(st), items(body), itemCount(n) {}

    static constexpr oneMenu::Depth depth() {return MaxDepth;}
    oneMenu::Sz size() const {return itemCount;}

    bool enabled() const override {return enabledFlag;}
    void enable(bool e = true) override {enabledFlag = e; dirty = true;}
    bool changed() const override {return dirty;}
    void sync() override {dirty = false;}
    void sync(oneMenu::IOut&) override {dirty = false;}
    bool up() const override {return false;}
    bool down() const override {return false;}
    bool onEvent(oneMenu::EventMask e) override {
      return (fn && (e & mask)) ? fn(e, *this) : false;
    }

    // ---- nav: mirrors Menu::Part::nav (menu.h) translated to a raw
    // IItem*[] body instead of a compile-time Body — now that INav exposes
    // open()/close()/padOpen()/doNav(), a type-erased item can drive them
    // directly. _nav/_kbdNav share one impl, parameterized on isKbd only to
    // preserve it across the recursive child call (losing it there would
    // silently break IndexGo-style digit-jump for any DynItem subtree —
    // not needed by dynamic.ino, but free to keep correct here).
  private:
    template<bool isKbd>
    bool navImpl(oneMenu::INav& n, const oneMenu::CKE& cke, oneMenu::Path p) {
      if (p.len > 0) {
        // Always offer the command to the currently-selected child first,
        // regardless of command type — unlike Menu::Part::nav (menu.h),
        // which only forwards non-Enter commands while the real
        // NavMode::Edit is set. DynNumItem/DynTextItem manage their own
        // `editing` flag as local state (INav exposes no navMode(NavMode)
        // *setter*, only the getter, so a type-erased field can't drive the
        // real edit-mode switch) — gating on n.navMode() here would mean
        // Up/Down while "locally editing" never reach the child at all.
        // Safe: any child with nothing to do with a given command already
        // returns false (its own base case), so the top-level
        // selection-move fallback below still fires exactly when nothing
        // else claimed the command.
        bool r = false;
        if (items && p.sel() < itemCount)
          r = items[p.sel()]->template nav<isKbd>(n, cke, p.next());
        if (!r && p.len == 1)
          r = n.doNav(cke, itemCount, (style & sWraps) != 0) ||
              (cke.cmd == oneMenu::Cmd::Enter && n.close());
        return r;
      }
      if (cke.cmd == oneMenu::Cmd::Enter) {
        if (style & sExit) return n.close();
        if (items) return (style & sAsPad) ? n.padOpen() : n.open();
        // Plain leaf: fire onEvent(Enter) directly rather than relying on
        // EventDispatch (nav.h) — EventDispatch::fireAt walks root().body,
        // a member StaticBody-shaped trees have and this raw IItem*[] tree
        // doesn't, so composing EventDispatch into a DynItem-rooted nav
        // chain would fail to compile. Self-contained instead: no separate
        // nav component needed, no further OneMenu change either.
        onEvent(oneMenu::EventMask::Enter);
        return true;
      }
      return false;
    }
  public:
    bool _nav(oneMenu::INav& n, const oneMenu::CKE& cke, const oneMenu::Path p) override {
      return navImpl<false>(n, cke, p);
    }
    bool _kbdNav(oneMenu::INav& n, const oneMenu::CKE& cke, const oneMenu::Path p) override {
      return navImpl<true>(n, cke, p);
    }

    // ---- print: the wall-breaking pair. Mirrors Menu::Part::printMenu's
    // own pAt/at depth gate (menu.h) — only the currently-open path down
    // the tree is ever drawn per frame — plus a simplified version of
    // printers.h's ItemPrinter (per-row idx/selection/lockMode gating,
    // Fmt::Item/Body/Title wrapping) written directly against IOut.
    void printItem(oneMenu::IOut& out, oneMenu::Ctx&) override {out.put(text);}

    bool printMenu(oneMenu::IOut& out, oneMenu::Ctx& ctx) override {
      if (ctx.pAt > ctx.at) {
        if (!items || ctx.sel() >= itemCount) return false;
        oneMenu::Ctx tmp{ctx.path, ctx.mode, ctx.pAt, ctx.enabled, ctx.tops,
                         (oneMenu::Depth)(ctx.at+1), ctx.prev, ctx.pad, 0, ctx.idx};
        return items[ctx.sel()]->printMenu(out, tmp);
      }
      ctx.at++;
      out.fmtStart(oneMenu::Fmt::Title, ctx);
      printItem(out, ctx);
      out.fmtStop(oneMenu::Fmt::Title, ctx);
      bool r = changed();
      if (items) r = printBody(out, ctx) || r;
      return r;
    }

    bool printBody(oneMenu::IOut& out, oneMenu::Ctx& ctx) override {
      ctx.idx = 0;
      out.fmtStart(oneMenu::Fmt::Body, ctx);
      bool r = false;
      for (oneMenu::Sz i = 0; i < itemCount; i++) {
        oneMenu::IItem* child = items[i];
        oneMenu::LockMode om = out.lockMode();
        bool sel = (ctx.idx == ctx.sel());
        if (om != oneMenu::LockMode::Update || child->changed() || sel) {
          ctx.enabled = child->enabled();
          out.fmtStart(oneMenu::Fmt::Item, ctx);
          child->printItem(out, ctx);
          out.fmtStop(oneMenu::Fmt::Item, ctx);
          r = child->changed() || r;
          if (out.lockMode() == oneMenu::LockMode::Sync) child->sync();
        }
        ctx.idx++;
      }
      out.fmtStop(oneMenu::Fmt::Body, ctx);
      return r;
    }
  };

  /// @brief models AM4's `menuValue<T>` — a named option leaf (e.g. one
  /// durData[]/fxData[]/togData[] entry). Always a leaf: selection is the
  /// *parent* DynEnumItem's job, not this type's own.
  template<typename T>
  struct DynValueOption : oneMenu::IItem {
    const char* text;
    T value;
    constexpr DynValueOption(const char* t, T v) : text(t), value(v) {}

    bool enabled() const override {return true;}
    void enable(bool) override {}
    bool changed() const override {return true;}
    void sync() override {}
    void sync(oneMenu::IOut&) override {}
    bool up() const override {return false;}
    bool down() const override {return false;}
    bool onEvent(oneMenu::EventMask) override {return false;}
    bool _nav(oneMenu::INav&, const oneMenu::CKE&, const oneMenu::Path) override {return false;}
    bool _kbdNav(oneMenu::INav& n, const oneMenu::CKE& cke, const oneMenu::Path p) override {return _nav(n, cke, p);}
    void printItem(oneMenu::IOut& out, oneMenu::Ctx&) override {out.put(text);}
    bool printMenu(oneMenu::IOut& out, oneMenu::Ctx& ctx) override {printItem(out, ctx); return false;}
    bool printBody(oneMenu::IOut&, oneMenu::Ctx&) override {return false;}
  };

  /// @brief models AM4's `choose<T>`/`select<T>`/`toggle<T>`. `target` binds
  /// the external variable; `options` is the DynValueOption<T>*[] (same
  /// shape as durData/fxData/togData). sToggleImmediate picks which real
  /// AM4 nav shape this is: Toggle cycles+commits directly on Enter, no
  /// level change; Select/Choose open a pick-list (n.padOpen(), a genuine
  /// TreeNav level bump) — Up/Down then move `n`'s own real selection state
  /// at that new depth via n.doNav() (same primitive DynItem's own navImpl
  /// uses, and the SAME storage TreeNav::Part::sel()/Ctx::sel() already
  /// read elsewhere) rather than a separate member here — a second Enter
  /// reads n.sel() to know which option was highlighted, commits its value,
  /// and n.close()s. Overrides printMenu/printBody itself (not DynItem's
  /// items-based ones) since "my body" here is the options list, a
  /// different concrete type (DynValueOption<T>, not IItem*).
  ///
  /// Disclosed simplification: padOpen() always resets the new level's
  /// selection to index 0 (its own hardcoded behavior, nav.h) — a freshly
  /// opened pick-list always highlights option 0 first, not necessarily
  /// *target's current value's index. Real AM4 opens showing the current
  /// choice highlighted; matching that would need a direct "set index i"
  /// nav primitive INav doesn't expose (only relative doNav() moves), so
  /// this is accepted as-is rather than working around it with a repeated-
  /// doNav walk.
  template<typename T, oneMenu::Depth MaxDepth = 1>
  struct DynEnumItem : DynItem<MaxDepth> {
    using Base = DynItem<MaxDepth>;
    T* target;
    DynValueOption<T>* const* options;
    oneMenu::Sz optCount;

    constexpr DynEnumItem(const char* t, T* tgt, DynValueOption<T>* const* opts, oneMenu::Sz n,
                           uint8_t st = sCanNav | sParentDraw,
                           oneMenu::EventMask m = oneMenu::EventMask::None,
                           oneMenu::EventFuncItemPtr f = nullptr)
      : Base(t, st, m, f), target(tgt), options(opts), optCount(n) {}

    // Current choice's index, found by value each time (target may change
    // externally between frames) — matches AM4's own choose<T>::selNr(v).
    oneMenu::Sz findSel() const {
      for (oneMenu::Sz i = 0; i < optCount; i++) if (options[i]->value == *target) return i;
      return 0;
    }

    void printItem(oneMenu::IOut& out, oneMenu::Ctx& ctx) override {
      out.put(this->text);
      if (optCount) options[findSel()]->printItem(out, ctx);   // show current choice inline
    }

    bool printMenu(oneMenu::IOut& out, oneMenu::Ctx& ctx) override {
      if (ctx.pAt > ctx.at) {
        // "descend" into our own options list — we ARE the node whose body
        // (at the next depth) is the options, no separate child node exists.
        ctx.at++;
        return printBody(out, ctx);
      }
      ctx.at++;
      out.fmtStart(oneMenu::Fmt::Title, ctx);
      printItem(out, ctx);
      out.fmtStop(oneMenu::Fmt::Title, ctx);
      return this->changed();
    }

    bool printBody(oneMenu::IOut& out, oneMenu::Ctx& ctx) override {
      ctx.idx = 0;
      out.fmtStart(oneMenu::Fmt::Body, ctx);
      for (oneMenu::Sz i = 0; i < optCount; i++) {
        out.fmtStart(oneMenu::Fmt::Item, ctx);
        options[i]->printItem(out, ctx);
        out.fmtStop(oneMenu::Fmt::Item, ctx);
        ctx.idx++;
      }
      out.fmtStop(oneMenu::Fmt::Body, ctx);
      return false;
    }

    // p.len==0: not yet open (or, for Toggle/immediate, never opens) —
    // decide whether to open. p.len>0: we ARE open, the path element in
    // front of us is our own pick-list's live selection (n.sel()/n.doNav(),
    // the same storage Ctx::sel() reads at print time).
    bool _nav(oneMenu::INav& n, const oneMenu::CKE& cke, const oneMenu::Path p) override {
      bool immediate = this->style & sToggleImmediate;
      if (p.len == 0) {
        if (cke.cmd == oneMenu::Cmd::Enter) {
          if (immediate) {
            if (optCount) {
              oneMenu::Sz i = (findSel()+1) % optCount;
              *target = options[i]->value;
            }
            this->dirty = true;
            return true;
          }
          this->dirty = true;
          return (this->style & sAsPad) ? n.padOpen() : n.open();
        }
        return false;
      }
      if (cke.cmd == oneMenu::Cmd::Enter) {
        if (optCount) *target = options[n.sel()]->value;
        this->dirty = true;
        return n.close();
      }
      return n.doNav(cke, optCount, (this->style & sWraps) != 0);
    }
    bool _kbdNav(oneMenu::INav& n, const oneMenu::CKE& cke, const oneMenu::Path p) override {return _nav(n, cke, p);}
  };

  /// @brief models AM4's `menuField<T>` (numeric, e.g. `test`/`year`/
  /// `month`/`day`). Always a leaf (MaxDepth=0 default) — edit mode is
  /// local state (`editing`), never a real nav level push.
  template<typename T, oneMenu::Depth MaxDepth = 0>
  struct DynNumItem : DynItem<MaxDepth> {
    using Base = DynItem<MaxDepth>;
    T* target;
    T low, high, step;
    bool editing{false};

    constexpr DynNumItem(const char* t, T* tgt, T lo, T hi, T st,
                          uint8_t sty = sNone,
                          oneMenu::EventMask m = oneMenu::EventMask::None,
                          oneMenu::EventFuncItemPtr f = nullptr)
      : Base(t, sty, m, f), target(tgt), low(lo), high(hi), step(st) {}

    void printItem(oneMenu::IOut& out, oneMenu::Ctx&) override {
      out.put(this->text);
      // IOut::put(int)/put(double) — an unqualified `*target` (uint8_t/
      // uint16_t/etc.) is an equally-good implicit conversion to either,
      // hence ambiguous; explicit int cast (every real dynamic.ino field —
      // test/year/month/day — is integral, no float DynNumItem usage here).
      out.put((int)*target);
    }

    bool _nav(oneMenu::INav&, const oneMenu::CKE& cke, const oneMenu::Path p) override {
      if (p.len > 0) return false;
      if (cke.cmd == oneMenu::Cmd::Enter) {editing = !editing; this->dirty = true; return true;}
      if (editing && cke.cmd == oneMenu::Cmd::Esc) {editing = false; this->dirty = true; return true;}
      if (editing) {
        // NumField's own edit-mode Up/Down inversion (item.h) — Up steps
        // DOWN, Down steps UP; re-derived empirically, not assumed, see
        // notes.md's own repeated caution about this exact direction.
        if (cke.cmd == oneMenu::Cmd::Up   && *target > low)        {*target = (T)(*target - step); this->dirty = true; return true;}
        if (cke.cmd == oneMenu::Cmd::Down && *target + step <= high) {*target = (T)(*target + step); this->dirty = true; return true;}
      }
      return false;
    }
    bool _kbdNav(oneMenu::INav& n, const oneMenu::CKE& cke, const oneMenu::Path p) override {return _nav(n, cke, p);}
  };

  /// @brief models AM4's `textField` (masked char buffer, e.g. `buf1`/
  /// `hexNr`). `validators` is a runtime `const char* const*` (one allowed-
  /// character-set string per buffer position, repeating cyclically via
  /// pos%rows — same algorithm as OneMenu's own CharMask::PosSet, but that
  /// type takes its validators array as a compile-time NTTP address
  /// (template<CText* V,int N>), which a single reusable runtime DynTextItem
  /// type can't use (every real textField instance has its own distinct
  /// validators table) — so the tiny find/up/down scan is re-derived here
  /// directly against a runtime pointer instead.
  template<oneMenu::Depth MaxDepth = 0>
  struct DynTextItem : DynItem<MaxDepth> {
    using Base = DynItem<MaxDepth>;
    char* buf;
    oneMenu::Sz len;
    const char* const* validators;
    oneMenu::Sz rows;
    oneMenu::Sz pos{0};
    bool editing{false};

    constexpr DynTextItem(const char* t, char* b, oneMenu::Sz l,
                           const char* const* v, oneMenu::Sz vRows,
                           uint8_t sty = sNone,
                           oneMenu::EventMask m = oneMenu::EventMask::None,
                           oneMenu::EventFuncItemPtr f = nullptr)
      : Base(t, sty, m, f), buf(b), len(l), validators(v), rows(vRows) {}

    void printItem(oneMenu::IOut& out, oneMenu::Ctx&) override {
      out.put(this->text);
      out.put(buf);
    }

    static int findCh(const char* s, unsigned char c) {
      for (int n = 0; s[n]; n++) if ((unsigned char)s[n] == c) return n;
      return -1;
    }
    void cycle(bool up) {
      const char* v = validators[pos % rows];
      int vlen = (int)strlen(v);
      if (vlen == 0) return;
      int i = findCh(v, (unsigned char)buf[pos]);
      if (up)   i = (i < 0) ? 0 : (i == vlen-1 ? 0 : i+1);
      else      i = (i < 0) ? 0 : (i == 0 ? vlen-1 : i-1);
      buf[pos] = v[i];
      this->dirty = true;
    }

    bool _nav(oneMenu::INav&, const oneMenu::CKE& cke, const oneMenu::Path p) override {
      if (p.len > 0) return false;
      if (cke.cmd == oneMenu::Cmd::Enter) {
        editing = !editing;
        if (!editing) pos = 0;
        this->dirty = true;
        return true;
      }
      if (editing && cke.cmd == oneMenu::Cmd::Esc) {editing = false; pos = 0; this->dirty = true; return true;}
      if (editing) {
        if (cke.cmd == oneMenu::Cmd::Left  && pos > 0)         {pos--; return true;}
        if (cke.cmd == oneMenu::Cmd::Right && pos+1 < len)     {pos++; return true;}
        if (cke.cmd == oneMenu::Cmd::Up)   {cycle(true);  return true;}
        if (cke.cmd == oneMenu::Cmd::Down) {cycle(false); return true;}
      }
      return false;
    }
    bool _kbdNav(oneMenu::INav& n, const oneMenu::CKE& cke, const oneMenu::Path p) override {return _nav(n, cke, p);}
  };

}
