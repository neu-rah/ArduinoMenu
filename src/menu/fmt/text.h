/* -*- C++ -*- */
#pragma once

#include "../out.h"

namespace Menu {

  //reflect nav. edit state using cursor chars
  template<char none,char edit,char tune>
  struct EditMode {
    template<typename O=Empty<Nil>>
    struct Part:O {
      using Base=O;
      using This=Part<Base>;
      using Base::Base;
      template<Fmt tag,bool start> 
      void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {
        if(tag==Fmt::EditMode) {
          if(start) {
            Base::template fmt<tag,start>(editing,tunning,n,sel,en);
            if(Base::padding()) {
              if(editing&&sel) Base::print(tunning?tune:edit);
            } else Base::print((editing&&sel)?(tunning?tune:edit):none);
          } else Base::template fmt<tag,start>(editing,tunning,n,sel,en);
        } else Base::template fmt<tag,start>(editing,tunning,n,sel,en);
      }
    };
  };

  //default edit cursors for text mode
  using TextEditMode=EditMode<' ',':','>'>;

  //print a character before part print
  template<char prefix='['>
  struct Prefix {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      template<Fmt tag,bool start> 
      void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {
        if(start) Base::print(prefix);
        Base::template fmt<tag,start>(editing,tunning,n,sel,en);
      }
    };
  };

  //print a character after part print
  template<char prefix=']'>
  struct Sufix {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      template<Fmt tag,bool start> 
      void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {
        if(!start) Base::print(prefix);
        Base::template fmt<tag,start>(editing,tunning,n,sel,en);
      }
    };
  };

  //wrap a part printing with prefix and sufix characters
  template<char prefix='[',char sufix=']'>
  struct PrintWrap {
    template<typename O>
    struct Part:Union<Prefix<prefix>::template Part,Sufix<sufix>::template Part>::template Part<O> {};
  };

  //conditionally apply a format to a specified part
  template<Fmt on,template<typename> class Do>
  struct OnFmt {
    template<typename O>
    struct Part:Do<O> {
      using Base=Do<O>;
      using This=Part<O>;
      using Base::Base;
      template<Fmt tag,bool start> 
      void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {
        using Skip=typename Base::Base;
        if(on==tag) Base::template fmt<tag,start>(editing,tunning,n,sel,en);
        else Skip::template fmt<tag,start>(editing,tunning,n,sel,en);
      }
    };
  };

  struct TextFmt {
    template<typename O=Void<>>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      using Base::print;
      using Base::nl;
      template<Fmt tag,bool start> 
      void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {
        switch(tag) {
          // case Fmt::EditCursor:
          //   if(!editing) return;
          //   if(start) print(tunning?'>':'[');
          //   else print(tunning?'<':']');
          //   break;
          case Fmt::Title:
            if(!start) {
              if(Base::padding()) print(" ");
              else nl();
            }
            break;
          case Fmt::Item:
            if (start) {
              if(Base::padding()) {
                if(sel) print(en?'>':'-');
              } else {
                print((char)(n<10?'0'+n:' '));
                print(sel?(en?'>':'-'):' ');
              }
            } else nl();
            break;
          default: break;
        }
        Base::template fmt<tag,start>(editing,tunning,n,sel,en);
      }
    };
  };

};//Menu namespace
