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
    template<bool io,typename Out,typename Nav>
    static inline void fmtPanel(Nav& nav,Out& out,Idx n=0) {
      // if (io) out.nl();
      // out.raw("*--------*");
      // out.nl();
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtMenu(Nav& nav,Out& out,Idx n=0) {
      // out.raw("----------");
      // out.nl();
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtBody(Nav& nav,Out& out,Idx n=0) {
      // out.raw(io?"| ":" |");
      // out.nl();
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtName(Nav& nav,Out& out,Idx n=0) {
      out.raw(io?"«":"»");
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtValue(Nav& nav,Out& out,Idx n=0) {
      // out.raw(io?"{":"}");
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtUnit(Nav& nav,Out& out,Idx n=0) {
      // out.raw(io?"(":")");
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtMode(Nav& nav,Out& out,Idx n=0) {
      if(io) switch(nav.mode()) {
        case Modes::Normal: out.raw(' ');break;
        case Modes::Edit: out.raw(nav.selected(n)?':':' ');break;
        case Modes::Tune: out.raw(nav.selected(n)?'>':' ');break;
      }
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtTitle(Nav& nav,Out& out,Idx n=0) {
      // if (!io) out.nl();
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtItem(Nav& nav,Out& out,Idx n=0) {
      if (!io) out.nl();
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtIndex(Nav& nav,Out& out,Idx n=0) {
      if(io) {
        if (n<9) out.raw(n+1);
        else out.raw(' ');
      }
    }
    template<bool io,typename Out,typename Nav>
    static inline void fmtCursor(Nav& nav,Out& out,Idx n=0) {
      if (io) out.raw(nav.selected(n)?((nav.enabled(n)?'>':'-')):' ');
    }
  public:
    template<Roles r,bool io,typename Nav,typename Out>
    static inline void fmt(Nav& nav,Out& out,Idx n=0) {
      if (!io) O::template fmt<r,io,Nav,Out>(nav,out,n);
      switch(r) {
        case Roles::Panel:  Out::template fmtPanel <io,Out,Nav>(nav,out,n);break;
        case Roles::Menu:   Out::template fmtMenu  <io,Out,Nav>(nav,out,n);break;
        case Roles::Title:  Out::template fmtTitle <io,Out,Nav>(nav,out,n);break;
        case Roles::Body:   Out::template fmtBody  <io,Out,Nav>(nav,out,n);break;
        case Roles::Prompt: Out::template fmtItem  <io,Out,Nav>(nav,out,n);break;
        case Roles::Index:  Out::template fmtIndex <io,Out,Nav>(nav,out,n);break;
        case Roles::Cursor: Out::template fmtCursor<io,Out,Nav>(nav,out,n);break;
        case Roles::Name:   Out::template fmtName  <io,Out,Nav>(nav,out,n);break;
        case Roles::Mode:   Out::template fmtMode  <io,Out,Nav>(nav,out,n);break;
        case Roles::Value:  Out::template fmtValue <io,Out,Nav>(nav,out,n);break;
        case Roles::Unit:   Out::template fmtUnit  <io,Out,Nav>(nav,out,n);break;
        default:break;
      }
      if (io) O::template fmt<r,io,Nav,Out>(nav,out,n);
    }
};
