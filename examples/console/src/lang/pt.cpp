/* -*- C++ -*- */

#include "../lang.h"
namespace lang_pt {
  const char* lang[nText]{
    #ifdef HEADLESS
      "MicroTC361",//  txtMain,
    #else
      "MicroTC360",//  txtMain,
    #endif
    "Sair",//txtExit
    "<Voltar",//txtBack,
    "Hora",//  txtTime,
    "Ligado",// txtOn,
    "Desligado",// txtOff,
    "Sim",// txtYes,
    "Não",// txtNo,
    "",//  txtLevel,
    "",//  txtSched,
    "Guardar",//  txtSave,
    "Carregar",//txtLoad
    "Led",//  txtLed,
    "Luz",//  txtLight,
    "Parametros",//txtSettings,
    "Inactivo",//txtStandby,
    "Wi-Fi",//txtWifi,
    "Wi-Fi:",//txtWifiTog,
    "Nuvens:",//txtCloud,
    "Relampagos:",//txtThunder,
    "Luar",//txtMoon,
    "Data e Hora",//txtDateTime,
    "Aplicar",//txtOk
    "Cancelar",//txtCancel
    "sem resposta",//txtNoPack
    "segundos desde 1900",//txtSeconds1900
    "use NTP:",//txtUseNTP
    "fuso horário GMT",//txtGMT
    "(+adicionar item)",//txtAddEntry
    "Horário automático",//txtAutoSched
    "Entrada",//txtSlot
    "Auto",//txtAuto
    "Manual",//txtManual
    "Controlo:",//txtControl
    "Nível manual",//txtManualLevel
    "[clique]",//txtPressSelect
    "para continuar...",//txtToCont
    "WiFi não encontrado",//txtNoWifi
    "Erro Wifi",//txtWifiError
    "Procurar...",//txtScan
    "Sem redes",//txtNoNet
    "selecione...",//txtSelect
    "SSID:",//txtSSID
    "Chave",//txtPwd
    "Ligar",//txtConnect
    "Desligar",//txtDisconnect,
    "#1",//txtMoonChan1
    "#2",//txtMoonChan2
    "#3",//txtMoonChan3
    "#4",//txtMoonChan4
    "#5",//txtMoonChan5
    "#6",//txtMoonChan6
    "Guardado.",//txtSavedOk
    "Erro ao guardar!",//txtSaveError
    "Carregado",//txtLoadedOk
    "Erro ao carregar!",//txtLoadError,
    "Ligando a:",//txtConWifi
    "Chave: ",//txtApPwd
    "IP: ",//txtIP
    "Ponto de acesso",//txtAccessPoint
    "Armazenar",//txtBackup
    "Recuperar",//txtRestore
    "max",//txtCloudMax,
    "max",//txtThunderMax,
    "max",//txtMoonMax,
    "Demo:",//txtDemo
    "A obter data/hora da net...", //txtNTPsync
    "Hora de verão:",//txtDLS,
    "Data/Hora acertada",//txtNTPset
    "Lingua",//txtLang,
    "Actualizar...",//txtUpdate
    "Reverter!",//txtReset
    "Usando configurações de fabrica.",//txtLCFactory
    #ifdef HEADLESS
      "Acerca MicroTC361",//txtAbout
    #else
      "Acerca MicroTC360",//txtAbout
    #endif
    "Créditos:\n\n"
    "Agradecemos a \ntodos os que \ntornaram este \nprojeto possível:\n\n"
    "Rui Azevedo da \nr-site.net / \nArduinoMenu por \nsua ajuda na \nprogramação do \ncontrolador \nMicroTC360/1, \ntestes e \ndepuração\n\n"
    "Tobias Gawrisch \nda AquaOwner \npelas suas \nideias sobre a \nsérie LED \nUNIQ FS e a \najuda com o \ndesign\n\n"
    "Oliver L., da \nOcean Optics, \nque há muitos \nanos me apoia na \npesquisa de LEDs\n\n"
    "Kable, Gerente \nde Vendas da \nfábrica onde eu \nfabrico os meus \nLED's porque ele \nimplementa \nsoluções de LED \ncomigo há muitos \nanos e sempre me \napoia com \nconselhos e \nassistência.\n\n"
    "Meus pais que me \napoiarem por \nmuitos anos, não \napenas \nmoralmente \ndurante a \nimplementação de \nprojetos, mas \ntambém como uma \nfamília!\n\n"
    ,//txtAboutText
    "Config. de fábrica",//txtFactoryReset
    "Click p/confirmar",//txtFactoryResetConfirm
    "reset p/ignorar.",//txtFactoryResetIgnore
    "Ajuste horário",//txtTimeOffset,
    "Aplicar data/hora",//txtSetDateTime,
  };
};
