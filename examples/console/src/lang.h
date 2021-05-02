/* -*- C++ -*- */
#pragma once
// LANGUAGE /////////////////////////////////////////////////////////
// or define DEFAULT_LANG at compiler options (preferable)
// #define DEFAULT_LANG lang_en
// #define DEFAULT_LANG lang_pt
// #define DEFAULT_LANG lang_de
#ifndef DEFAULT_LANG
  #error Must define DEFAULT_LANG, see lang.h file
#endif

enum Langs {de=0,en,pt,nLangs};
//if having names on its own language this settings stay here (font problem, cant find a font to support all languages special chars)
//if having language names in selected language this setting must replicate to lang/*.cpp files
constexpr const char*langNames[nLangs]{"DE","EN","PT"};
constexpr const char*langCodes[nLangs]{"de","en","pt"};

enum TextId {
  txtMain=0,
  txtExit,
  txtBack,
  txtTime,
  txtOn,
  txtOff,
  txtYes,
  txtNo,
  txtLevel,
  txtSched,
  txtSave,
  txtLoad,
  txtLed,
  txtLight,
  txtSettings,
  txtStandby,
  txtWifi,
  txtWifiTog,
  txtCloud,
  txtThunder,
  txtMoon,
  txtDateTime,
  txtOk,
  txtCancel,
  txtNoPack,
  txtSeconds1900,
  txtUseNTP,
  txtGMT,
  txtAddEntry,
  txtAutoSched,
  txtSlot,
  txtAuto,
  txtManual,
  txtControl,
  txtManualLevel,
  txtPressSelect,
  txtToCont,
  txtNoWifi,
  txtWifiError,
  txtScan,
  txtNoNet,
  txtSelect,
  txtSSID,
  txtPwd,
  txtConnect,
  txtDisconnect,
  txtMoonChan1,
  txtMoonChan2,
  txtMoonChan3,
  txtMoonChan4,
  txtMoonChan5,
  txtMoonChan6,
  txtSavedOk,
  txtSaveError,
  txtLoadedOk,
  txtLoadError,
  txtConWifi,
  txtApPwd,
  txtIP,
  txtAccessPoint,
  txtBackup,
  txtRestore,
  txtCloudMax,
  txtThunderMax,
  txtMoonMax,
  txtDemo,
  txtNTPsync,
  txtDLS,
  txtNTPset,
  txtLang,
  txtUpdate,
  txtReset,
  txtLCFactory,
  txtAbout,
  txtAboutText,
  txtFactoryReset,
  txtFactoryResetConfirm,
  txtFactoryResetIgnore,
  txtTimeOffset,
  txtSetDateTime,
  //------------------
  nText//this one MUST be the last
};

namespace lang_de {
  constexpr Langs fwLang=Langs::de;
  extern const char* lang[nText];
};
namespace lang_en {
  constexpr Langs fwLang=Langs::en;
  extern const char* lang[nText];
};
namespace lang_pt {
  constexpr Langs fwLang=Langs::pt;
  extern const char* lang[nText];
};

// namespace lang_ru {
//   constexpr Langs fwLang=Langs::ru;
//   extern const char* lang[nText];
// };

using namespace DEFAULT_LANG;

extern const char** langs[nLangs];

