/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
*/

#include <menu.h>

template<typename O>
struct Fmt:O {
  template<Roles r,bool io,bool toPrint=true>
  inline void fmt(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (!io) O::template fmt<r,io,toPrint>(n,s,e,m);
    switch(r) {
      case Roles::Panel:  O::template fmtPanel <io,toPrint>(n,s,e,m);break;
      case Roles::Menu:   O::template fmtMenu  <io,toPrint>(n,s,e,m);break;
      case Roles::Title:  O::template fmtTitle <io,toPrint>(n,s,e,m);break;
      case Roles::Body:   O::template fmtBody  <io,toPrint>(n,s,e,m);break;
      case Roles::Item:   O::template fmtItem  <io,toPrint>(n,s,e,m);break;
      case Roles::Index:  O::template fmtIndex <io,toPrint>(n,s,e,m);break;
      case Roles::Cursor: O::template fmtCursor<io,toPrint>(n,s,e,m);break;
      case Roles::Name:   O::template fmtName  <io,toPrint>(n,s,e,m);break;
      case Roles::Mode:   O::template fmtMode  <io,toPrint>(n,s,e,m);break;
      case Roles::Value:  O::template fmtValue <io,toPrint>(n,s,e,m);break;
      case Roles::Unit:   O::template fmtUnit  <io,toPrint>(n,s,e,m);break;
      default:break;
    }
    if (io) O::template fmt<r,io,toPrint>(n,s,e,m);
  }
};
