/* -*- C++ -*- */

#include "../lang.h"
namespace lang_en {
  const char* lang[(int)TextId::nText]{
    #ifdef HEADLESS
      "MicroTC361",//  txtMain,
    #else
      "MicroTC360",//  txtMain,
    #endif
    "Exit",//txtExit
    "<Back",//txtBack,
    "Time",//  txtTime,
    "On",// txtOn,
    "Off",// txtOff,
    "Yes",// txtYes,
    "No",// txtNo,
    "",//  txtLevel,
    "",//  txtSched,
    "Save",//  txtSave,
    "Load",//txtLoad
    "Led",//  txtLed,
    "Light",//  txtLight,
    "Settings",//txtSettings,
    "Standby",//txtStandby,
    "Wi-Fi",//txtWifi,
    "Wi-Fi:",//txtWifiTog,
    "Cloud:",//txtCloud,
    "Thunderstorm:",//txtThunder,
    "Moonlight",//txtMoon,
    "Set Date/Time",//txtDateTime,
    "Ok",//txtOk
    "Cancel",//txtCancel
    "no packet yet",//txtNoPack
    "seconds since 1900",//txtSeconds1900
    "use NTP:",//txtUseNTP
    "Timezone GMT",//txtGMT
    "(+add entry)",//txtAddEntry
    "Auto schedule",//txtAutoSched
    "Slot",//txtSlot
    "Auto",//txtAuto
    "Manual",//txtManual
    "Control:",//txtControl
    "Manual level",//txtManualLevel
    "press [select]",//txtPressSelect
    "to continue...",//txtToCont
    "no WiFi found",//txtNoWifi
    "Wifi error",//txtWifiError
    "Scanning...",//txtScan
    "no networks found",//txtNoNet
    "select...",//txtSelect
    "SSID:",//txtSSID
    "Pwd",//txtPwd
    "Connect",//txtConnect
    "Disconnect",//txtDisconnect
    "#1",//txtMoonChan1
    "#2",//txtMoonChan2
    "#3",//txtMoonChan3
    "#4",//txtMoonChan4
    "#5",//txtMoonChan5
    "#6",//txtMoonChan6
    "Saved.",//txtSavedOk
    "Save error!",//txtSaveError
    "Loaded",//txtLoadedOk
    "Load error!",//txtLoadError,
    "Connecting to:",//txtConWifi
    "Pwd: ",//txtApPwd
    "IP: ",//txtIP
    "Access Point",//txtAccessPoint
    "Backup",//txtBackup
    "Restore",//txtRestore
    "max",//txtCloudMax,
    "max",//txtThunderMax,
    "max",//txtMoonMax,
    "Demo:",//txtDemo
    "Getting date/time from web...", //txtNTPsync
    "Daylight save:",//txtDLS,
    "Date/Time set",//txtNTPset
    "Language",//txtLang,
    "Update...",//txtUpdate
    "Reset!",//txtReset
    "Using factory settings",//txtLCFactory
    #ifdef HEADLESS
      "About MicroTC361",//txtAbout
    #else
      "About MicroTC360",//txtAbout
    #endif
    "Credits:\n\n"
    "At this point we \nwould like to \nthank everyone \nwho made this \nproject possible:\n\n"
    "Rui Azevedo from \nr-site.net / \nArduinoMenu for \nhis help with \nprogramming the \nMicroTC360/1 \ncontroller, \ntesting and \ndebugging\n\n"
    "Tobias Gawrisch \nfrom AquaOwner \nfor his ideas \nabout the \nUNIQ FS LED \nseries and the \nhelp with the \ndesign\n\n"
    "Oliver L. of \nOcean Optics, \nwho has been \nsupporting me in \nLED research for \nmany years\n\n"
    "Kable, Sales \nManager of the \nfactory where I \nmanufacture my \nLED's because he \nimplements LED \nsolutions with \nme for many \nyears and always \nsupports me with \nadvice and \nassistance.\n\n"
    "My parents who \nsupported me for \nmany years, not \nonly morally \nduring the \nimplementation \nof projects, but \nback me up as \nfamily again and \nagain!\n\n"
    ,//txtAboutText
    "Factory reset?",//txtFactoryReset
    "Click to confirm.",//txtFactoryResetConfirm
    "reset to ignore.",//txtFactoryResetIgnore
    "Time offset",//txtTimeOffset,
    "Set date/time",//txtSetDateTime,
  };
};