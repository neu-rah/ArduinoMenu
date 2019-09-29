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
    static inline void fmtPanel(Idx n,Nav& nav,Out& out) {
      if (io) out.nl();
      out.raw("*--------*");
      out.nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtMenu(Idx n,Nav& nav,Out& out) {
      // out.raw("----------");
      // out.nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtBody(Idx n,Nav& nav,Out& out) {
      // out.raw(io?"| ":" |");
      // out.nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtName(Idx n,Nav& nav,Out& out) {
      out.raw(io?"«":"»");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtValue(Idx n,Nav& nav,Out& out) {
      // out.raw(io?"{":"}");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtUnit(Idx n,Nav& nav,Out& out) {
      // out.raw(io?"(":")");
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtMode(Idx n,Nav& nav,Out& out) {
      if(io) switch(Nav::mode()) {
        case Modes::Normal: out.raw(' ');break;
        case Modes::Edit: out.raw(':');break;
        case Modes::Tune: out.raw('>');break;
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtTitle(Idx n,Nav& nav,Out& out) {
      if (!io) out.nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtItem(Idx n,Nav& nav,Out& out) {
      if (!io) out.nl();
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtIndex(Idx n,Nav& nav,Out& out) {
      if(io) {
        if (n<9) out.raw(n+1);
        else out.raw(' ');
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtCursor(Idx n,Nav& nav,Out& out) {
      if (io) out.raw(nav.selected(n)?((nav.enabled(n)?'>':'-')):' ');
    }
  public:
    template<Roles r,bool io,typename I,typename Out,typename Nav>
    static inline void fmt(Idx n,Nav& nav,Out& out) {
      if (!io) O::template fmt<r,io,I,Out,Nav>(n,nav,out);
      switch(r) {
        case Roles::Panel:  out.template fmtPanel <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Menu:   out.template fmtMenu  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Title:  out.template fmtTitle <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Body:   out.template fmtBody  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Prompt: out.template fmtItem  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Index:  out.template fmtIndex <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Cursor: out.template fmtCursor<io,I,Out,Nav>(n,nav,out);break;
        case Roles::Name:   out.template fmtName  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Mode:   out.template fmtMode  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Value:  out.template fmtValue <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Unit:   out.template fmtUnit  <io,I,Out,Nav>(n,nav,out);break;
        default:break;
      }
      if (io) O::template fmt<r,io,I,Out,Nav>(n,nav,out);
    }
};
