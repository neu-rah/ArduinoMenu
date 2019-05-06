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
    using This=VectorMenu<O>;
    template<typename... OO>
    inline VectorMenu(OO... oo):vector<Item*>{oo...} {}
    template<typename... OO>
    inline VectorMenu(const char*title,OO... oo):O(title),vector<Item*>{oo...} {}
    inline size_t size() const {return vector<Item*>::size();}
    inline Item& operator[](size_t n) const {
      return *vector<Item*>::operator[](n);
    }
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItem(NavHead& nav,OutHead& out) {
      // cout<<"VectorMenu::printItem"<<endl;
      O::template printItem<NavHead,OutHead,ItemHead,idx>(nav,out);
    }
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    inline void printItems(NavHead& nav,OutHead& out,ItemHead& item) {
      // cout<<"VectorMenu::printItems"<<endl;
      for(auto i: *this) {
        using ItemPrinter=typename OutHead::Printers::template Item<This>;
        out.template fmtItem<NavHead,OutHead,ItemHead,true,idx>(nav,out);
        MenuNav<NavHead> tmp(nav);
        i->printTo(tmp,out);
        using ItemPrinter=typename OutHead::Printers::template Item<This>;
        out.template fmtItem<NavHead,OutHead,ItemHead,false,idx>(nav,out);
      }
    }

    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(NavHead& nav,OutHead& out,ItemHead& item) {
      cout<<"VectorMenu::printMenuRaw"<<endl;
      OutHead::template printMenu<NavHead,OutHead,ItemHead,0>(nav,out,item);
    }

  };
};//AM5
