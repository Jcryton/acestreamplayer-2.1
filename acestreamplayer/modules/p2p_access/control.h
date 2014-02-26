#ifndef __P2P_PLUGIN_CONTROL__HEADER__
#define __P2P_PLUGIN_CONTROL__HEADER__

#include <p2p_object.h>
#include <string>
#include <vector>
#include <sstream>

#include "api/api.h"
#include "p2p_access.h"

#ifdef WIN32
#ifdef TORRENT_STREAM
const wchar_t REG_SECTION[] = L"Software\\TorrentStream";
#else
const wchar_t REG_SECTION[] = L"Software\\ACEStream";
#endif
const wchar_t REG_ENGINE_KEY[] = L"EnginePath";
const wchar_t REG_INSTALL_KEY[] = L"InstallDir";
const wchar_t PORT_FILE[] = L"\\engine\\acestream.port";
#else
const char UNIX_PATH[] = "/usr/bin/acestreamengine-client-gtk";
#endif

class Connection;
class DB;
class Control 
{
public:
    Control( p2p_object_t * );
    ~Control();
    
    bool isReady();
    bool isShutdown();
    
    bool startup();
    void shutdown();
    void ready();
    
    bool send( base_out_message* );
    bool sendString( std::string& );
    base_in_message *sendSync( base_out_message* );
    bool receive( std::string& );
    void processEngineMessage( base_in_message* );
    int processEngineLoadMessage( load_in_msg*, async_loading_item );
    
    bool dbSaveOption( std::string, std::string, std::string );
    bool supportedStatEvents() { return m_version_options.support_stat_events; }
    bool supportedTenAgesUserinfo() { return m_version_options.support_ten_ages_userinfo; }
    
    void requestPauseInteractiveAd();
    void requestNonLinearInteractiveAd();
    void requestStopInteractiveAd();
    
    void registerCloseInteractiveAd(std::string);
    void registerShowInteractiveAd(std::string);
    
    void clearPauseItems();
    void clearNonLinearItems();
    void clearStopItems();
    
private:
#ifdef WIN32
    bool readPortFromFile();
    int readFile( std::string );
#endif
    bool processConnect2Engine();
    bool helloEngine();
    bool startEngine();
    
    void versionProcess( int, int, int, int );
    
    void preparePauseItems();
    void prepareNonLinearItems();
    void prepareStopItems();
    
private:
    p2p_object_t *m_vlcobj;
    Connection *m_connection;
    DB *m_db;
    bool m_closing;
    bool m_ready;
    bool m_shutdown;
    bool m_db_enabled;
    std::string m_ready_key;
    bool m_remote_engine;
    bool m_nonlinear_requested;
    
    std::vector<preload_pause_ad_item> m_pause_items;
    std::vector<preload_nonlinear_ad_item> m_nonlinear_items;
    std::vector<preload_stop_ad_item> m_stop_items;
    
    struct version_options {
        bool support_stat_events;
        bool support_ten_ages_userinfo;
    } m_version_options;
};

#endif