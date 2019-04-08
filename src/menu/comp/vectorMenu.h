/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use std::vector to hold menu data
// with full power of std::vector
// TODO: generalize for other containers

#include <vector>
using namespace std;
#include <menu.h>

namespace AM5 {

  template<typename O=Text<Empty>>
  struct VectorMenuDef:public O,vector<AM5::Item*> {
    template<typename... OO>
    inline VectorMenuDef(OO... oo):vector<AM5::Item*>{oo...} {}
    template<typename... OO>
    inline VectorMenuDef(const char*title,OO... oo):O(title),vector<AM5::Item*>{oo...} {}
    inline size_t size() const {return vector<AM5::Item*>::size();}
    inline AM5::Item& operator[](size_t n) const {
      return *vector<AM5::Item*>::operator[](n);
    }
  };

};//AM5
