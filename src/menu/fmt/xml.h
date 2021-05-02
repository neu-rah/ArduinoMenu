/* -*- C++ -*- */
#pragma once

#include "../out.h"

namespace Menu {
  struct XmlFmt {
    template<typename O=Void<>>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      template<Fmt tag,bool start> 
      void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {
        Base::print("<");
        if(!start) Base::print("/");
        Base::print(Base::template tagName<tag>());
        Base::print(">");
        if(!start) Base::print("\r\n");
      }
    };
  };

};//Menu namespace
