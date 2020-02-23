#pragma once

#include "path.h"

namespace Menu {

  template<Tag role,typename... R>
  struct As {
    template<typename I>
    struct Part:I {
      using Base=I;
      using This=As<role,R...>;
      using Base::Base;
      using RoleBlock=typename Chain<R::template Part...,Empty>::template To<Obj<Item<R...>>>;
      RoleBlock blk;
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        out.template fmt<role,true>();
        blk.template print<Nav,Out,op>(nav,out,ref);
        out.template fmt<role,false>();
        Base::template print<Nav,Out,op>(nav,out,ref);
      }
    };
  };

  template<typename... O> using AsPanel=As<Tag::Panel,O...>;
  template<typename... O> using AsMenu=As<Tag::Menu,O...>;
  template<typename... O> using AsTitle=As<Tag::Title,O...>;
  template<typename... O> using AsBody=As<Tag::Body,O...>;
  template<typename... O> using AsItem=As<Tag::Item,O...>;
  template<typename... O> using AsIndex=As<Tag::Index,O...>;
  template<typename... O> using AsCursor=As<Tag::Cursor,O...>;
  template<typename... O> using AsName=As<Tag::Name,O...>;
  template<typename... O> using AsMode=As<Tag::Mode,O...>;
  template<typename... O> using AsValue=As<Tag::Value,O...>;
  template<typename... O> using AsUnit=As<Tag::Unit,O...>;
  template<typename... O> using AsRaw=As<Tag::Raw,O...>;

};