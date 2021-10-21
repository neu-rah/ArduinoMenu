/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  #include <ERM19264_UC1609.h>
  #include "../../out.h"

  namespace Menu {

    struct UC1609TextFmt {
      template<typename O=Void<>>
      struct Part:O {
        using Base=O;
        using This=Part<O>;
        using Base::Base;
        template<Fmt tag,bool start> 
        void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true,bool preview=false) {
          // if(start) fmt<tag,start>(editing,tunning,n,sel,en);
          // if(tag==Fmt::Item&&!start) Base::cl();
          // if(!start) 
          // fmt<tag,start>(editing,tunning,n,sel,en);
        }
      };
    };
    template<typename Dev,Dev& dev,int fontx,int fonty,int lcdwidth,int lcdheight>
    struct UC1609TextOutBase {
      template<typename O>
      struct Part:O {
        using Base=O;
        using Base::Base;
        using This=Part<O>;
        Idx cx,cy;
        void newView() {
          cx=0;
          cy=0;
          dev.LCDNoBufferGotoXY(0,0);
          Base::newView();
        }
        void write(const char* o,int len){
          for(int n=0;n<len&&cx<lcdwidth;cx++,n++)
            dev.LCDNoBufferChar(o[n]);
        }
        void cl() {
          for(;cx<lcdwidth;cx++)
            dev.LCDNoBufferChar(' ');
        }
        template<typename Data>
        void print(Data o) {
          String d(o);
          write(d.c_str(),d.length());
        }
        void nl() {
          cl();
          cx=0;
          dev.LCDNoBufferGotoXY(0,++cy);
          Base::nl();
        }
      };
    };
    
    template<typename Dev,Dev& dev,int fontx=7,int fonty=9,int lcdwidth=192/fontx,int lcdheight=64/fonty>
    using UC1609TextOut=Chain<
      UC1609TextFmt::template Part,
      UC1609TextOutBase<Dev,dev,fontx,fonty,lcdwidth,lcdheight>::template Part
    >;


  };//Menu namespace
#endif