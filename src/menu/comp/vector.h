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
  struct VectorMenu:public I,vector<Item*> {
    using vector<Item*>::vector;
    constexpr static inline bool isNode(idx_t) const {return true;}
    inline idx_t size() {return (idx_t)vector<Item*>::size();}
    template<typename Nav,typename Out>
    static inline void print(Nav& bav,Out& out) {}
    template<typename... II>
    inline VectorMenu(II... oo):vector<Item*>{oo...} {}
    template<typename Nav,typename Out>
    inline void printItem(Nav& nav,Out& out,idx_t n) {
      vector<Item*>::operator[](n)->print(nav,out);
    }
    inline bool enabled(idx_t n) const {
      trace(MDO<<"enabled? "<<n<<endl);
      return vector<Item*>::operator[](n)->enabled(0);
    }
    inline void enable(idx_t n,bool o) {
       vector<Item*>::operator[](n)->enable(0,o);
    }
    // inline Item& operator[](idx_t n) {return *vector<Item*>::operator[](n);}
  };
// };
