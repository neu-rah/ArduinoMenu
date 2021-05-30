/* -*- C++ -*- */
#pragma once
/*
output base and virtual cap
*/

#include "sys/base.h"

namespace Menu {

  //Void class
  //The inactive output definition 
  //for static output API
  template<typename O=Nil>
  struct Void:O {
    Void() {}
    Void(const Void&)=delete;
    inline static void begin() {}
    Void& operator=(const Void&)=delete;
    template<typename Dev> inline static void poll(Dev&) {}
    inline static void nl() {}
    inline static void newView() {}
    template<typename Nav,typename Menu>
    static void printMenu(Nav& nav,Menu& menu,Idx selIdx) {}
    template<typename Nav,typename Item>
    static void printTitle(Nav& nav,Item& i) {}
    template<typename Nav,typename Item>
    static void printItem(Nav& nav,Item& i,Idx n=0,bool sel=false) {}
    template<Fmt,bool> static void fmt(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {}
    template<Fmt tag> void fmtStart(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {O::obj().template fmt<tag,true>(editing,tunning,n,sel,en);}
    template<Fmt tag> void fmtStop(bool editing,bool tunning,int n=0,bool sel=false,bool en=true) {O::obj().template fmt<tag,false>(editing,tunning,n,sel,en);}
    template<Fmt tag>
    static ConstText tagName() {//we need this for xml (not just debug)
      switch(tag) {
        case Fmt::Panel: return "panel";
        case Fmt::Menu: return "menu";
        case Fmt::Title: return "title";
        case Fmt::Body: return "body";
        case Fmt::Item: return "item";
        case Fmt::Pad: return "pad";
        case Fmt::Index: return "index";
        case Fmt::Accel: return "accel";
        case Fmt::Cursor: return "cursor";
        case Fmt::Field: return "field";
        case Fmt::Label: return "label";
        case Fmt::EditMode: return "editMode";
        case Fmt::EditCursor: return "editCursor";
        case Fmt::Value: return "value";
        case Fmt::Unit: return "unit";
        default: return "unknown";
      }
    }
  };

  //print menu title
  struct TitlePrinter {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      template<typename Nav,typename Menu>
      void printMenu(Nav& nav,Menu& menu,Idx selIdx) {
        menu.onPrintTitleTo(nav,Base::obj());
        Base::template printMenu<Nav,Menu>(nav,menu,selIdx);
      }
      template<typename Nav,typename Item>
      void printTitle(Nav& nav,Item& i) {
        Base::template fmtStart<Fmt::Title>(nav.focus(),nav.tune());
        i.template printTo<Nav,typename This::Type>(nav,Base::obj());
        Base::template fmtStop<Fmt::Title>(nav.focus(),nav.tune());
      }
    };
  };

  //print menu body
  struct BodyPrinter {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      template<typename Nav,typename Menu>
      void printMenu(Nav& nav,Menu& menu,Idx selIdx) {
        Base::template fmtStart<Fmt::Body>(nav.focus(),nav.tune());
        menu.onPrintBodyTo(nav,Base::obj(),selIdx,0);
        Base::template fmtStop<Fmt::Body>(nav.focus(),nav.tune());
        Base::template printMenu<Nav,Menu>(nav,menu,selIdx);
      }
    };
  };

  //provides aux functions for printing
  struct BasePrinter {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      template<typename Nav,typename Menu>
      void printMenu(Nav& nav,Menu& menu,Idx selIdx) {
        Base::template fmtStart<Fmt::Menu>(nav.focus(),nav.tune());
        Base::template printMenu<Nav,Menu>(nav,menu,selIdx);
        Base::template fmtStop<Fmt::Menu>(nav.focus(),nav.tune());
      }
      template<typename Nav,typename Item>
      void printItem(Nav& nav,Item& i,Idx n=0,bool sel=false) {
        Base::template fmtStart<Fmt::Item>(nav.focus(),nav.tune(),n,sel,i.enabled());
        i.onPrintItemTo(nav,Base::obj(),n,sel);
        Base::template fmtStop<Fmt::Item>(nav.focus(),nav.tune(),n,sel,i.enabled());
      }
    };
  };

  //print title and body
  using FullPrinter=MakePart<TitlePrinter::Part,BodyPrinter::Part,BasePrinter::Part>;

////////////////////////////////

  // struct TextMeasure {
  //   template<typename O>
  //   struct Part:O {
  //     using Base=O;
  //     using This=Part<O>;
  //     using Base::Base;
  //     template<bool toPrint=true>
  //     void write(const char* o,Idx len){dev.write(o,len);}
  //     template<typename Data>
  //     void print(Data o) {dev.print(o);}
  //     void nl() {dev.println();}
  //   };
  // };

///////////////////////////////////////////////////////////////////////////////////
// panels

  //statically defined area
  //defined by rectangle x,y,w,h
  template<Idx x,Idx y,Idx w,Idx h>
  struct StaticPanel {
    template<typename O>
    struct Part:O {
      constexpr static inline Idx orgX() {return x;}
      constexpr static inline Idx orgY() {return y;}
      constexpr static inline Idx width() {return w;}
      constexpr static inline Idx height() {return h;}

      constexpr static inline Idx posX() {return x;}
      constexpr static inline Idx posY() {return y;}
      constexpr static inline Idx freeX() {return w;}
      constexpr static inline Idx freeY() {return h;}
      constexpr static inline Idx free() {return w*h;}
      static inline void useX(Idx ux=1) {}
      static inline void useY(Idx uy=1) {}
      static inline void use(Idx ux=1,Idx uy=1) {}
    };
  };

  template<typename Unit>
  struct ViewportTrack {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      //restart the panel
      inline void newView() {fx=O::width();fy=Base::height();}
      //device coordinates ---------
      inline Idx posX() const {return Base::width()-fx;}
      inline Idx posY() const {return Base::height()-fy;}
      // get free space ----
      inline Idx freeX() const {return fx;}
      inline Idx freeY() const {return fy;}
      inline Idx free() const {return fx+Base::width()*fy;}
      // // use space ----
      inline void useX(Unit ux=1) {
        if (fx) fx-=ux;
        else useY();
      }
      inline void useY(Unit uy=1) {
        if (!fy) fx=0;
        else {
          fy-=uy;
          if(uy) fx=O::width();
        }
      }
      template<bool toPrint=true>
      void write(const char* o,Idx len){
        if(toPrint) Base::write(o,len);
        useX(len);
      }
      template<typename Data,bool toPrint=true>
      void print(Data o) {
        if(toPrint) Base::print(o);
        
      }
      template<bool toPrint=true>
      void nl() {
        if(toPrint) Base::nl();
        useY();
      }
      protected:
      //free space tracking
      Unit fx=Base::width();
      Unit fy=Base::height();
    };
  };

  template<Idx Sz>
  struct RangePanel {
    template<typename O>
    struct Part:O {
      using Base=O;
      using This=Part<O>;
      using Base::Base;
      Idx top=0;
      template<typename Nav,typename Item>
      void printItem(Nav& nav,Item& i,Idx n=0,bool sel=false) {
        auto pos=nav.ppos();//has<Style::ParentDraw>(i.styles())?nav.ppos():nav.pos();
        while(pos<top) top--;
        while(pos>(top+Sz-1)) top++;
        if(n>=top&&(n-top)<Sz) Base::template printItem<Nav,Item>(nav,i,n,sel);
      }
    };
  };

/////////////////////////////////////////////////////////////////////////////////
// virtual top interface

  //define an output
  //aglomerate all parts into a single output definition
  template<template<typename> class Base,template<typename> class... O>
  using OutDefBase=Composition<Base,O...,Void>;

  class IOut {
    public:
      IOut() {}
      virtual void print(int o){}
      virtual void print(const char* o){}
      virtual void printMenu(INav& nav,IItem&,Idx) {}
      virtual void printTitle(INav& nav,IItem&) {}
      virtual void printItem(INav& nav,IItem&,Idx=0,bool=false) {}
  };

  //joins mixins static output composition with the vitual output interface
  class MenuOut {
    public:
      template<typename O>
      struct Part:IOut, O {
        using Base=O;
        using This=Part<Base>;
        using Base::Base;
        void print(int o) override {O::print(o);}
        void print(const char* o) override {O::print(o);}
        void printMenu(INav& nav,IItem& it,Idx selIdx) override {Base::printMenu(nav,it,selIdx);}
        void printTitle(INav& nav,IItem& it) override {Base::printTitle(nav,it);}
        void printItem(INav& nav,IItem& it,Idx i=0,bool s=false) override {Base::printItem(nav,it,s);}
      };
  };

};//Menu namespace
