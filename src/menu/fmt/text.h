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
    static inline void fmtMode(Idx n,const Nav& nav,const Out& out) {
      if(io) switch(Nav::mode()) {
        case Modes::Normal: Out::raw(' ');break;
        case Modes::Edit: Out::raw(':');break;
        case Modes::Tune: Out::raw('>');break;
      }
      O::template fmtMode<io,I,Out,Nav>(n,nav,out);
    }
    // template<bool io,typename Nav,typename Out,typename I>
    // static inline void fmtUnit(Idx n) {}
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtTitle(Idx n,const Nav& nav,const Out& out) {
      if (io) {
        // out.fmt(Roles::Prompt,true,nav,out,i,n);//TODO:this will repeat the switch case many times!
        O::template fmtTitle<io,I,Out,Nav>(n,nav,out);
      } else {
        O::template fmtTitle<io,I,Out,Nav>(n,nav,out);
        // out.fmt(Roles::Prompt,false,nav,out,i,n);
        Out::nl();
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtItem(Idx n,const Nav& nav,const Out& out) {
      if(io) O::template fmtItem<io,I,Out,Nav>(n,nav,out);
      else {
        O::template fmtItem<io,I,Out,Nav>(n,nav,out);
        Out::nl();
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtIndex(Idx n,const Nav& nav,const Out& out) {
      if(io) {
        O::template fmtIndex<io,I,Out,Nav>(n,nav,out);
        if (n<9) Out::raw(n+1);
        else Out::raw(' ');
        // Out::raw(')');
      } else {
        O::template fmtItem<io,I,Out,Nav>(n,nav,out);
      }
    }
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtCursor(Idx n,const Nav& nav,const Out& out) {
      if (io) {
        Out::raw(nav.selected(n)?((nav.enabled(n)?'>':'-')):' ');
        O::template fmtCursor<io,I,Out,Nav>(n,nav,out);
      } else {
        O::template fmtCursor<io,I,Out,Nav>(n,nav,out);
      }
    }
  public:
    template<Roles r,bool io,typename I,typename Out,typename Nav>
    static inline void fmt(Idx n,const Nav& nav,Out& out) {
      switch(r) {
        case Roles::Panel:  Out::template fmtPanel <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Menu:   Out::template fmtMenu  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Title:  Out::template fmtTitle <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Body:   Out::template fmtBody  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Prompt: Out::template fmtItem  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Index:  Out::template fmtIndex <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Cursor: Out::template fmtCursor<io,I,Out,Nav>(n,nav,out);break;
        case Roles::Name:   Out::template fmtName  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Mode:   Out::template fmtMode  <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Value:  Out::template fmtValue <io,I,Out,Nav>(n,nav,out);break;
        case Roles::Unit:   Out::template fmtUnit  <io,I,Out,Nav>(n,nav,out);break;
        default:break;
      }
    }
};
