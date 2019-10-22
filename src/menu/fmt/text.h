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
    static inline void fmtPanel(Nav& nav,Idx n=0) {
      // if (io) Out::nl();
      // Out::raw("*--------*");
      // Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtMenu(Nav& nav,Idx n=0) {
      // Out::raw("----------");
      // Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtBody(Nav& nav,Idx n=0) {
      // Out::raw(io?"| ":" |");
      // Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtName(Nav& nav,Idx n=0) {
      Out::raw(io?"«":"»");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtValue(Nav& nav,Idx n=0) {
      // Out::raw(io?"{":"}");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtUnit(Nav& nav,Idx n=0) {
      // Out::raw(io?"(":")");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtMode(Nav& nav,Idx n=0) {
      if(io) switch(Nav::mode()) {
        case Modes::Normal: Out::raw(' ');break;
        case Modes::Edit: Out::raw(':');break;
        case Modes::Tune: Out::raw('>');break;
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtTitle(Nav& nav,Idx n=0) {
      // if (!io) Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtItem(Nav& nav,Idx n=0) {
      if (!io) Out::nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtIndex(Nav& nav,Idx n=0) {
      if(io) {
        if (n<9) Out::raw(n+1);
        else Out::raw(' ');
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtCursor(Nav& nav,Idx n=0) {
      if (io) Out::raw(nav.selected(n)?((nav.enabled(n)?'>':'-')):' ');
    }
  public:
    template<Roles r,bool io,typename I,typename Out,typename Nav>
    static inline void fmt(Nav& nav,Idx n=0) {
      if (!io) O::template fmt<r,io,I,Out,Nav>(nav,n);
      switch(r) {
        case Roles::Panel:  Out::template fmtPanel <io,I,Out,Nav>(nav,n);break;
        case Roles::Menu:   Out::template fmtMenu  <io,I,Out,Nav>(nav,n);break;
        case Roles::Title:  Out::template fmtTitle <io,I,Out,Nav>(nav,n);break;
        case Roles::Body:   Out::template fmtBody  <io,I,Out,Nav>(nav,n);break;
        case Roles::Prompt: Out::template fmtItem  <io,I,Out,Nav>(nav,n);break;
        case Roles::Index:  Out::template fmtIndex <io,I,Out,Nav>(nav,n);break;
        case Roles::Cursor: Out::template fmtCursor<io,I,Out,Nav>(nav,n);break;
        case Roles::Name:   Out::template fmtName  <io,I,Out,Nav>(nav,n);break;
        case Roles::Mode:   Out::template fmtMode  <io,I,Out,Nav>(nav,n);break;
        case Roles::Value:  Out::template fmtValue <io,I,Out,Nav>(nav,n);break;
        case Roles::Unit:   Out::template fmtUnit  <io,I,Out,Nav>(nav,n);break;
        default:break;
      }
      if (io) O::template fmt<r,io,I,Out,Nav>(nav,n);
    }
};
