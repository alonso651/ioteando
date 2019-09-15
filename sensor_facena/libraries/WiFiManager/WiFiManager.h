/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.center {text-align: center;} .c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;.l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><div style=\"text-align:left;display:inline-block;min-width:260px;\"><div class=\"center\"><img alt=\"logoGICS\" src=\"data:image/jpeg;base64,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\"></div><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>guardar</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Parametros guardados<br />Tratando de conectar a la red..<br />Si falla reconecte y vuelva a intentar</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();
    String        getSSID();
    String        getPassword(); 

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    int                    _max_params;
    WiFiManagerParameter** _params;

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
