/* -*- C++ -*- */
#pragma once
/**
* @file vector.h
* @author Rui Azevedo
* @brief ArduinoMenu std::vector based menu
*/

#include <menu.h>
#include <vector>
#include <iostream>
using namespace std;

// using namespace Menu;

// namespace Menu {
  /**
  * The VectorMenu class extends std::vector as a menu
  */
  template<typename I=Empty<>>
  struct VectorMenu:public I,vector<Prompt*> {
    using vector<Prompt*>::vector;
    constexpr static inline bool isNode(Idx) const {return true;}
    inline Idx size() {return (Idx)vector<Prompt*>::size();}
    template<typename Nav,typename Out>
    static inline void print(Nav& bav,Out& out) {}
    template<typename... II>
    inline VectorMenu(II... oo):vector<Prompt*>{oo...} {}
    template<typename Nav,typename Out>
    inline void printItem(Nav& nav,Out& out,Idx n,Idx p=0) {
      vector<Prompt*>::operator[](n)->print(nav,out);
    }
    inline bool enabled(Idx n) const {
      return vector<Prompt*>::operator[](n)->enabled(0);
    }
    inline void enable(Idx n,bool o) {
       vector<Prompt*>::operator[](n)->enable(0,o);
    }
    // inline Prompt& operator[](Idx n) {return *vector<Prompt*>::operator[](n);}
  };
// };
