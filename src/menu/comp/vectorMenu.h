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

  template<typename O>
  struct VectorMenu:public O,vector<Item*> {
    template<typename... OO>
    inline VectorMenu(OO... oo):vector<Item*>{oo...} {}
    template<typename... OO>
    inline VectorMenu(const char*title,OO... oo):O(title),vector<Item*>{oo...} {}
    inline size_t size() const {return vector<Item*>::size();}
    inline Item& operator[](size_t n) const {
      return *vector<Item*>::operator[](n);
    }
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItem() {
      // cout<<"VectorMenu::printItem"<<endl;
      O::template printItem<NavHead,OutHead,ItemHead,idx>();
    }
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    inline void printItems(ItemHead& item) {
      cout<<"VectorMenu::printItems"<<endl;
      for(auto i: *this)
        i->printTo();//<NavHead,OutHead,ItemHead,idx>();
    }
    // template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    // static inline void printMenu(ItemHead&) {
    //   cout<<"VectorMenu::printMenu"<<endl;
    // }

    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(ItemHead& item) {
      cout<<"VectorMenu::printMenuRaw"<<endl;
      OutHead::template printMenu<NavHead,OutHead,ItemHead,0>(item);
    }

  };
};//AM5
