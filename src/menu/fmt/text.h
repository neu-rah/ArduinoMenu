/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, add `\n` at title and item end, print index and text cursor
*/

template<typename O>
struct TextFmt:public O {
  public:
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtPanel(Idx n,Nav& nav) {
      // if (io) Out::nl();
      // Out::raw("*--------*");
      // Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtMenu(Idx n,Nav& nav) {
      // Out::raw("----------");
      // Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtBody(Idx n,Nav& nav) {
      // Out::raw(io?"| ":" |");
      // Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtName(Idx n,Nav& nav) {
      Out::raw(io?"«":"»");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtValue(Idx n,Nav& nav) {
      // Out::raw(io?"{":"}");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtUnit(Idx n,Nav& nav) {
      // Out::raw(io?"(":")");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtMode(Idx n,Nav& nav) {
      if(io) switch(Nav::mode()) {
        case Modes::Normal: Out::raw(' ');break;
        case Modes::Edit: Out::raw(':');break;
        case Modes::Tune: Out::raw('>');break;
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtTitle(Idx n,Nav& nav) {
      // if (!io) Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtItem(Idx n,Nav& nav) {
      if (!io) Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtIndex(Idx n,Nav& nav) {
      if(io) {
        if (n<9) Out::raw(n+1);
        else Out::raw(' ');
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtCursor(Idx n,Nav& nav) {
      if (io) Out::raw(nav.selected(n)?((nav.enabled(n)?'>':'-')):' ');
    }
  public:
    template<Roles r,bool io,typename I,typename Out,typename Nav>
    static inline void fmt(Idx n,Nav& nav) {
      if (!io) O::template fmt<r,io,I,Out,Nav>(n,nav);
      switch(r) {
        case Roles::Panel:  Out::template fmtPanel <io,I,Out,Nav>(n,nav);break;
        case Roles::Menu:   Out::template fmtMenu  <io,I,Out,Nav>(n,nav);break;
        case Roles::Title:  Out::template fmtTitle <io,I,Out,Nav>(n,nav);break;
        case Roles::Body:   Out::template fmtBody  <io,I,Out,Nav>(n,nav);break;
        case Roles::Prompt: Out::template fmtItem  <io,I,Out,Nav>(n,nav);break;
        case Roles::Index:  Out::template fmtIndex <io,I,Out,Nav>(n,nav);break;
        case Roles::Cursor: Out::template fmtCursor<io,I,Out,Nav>(n,nav);break;
        case Roles::Name:   Out::template fmtName  <io,I,Out,Nav>(n,nav);break;
        case Roles::Mode:   Out::template fmtMode  <io,I,Out,Nav>(n,nav);break;
        case Roles::Value:  Out::template fmtValue <io,I,Out,Nav>(n,nav);break;
        case Roles::Unit:   Out::template fmtUnit  <io,I,Out,Nav>(n,nav);break;
        default:break;
      }
      if (io) O::template fmt<r,io,I,Out,Nav>(n,nav);
    }
};
