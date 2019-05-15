/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, add `\n` at title and item end, print index and text cursor
*/

template<typename O>
struct TextFmt:public O {
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtTitle(Nav& nav,Out& out,I& i,idx_t n) {
    if (io) {
      out.fmt(Roles::Item,true,nav,out,i,n);//TODO:this will repeat the switch case many times!
      //alternative is to add many virtual functions to the interface...
      //or => do demux on virtual and never call the switch version again!
      //Cap should always call template version
      //this gets called with the virtual class, cant use the templates there
      //out.template fmtItem<true>(nav,out,i);
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i,n);
    } else {
      O::template fmtTitle<io,Nav,Out,I>(nav,out,i,n);
      out.fmt(Roles::Item,false,nav,out,i,n);
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtItem(Nav& nav,Out& out,I& i,idx_t n) {
    if(io) O::template fmtItem<io,Nav,Out,I>(nav,out,i,n);
    else {
      O::template fmtItem<io,Nav,Out,I>(nav,out,i,n);
      out.nl();
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  static inline void fmtIndex(Nav& nav,Out& out,I& i,idx_t n) {
    if(io) {
      O::template fmtIndex<io,Nav,Out,I>(nav,out,i,n);
      if (n<9) out.raw(n+1);
      else out.raw(' ');
      // out.raw(')');
    } else {
      O::template fmtItem<io,Nav,Out,I>(nav,out,i,n);
    }
  }
  template<bool io,typename Nav,typename Out,typename I>
  inline void fmtCursor(Nav& nav,Out& out,I& i,idx_t n) {
    if (io) {
      out.raw(nav.selected(n)?(nav.enabled(n)?'>':'-'):' ');
      O::template fmtCursor<io,Nav,Out,I>(nav,out,i,n);
    } else {
      O::template fmtCursor<io,Nav,Out,I>(nav,out,i,n);
    }
  }
};
