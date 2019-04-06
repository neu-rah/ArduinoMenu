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

namespace Menu {

  template<typename O=Text<Empty>>
  struct VectorMenuDef:public O,vector<Menu::Item*> {
    template<typename... OO>
    inline VectorMenuDef(OO... oo):vector<Menu::Item*>{oo...} {}
    template<typename... OO>
    inline VectorMenuDef(const char*title,OO... oo):O(title),vector<Menu::Item*>{oo...} {}
    inline size_t size() const {return vector<Menu::Item*>::size();}
    inline Menu::Item& operator[](size_t n) const {
      return *vector<Menu::Item*>::operator[](n);
    }
  };

};//Menu
