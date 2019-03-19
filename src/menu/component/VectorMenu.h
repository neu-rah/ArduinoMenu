/* -*- C++ -*- */
#pragma once
#include <vector>
using namespace std;
#include <menu.h>

template<typename Out,typename Q>
class VectorMenuDef:public asMenu<Q> {
//straight transpose objectpreserving templates... on production code we will be more succinct
public:
  using O=asMenu<Q>;
  using Def=MenuSystemDef<Out>;
  using Base=typename Def::Base;
  template<typename T>
  using Item=typename Def::template Item<T>;

  VectorMenuDef(initializer_list<Base*> oo):data(oo) {}
  VectorMenuDef(const char*title,initializer_list<Base*> oo):O(title),data(oo) {}
  template<typename... OO>
  VectorMenuDef(const char*title,OO... oo):O(title),data{oo...} {}
  template<typename... OO>
  VectorMenuDef(OO... oo):data{oo...} {}
  inline size_t size() const {return data.size();}
  inline Base& operator[](size_t n) const {return *data[n];}
protected:
  vector<Base*> data;
};
