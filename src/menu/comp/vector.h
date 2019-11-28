/* -*- C++ -*- */
#pragma once

#include <menu.h>
#include <vector>

using namespace std;

template<typename I=Empty>
struct VectorMenu:public I,vector<IItem*> {
  using vector<IItem*>::vector;
  // inline Idx size() const {return data.size();}
  // inline Idx size(Ref ref) const {return ref.len?data[ref.head()].size(ref.tail()):data.size();}
  // constexpr static inline bool isNode(idx_t) const {return true;}
  // inline idx_t size() {return (idx_t)vector<Prompt*>::size();}
  // template<typename Nav,typename Out>
  // static inline void print(Nav& bav,Out& out) {}
  // template<typename... II>
  // inline VectorMenu(II... oo):vector<Prompt*>{oo...} {}
  // template<typename Nav,typename Out>
  // inline void printItem(Nav& nav,Out& out,idx_t n) {
  //   vector<Prompt*>::operator[](n)->print(nav,out);
  // }
  // inline bool enabled(idx_t n) const {
  //   trace(MDO<<"enabled? "<<n<<endl);
  //   return vector<Prompt*>::operator[](n)->enabled(0);
  // }
  // inline void enable(idx_t n,bool o) {
  //    vector<Prompt*>::operator[](n)->enable(0,o);
  // }
  // inline Prompt& operator[](idx_t n) {return *vector<Prompt*>::operator[](n);}
};
