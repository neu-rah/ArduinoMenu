/* -*- C++ -*- */
#pragma once
/**
* @file textFmt.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, add `\n` at title and item end, print index and text cursor
*/

template<typename O>
struct TextFmt:public O {
  public:
    template<bool io,typename Out,typename Nav,typename I>
    static inline void fmtMode(idx_t n) {
      if(io) switch(Nav::mode()) {
        case Modes::Normal: Out::raw(' ');break;
        case Modes::Edit: Out::raw(':');break;
        case Modes::Tune: Out::raw('>');break;
      }
      O::template fmtMode<io,Nav,Out,I>(n);
    }
    // template<bool io,typename Nav,typename Out,typename I>
    // static inline void fmtUnit(idx_t n) {}
    template<bool io,typename Out,typename Nav,typename I>
    static inline void fmtTitle(idx_t n) {
      if (io) {
        // out.fmt(Roles::Prompt,true,nav,out,i,n);//TODO:this will repeat the switch case many times!
        O::template fmtTitle<io,Nav,Out,I>(n);
      } else {
        O::template fmtTitle<io,Nav,Out,I>(n);
        // out.fmt(Roles::Prompt,false,nav,out,i,n);
        Out::nl();
      }
    }
    template<bool io,typename Out,typename Nav,typename I>
    static inline void fmtItem(idx_t n) {
      if(io) O::template fmtItem<io,Nav,Out,I>(n);
      else {
        O::template fmtItem<io,Nav,Out,I>(n);
        Out::nl();
      }
    }
    template<bool io,typename Out,typename Nav,typename I>
    static inline void fmtIndex(idx_t n) {
      if(io) {
        O::template fmtIndex<io,Nav,Out,I>(n);
        if (n<9) Out::raw(n+1);
        else Out::raw(' ');
        // Out::raw(')');
      } else {
        O::template fmtItem<io,Nav,Out,I>(n);
      }
    }
    template<bool io,typename Out,typename Nav,typename I>
    static inline void fmtCursor(idx_t n) {
      if (io) {
        Out::raw(Nav::selected(n)?((Nav::enabled(n)?'>':'-')):' ');
        O::template fmtCursor<io,Nav,Out,I>(n);
      } else {
        O::template fmtCursor<io,Nav,Out,I>(n);
      }
    }
  public:
    template<Roles r,bool io,typename I,typename Out,typename Nav>
    static inline void fmt(idx_t n) {
      switch(r) {
        case Roles::Panel:Out::template fmtPanel<io,Out,Nav,I>(n);break;
        case Roles::Menu:Out::template fmtMenu<io,Out,Nav,I>(n);break;
        case Roles::Title:Out::template fmtTitle<io,Out,Nav,I>(n);break;
        case Roles::Body:Out::template fmtBody<io,Out,Nav,I>(n);break;
        case Roles::Prompt:Out::template fmtItem<io,Out,Nav,I>(n);break;
        case Roles::Index:Out::template fmtIndex<io,Out,Nav,I>(n);break;
        case Roles::Cursor:Out::template fmtCursor<io,Out,Nav,I>(n);break;
        case Roles::Name:Out::template fmtName<io,Out,Nav,I>(n);break;
        case Roles::Mode:Out::template fmtMode<io,Out,Nav,I>(n);break;
        case Roles::Value:Out::template fmtValue<io,Out,Nav,I>(n);break;
        case Roles::Unit:Out::template fmtUnit<io,Out,Nav,I>(n);break;
        default:break;
      }
    }
};
