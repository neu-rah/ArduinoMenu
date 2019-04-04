/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// translate roles enum to text, this helps debug and will be used by xml and json

namespace Menu {
  template<Roles role> constexpr char* roleName();
  template<> inline constexpr char* roleName<Roles::Self>() {return "Self";}
  template<> inline constexpr char* roleName<Roles::Menu>() {return "Menu";}
  template<> inline constexpr char* roleName<Roles::Panel>() {return "Panel";}
  template<> inline constexpr char* roleName<Roles::Title>() {return "Title";}
  template<> inline constexpr char* roleName<Roles::Item>() {return "Item";}
  template<> inline constexpr char* roleName<Roles::Pad>() {return "Pad";}
  template<> inline constexpr char* roleName<Roles::Accel>() {return "Accel";}
  template<> inline constexpr char* roleName<Roles::Cursor>() {return "Cursor";}
  template<> inline constexpr char* roleName<Roles::Label>() {return "Label";}
  template<> inline constexpr char* roleName<Roles::Mode>() {return "Mode";}
  template<> inline constexpr char* roleName<Roles::Value>() {return "Value";}
  template<> inline constexpr char* roleName<Roles::Unit>() {return "Unit";}
};//Menu
