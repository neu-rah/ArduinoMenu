#pragma once
/**
* @author Rui Azevedo
* @brief C array menu
*/

#include <menu/item.h>

namespace Menu {
  template<typename I,Idx size>
  struct ArrayMenu {
    template<typename O>
    struct Part:O {
      using This=Part<O>;
      using Base=O;
      using Base::Base;
      using ArrayItem=Prompt::template Part<I>*;
      ArrayItem data[size];
      Part(ArrayItem data[]):data(data) {}
      static constexpr Style styles() {return Style::IsMenu|Base::styles();}
      constexpr Idx sz() const {return size;}
      ArrayItem& operator[](size_t i) {return data[i]; }

      template<typename API,typename... Args>
      void forAll(Args... args) {
        for(Idx i=0;i<size;i++)
          API().operator()(*data[i],args...);
      }

      template<typename Target,Idx o,Idx... oo>
      struct Walk {
        Target& target;
        Walk(Target& t):target(t) {}
        template<typename API,typename... Args>
        auto walk(Args... args) 
          ->decltype(
            Walk<IItem,oo...>(*target[0]).template walk<API,Args...>(args...)
          ){
          return Walk<IItem,oo...>(*target[o]).template walk<API,Args...>(args...);
        }
      };

      template<typename Target,Idx o>
      struct Walk<Target,o> {
        Target& target;
        Walk(Target& t):target(t) {}
        template<typename API,typename... Args>
        auto walk(Args... args) 
          ->decltype(
            API().operator()(*(target[o]),args...)
          ){
          return API().operator()(*(target[o]),args...);
        }
      };

      struct RefWalk {
        Idx pathLen;
        const Idx* path;
        RefWalk(Idx l,Idx* p):pathLen(l),path(p) {}
        RefWalk(const RefWalk& p):pathLen(p.len()),path(p.path) {}
        Idx len() const {return pathLen;}
        Idx head() {return path[0];}
        RefWalk tail() {return RefWalk(len()-1,(Idx*)&path[1]);}
        template<typename Target,typename API,typename... Args>
        auto step(Idx at,Target& target,Args... args) 
          ->decltype(API().operator()(*target[0],args...))
          {
          return len()?
            RefWalk(pathLen-1,(Idx*)&path[1])
              .template step<decltype(*target[at]),API,Args...>(head(),*target[at],args...):
            API().operator()(*target[at],args...);
        }
      };

    };
  };
};