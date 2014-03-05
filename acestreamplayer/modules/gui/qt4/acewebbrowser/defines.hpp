#ifndef AWB_DEFINES_H
#define AWB_DEFINES_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <p2p_object.h>

#include <QMetaType>
#include <QDebug>

namespace AceWebBrowser {

enum BrowserType {
    BTYPE_UNDEFINED = -1,
    BTYPE_PAUSE = 0,
    BTYPE_STOP,
    BTYPE_OVERLAY,
    BTYPE_PREROLL,
    BTYPE_SLIDER,
    BTYPE_HIDDEN
};

enum BrowserCookies {
    BCOOK_OUR = 1,
    BCOOK_ALL,
    BCOOK_NONE
};

enum BrowserUserAgent {
    BUA_OUR = 1,
    BUA_HOST
};

enum BrowserHolderPlayerState {
    BHPS_NONE = 0,
    BHPS_PLAYING = 2,
    BHPS_PAUSED = 3,
    BHPS_STOPPED = 4,
    BHPS_ERROR = 5
};

#define USER_AGENT "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/534.34 (KHTML, like Gecko) Qt/"QT_VERSION_STR" AceStream/"P2P_STD_VERSION"";

#define ADD_JS_SCRIPT_JS "" \
    "(function(){" \
    "  function embed(tries) {" \
    "      if(tries <= 0) return;" \
    "      if(!document || !document.head) {" \
    "          setTimeout(function() { embed(tries-1); }, 500);" \
    "          return;" \
    "      }" \
    "      var scripts = document.head.getElementsByTagName('script');" \
    "      var has_script = false;" \
    "      for(var i = 0; i < scripts.length; i++) {" \
    "          if(scripts[i].getAttribute('src') === '%SCRIPT_URL%') { has_script = true; }" \
    "      }" \
    "      if(!has_script) {" \
    "          var jsref = document.createElement('script');" \
    "          jsref.setAttribute('type','text/javascript');" \
    "          jsref.setAttribute('src', '%SCRIPT_URL%');" \
    "          document.head.appendChild(jsref);" \
    "      }" \
    "  }" \
    "  embed(50);" \
    "})();"

#define ADD_JS_CODE_JS "" \
    "(function(){" \
    "    function embed(tries) {" \
    "        if(tries <= 0) return;" \
    "        if(!document || !document.head) {" \
    "            setTimeout(function() { embed(tries-1); }, 500);" \
    "            return;" \
    "        }" \
    "        var js = document.createElement('script');" \
    "        js.setAttribute('type', 'text/javascript');" \
    "        js.innerHTML = '%SCRIPT_CODE%';" \
    "        document.head.appendChild(js);" \
    "    }" \
    "    embed(50);" \
    "})();"

}

Q_DECLARE_METATYPE(AceWebBrowser::BrowserType)

#endif // AWB_DEFINES_H
