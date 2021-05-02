/* -*- C++ -*- */

#include "../lang.h"
namespace lang_de {
  const char* lang[(int)TextId::nText]{
    #ifdef HEADLESS
      "MicroTC361",//  txtMain,
    #else
      "MicroTC360",//  txtMain,
    #endif
    "Exit",//txtExit
    "Zurück",
    // "<Zur\xFcck",//txtBack,
    "Zeit",//  txtTime,
    "An",// txtOn,
    "Aus",// txtOff,
    "Ja",// txtYes,
    "Nein",// txtNo,
    "",//  txtLevel,
    "",//  txtSched,
    "Speichern",//  txtSave,
    "Laden",//txtLoad
    "Led",//  txtLed,
    "Licht",//  txtLight,
    "Einstellungen",//txtSettings,
    "Standby",//txtStandby,
    "Wi-Fi",//txtWifi,
    "Wi-Fi:",//txtWifiTog,
    "Wolken:",//txtCloud,
    "Gewitter:",//txtThunder,
    "Mondlicht",//txtMoon,
    "Zeit/Datum einstellen",//txtDateTime,
    "Ok",//txtOk
    "Abbrechen",//txtCancel
    "kein Packet empfangen",//txtNoPack
    "Sekunden seit 1900",//txtSeconds1900
    "benutze NTP",//txtUseNTP
    "Zeitzone GMT",//txtGMT
    "(+neuer Eintrag)",//txtAddEntry
    "Auto. Verlauf",//txtAutoSched
    "Slot",//txtSlot
    "Auto",//txtAuto
    "Manuell",//txtManual
    "Control:",//txtControl
    "Manuelles Level",//txtManualLevel
    "drücke [select]",//txtPressSelect
    "zum fortfahren...",//txtToCont
    "kein WiFi gefunden",//txtNoWifi
    "Wifi Fehler",//txtWifiError
    "Suche...",//txtScan
    "kein Netzwerk gefunden",//txtNoNet
    "auswählen...",//txtSelect
    "SSID:",//txtSSID
    "Pwd",//txtPwd
    "Verbinden",//txtConnect
    "Trennen",//txtDisconnect
    "#1",//txtMoonChan1
    "#2",//txtMoonChan2
    "#3",//txtMoonChan3
    "#4",//txtMoonChan4
    "#5",//txtMoonChan5
    "#6",//txtMoonChan6
    "gespeichert.",//txtSavedOk
    "Speicher Fehler!",//txtSaveError
    "geladen",//txtLoadedOk
    "Lade Fehler!",//txtLoadError,
    "Verbinde mit:",//txtConWifi
    "Pwd: ",//txtApPwd
    "IP: ",//txtIP
    "Access Point",//txtAccessPoint
    "Sichern",//txtBackup
    "Laden",//txtRestore
    "max",//txtCloudMax,
    "max",//txtThunderMax,
    "max",//txtMoonMax,
    "Demo:",//txtDemo
    "Datum / Uhrzeit erhalten...", //txtNTPsync
    "Sommerzeit:",//txtDLS,
    "Datum / Uhrzeit eingestellt",//txtNTPset
    "Sprache",//txtLang
    "Aktualisieren...",//txtUpdate
    "Wiederherstellen!",//txtReset
    "Verwenden der Werkseinstellungen",//txtLCFactory
    #ifdef HEADLESS
      "Über MicroTC361",//txtAbout
    #else
      "Über MicroTC360",//txtAbout
    #endif
    "Credits:\n\n"
    "An dieser Stelle \nmöchten wir uns \nbei allen \nbedanken die \ndieses Projekt \nmöglich gemacht \nhaben:\n\n"
    "Rui Azevedo von \nr-site.net / \nArduinoMenu für \nseine Hilfe beim \nProgrammieren \ndes MicroTC360/1 \nControllers, \ntesten und \ndebuggen\n\n"
    "Tobias Gawrisch \nvon AquaOwner \nfür seine Ideen \nrund um die \nUNIQ FS LED \nSerie und die \nHilfe beim \nDesign\n\n"
    "Oliver L. von \nOcean Optics \nder/die mich \nseit vielen \nJahren bei der \nForschung rund \num das Thema LED \nunterstützen\n\n"
    "Kable, Sales \nManager der \nFabrik wo ich \nmeine LED’s \nfertigen lasse \nda er über viele \nJahre hinaus mit \nmir LED-Lösungen \numsetzt und \nimmer mir Rat \nund Tat zur \nSeite steht.\n\n"
    "Meine Eltern \nweil Sie mich \nseit sehr vielen \nJahren nicht nur \nmoralisch \nunterstützen bei \nder Umsetzung \nder Projekte, \nsondern mir als \nFamilie immer \nwieder Rückhalt \ngeben!\n\n"
    ,//txtAboutText
    "Factory reset?",//txtFactoryReset
    "Click to confirm.",//txtFactoryResetConfirm
    "reset to ignore.",//txtFactoryResetIgnore
    "Zeit anpassen",//txtTimeOffset,
    "Datum einstellen",//txtSetDateTime,
  };
};
