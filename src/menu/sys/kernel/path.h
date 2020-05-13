/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu path reference, this kind of reference can be used by both dynamic and static content
* use like: Path<3,0>().ref()
*/

#include "base.h"

namespace Menu {
  //a reference to menu item that works also for static structures
  //and dynamic access
  struct PathRef {
    Idx len;
    const Idx* path;
    // inline PathRef(Idx len,Idx* path):len(len),path(path) {}
    inline Idx head() const {return len?path[0]:0;}
    inline PathRef tail() const {return len?(PathRef){(Idx)(len-1),&path[1]}:(PathRef){0,0};}
    // inline operator Idx() const {return len;}
    // inline operator Idx() const=delete;
    inline operator bool() const {return len>1;}
    inline PathRef parent() const {
      return len?(PathRef){(Idx)(len-1),path}:*this;
    }
  };

  //default self reference
  extern constexpr PathRef self{0,0};

  //synthesizing a PathReference at compile time
  template<Idx... OO>
  struct Path {
    template<Idx i,Idx... ii> struct Len {inline static constexpr size_t len() {return 1+Len<ii...>::len();}};
    template<Idx i> struct Len<i> {inline static constexpr size_t len() {return 1;}};

    Idx path[Len<OO...>::len()]{OO...};
    inline PathRef ref() {return {Len<OO...>::len(),path};}
  };

  #ifdef MENU_DEBUG
    template<typename O>
    inline O& operator<<(O& o,PathRef ref) {
      o<<"{"<<ref.len<<",[";
      if(ref.path)
        for(int n=0;n<ref.len;n++)
          o<<(n?", ":"")<<ref.path[n];
      else o<<"NULL";
      o<<"]}";
      return o;
    }
  #endif
};
