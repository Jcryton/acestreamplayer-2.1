#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <p2p_object.h>
#include <vlc_charset.h>

#include "control.h"
#include "connection.h"
#include "db.h"
#include "developer.h"
#include "api/out.h"
#include "api/in.h"

#ifndef WIN32
#include <sys/stat.h>
#include <errno.h>
#endif

using namespace std;

Control::Control( p2p_object_t *vlc_obj )
    : m_vlcobj(vlc_obj)
    , m_connection(NULL)
    , m_db(NULL)
    , m_closing(false)
    , m_ready(false)
    , m_shutdown(false)
    , m_db_enabled(false)
    , m_ready_key("")
    , m_remote_engine(false)
    , m_nonlinear_requested(false)
{
    versionProcess( 0, 0, 0, 0 );
    
    string _host = var_CreateGetString(vlc_obj, "ace-host");
    int _port = var_CreateGetInteger(vlc_obj, "ace-port");
    
    m_remote_engine = !_host.empty() && _port != 0;
    
    if(m_remote_engine) {
        msg_P2PLog( m_vlcobj, "[control.cpp]: remote engine connection: %s : %d", _host.c_str(), _port );
        m_connection = new Connection( vlc_obj, _host, _port );
    }
    else
        m_connection = new Connection( vlc_obj );
    m_db = new DB( vlc_obj );
    
    m_db_enabled = m_db->connect();
    if( !m_db_enabled )
        msg_P2PLog( m_vlcobj, "[Control]: Without db..." );
}

Control::~Control()
{
    delete m_connection;
    delete m_db;
}

bool Control::isReady()
{
    return m_ready;
}

bool Control::isShutdown()
{
    return m_shutdown;
}

bool Control::startup()
{
    if( m_connection->connected() )
        return true;
    
    if(m_remote_engine) {
        if(m_connection->connect()) {
            return processConnect2Engine();
        }
        else {
            msg_Err( m_vlcobj, "[control.cpp::startup]: Cannot connect to engine." );
            return false;
        }
    }
    else {
        bool _b_has_port = true;
#ifdef WIN32
        _b_has_port = readPortFromFile();
#endif

        if( _b_has_port ) {
            bool _con = m_connection->connect();
            (void)_con;
        }
        if( !processConnect2Engine() ) {
            msg_Err( m_vlcobj,  "[Control]: Cannot connect to engine" );
            return false;
        }
    }
    return true;
}

void Control::shutdown()
{
    m_closing = true;
    shutdown_out_msg msg;
    send( &msg );
}

void Control::ready()
{
    ready_out_msg msg;
    msg.key = m_ready_key;
    msg.support_spaces = m_version_options.support_extra_spaces;
    if( send( &msg ) ) {
        m_ready = true;
        m_vlcobj->p_sys->state = P2P_STATE_IDLE;
        var_SetInteger( m_vlcobj, "state", P2P_STATE_IDLE );
    }
    m_ready_key = "";
}

bool Control::send( base_out_message *message )
{
    return m_connection->sendMsg( Out::Build( message ) );
}

bool Control::sendString( string &message )
{
    return m_connection->sendMsg( message );
}

base_in_message *Control::sendSync( base_out_message *message )
{
    string _response = "";
    if( m_connection->sendMsgSync( Out::Build( message ), _response ) ) {
        base_in_message *in_msg = NULL;
        if( message->type == OUT_MSG_LOAD )
            in_msg = In::ParseSyncLoad( _response );
        else if( message->type == OUT_MSG_GET_CID )
            in_msg = In::ParseSyncGetCid( _response );
        else if( message->type == OUT_MSG_GET_PID )
            in_msg = In::ParseSyncGetPid( _response );
        return in_msg;
    }
    return NULL;
}

bool Control::receive( string &engine_message )
{
    return m_connection->recvMsg( engine_message );
}

void Control::processEngineMessage( base_in_message *in_msg )
{
    switch( in_msg->type ) {
    case IN_MSG_NOTREADY: {
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_NOTREADY: ???" );
        break;
    }
    case IN_MSG_PLAY : {
        play_in_msg *play = static_cast<play_in_msg *>(in_msg);
        if( play->play_type != P2P_PLAY_UNDF ) {
            std::map<p2p_command_callback_type, std::pair<p2p_common_callback, void*> >::iterator it = m_vlcobj->p_sys->callbacks->find(P2P_PLAY_CALLBACK);
            if( it != m_vlcobj->p_sys->callbacks->end() ) {
                //if( play->play_type != P2P_PLAY_INTERRUPTABLE_AD && play->play_type != P2P_PLAY_AD )
                //    m_nonlinear_requested = false;
            
                p2p_play_item_t p_item;

                p_item.url = play->url.c_str();
                p_item.start_position = play->start_position;
                p_item.play_content_type = play->play_type;

                var_SetString( m_vlcobj, "clickurl", play->clickurl.c_str() );
                var_SetInteger( m_vlcobj, "advolume", play->volume );

                //var_SetString( m_vlcobj, "skipoffset", decodedskipoffset.c_str() );
                
                p2p_ad_params_t p_ad_params;
                p_ad_params.url = play->clickurl.c_str();
                p_ad_params.text = play->clicktext.c_str();
                p_ad_params.skipoffset = play->skipoffset.c_str();
                p_ad_params.noadsurl = play->noadsurl.c_str();
                
                p_ad_params.noadstext = play->noadstext.c_str();
                p_ad_params.adinfotext = play->adinfotext.c_str();
                var_SetAddress( m_vlcobj, "adparams", &p_ad_params );
                
                it->second.first( m_vlcobj, &p_item, it->second.second );
            }
            else {
                msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: no P2P_PLAY_CALLBACK specified" );
                break;
            }
        }
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_PLAY: url: %s position: %d type: %d", play->url.c_str(), play->start_position, play->play_type );
        break;
    }
    case IN_MSG_PAUSE : {
        std::map<p2p_command_callback_type, std::pair<p2p_common_callback, void*> >::iterator it = m_vlcobj->p_sys->callbacks->find(P2P_PAUSE_CALLBACK);
        if( it != m_vlcobj->p_sys->callbacks->end() )
            it->second.first( m_vlcobj, NULL, it->second.second );
        else {
            msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: no P2P_PAUSE_CALLBACK specified" );
            break;
        }
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_PAUSE" );
        break;
    }
    case IN_MSG_RESUME : {
        std::map<p2p_command_callback_type, std::pair<p2p_common_callback, void*> >::iterator it = m_vlcobj->p_sys->callbacks->find(P2P_RESUME_CALLBACK);
        if( it != m_vlcobj->p_sys->callbacks->end() )
            it->second.first( m_vlcobj, NULL, it->second.second );
        else {
            msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: no P2P_RESUME_CALLBACK specified" );
            break;
        }
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_RESUME" );
        break;
    }
    case IN_MSG_INFO : {
        info_in_msg *info = static_cast<info_in_msg *>(in_msg);
        var_SetString( m_vlcobj, "info", info->value.c_str() );
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_INFO: %s", info->value.c_str() );
        break;
    }
    case IN_MSG_SHUTDOWN : {
        if( !m_closing ) {
            m_vlcobj->p_sys->state = P2P_STATE_NOTLAUNCHED;
            var_SetInteger( m_vlcobj, "state", P2P_STATE_NOTLAUNCHED );
            m_vlcobj->p_sys->auth = 0;
            var_SetInteger( m_vlcobj, "auth", 0 );
            var_SetString( m_vlcobj, "info", "Engine stopped" );
            msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_SHUTDOWN: it seems to engine exited" );
        }
        else
            msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_SHUTDOWN: player exited" );
        m_shutdown = true;
        m_ready = false;
        break;
    }
    case IN_MSG_AUTH : {
        auth_in_msg *auth = static_cast<auth_in_msg *>(in_msg);
        m_vlcobj->p_sys->auth = auth->value;
        var_SetInteger( m_vlcobj, "auth", auth->value );
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: EV_AUTH: %d", auth->value );
        break;
    }
    case IN_MSG_STATUS : {
        status_in_msg *status = static_cast<status_in_msg *>(in_msg);
        if( status->is_error )
            var_SetString( m_vlcobj, "error", status->value.c_str() );
        else
            var_SetString( m_vlcobj, "status", status->value.c_str() );
        var_SetString( m_vlcobj, "status-raw", status->raw.c_str() );
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_STATUS: %s", status->value.c_str() );
        break;
    }
    case IN_MSG_STATE : {
        state_in_msg *state = static_cast<state_in_msg *>(in_msg);
        m_vlcobj->p_sys->state = state->value;
        if( state->value == P2P_STATE_LOADING )
            var_SetString( m_vlcobj, "status", _("Loading...") );
        var_SetInteger( m_vlcobj, "state", state->value );
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_STATE: %d", state->value );
        break;
    }
    case IN_MSG_STOP : {
        std::map<p2p_command_callback_type, std::pair<p2p_common_callback, void*> >::iterator it = m_vlcobj->p_sys->callbacks->find(P2P_STOP_CALLBACK);
        if( it != m_vlcobj->p_sys->callbacks->end() )
            it->second.first( m_vlcobj, NULL, it->second.second );
        else {
            msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: no P2P_STOP_CALLBACK specified" );
            break;
        }
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_STOP" );
        break;
    }
    case IN_MSG_LOAD : {
        // load message don't process here
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_LOAD: ???" );
        break;
    }
    case IN_MSG_EVENT : {
        event_in_msg *event = static_cast<event_in_msg *>(in_msg);
        switch( event->event_type ) {
        case IN_EVENT_MSG_GET_USER_DATA : {
                user_data_in_event_msg *userdata = event->event.user_data_event;
                var_TriggerCallback( m_vlcobj, "show-userdata-dialog" );
                delete userdata;
            }
            break;
        case IN_EVENT_MSG_CANSAVE : {
                cansave_in_event_msg *cansave = event->event.cansave_event;
                
                std::map<p2p_command_callback_type, std::pair<p2p_common_callback, void*> >::iterator it = m_vlcobj->p_sys->callbacks->find(P2P_CANSAVE_CALLBACK);
                if( it != m_vlcobj->p_sys->callbacks->end() ) {
                    p2p_cansave_item_t p_item;
                    p_item.infohash = cansave->infohash.c_str();
                    p_item.index = cansave->index;
                    p_item.format = cansave->format;
                    
                    it->second.first( m_vlcobj, &p_item, it->second.second );
                }
                else {
                    msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: no P2P_CANSAVE_CALLBACK specified" );
                    delete cansave;
                    break;
                }
                delete cansave;
            }
            break;
        case IN_EVENT_MSG_LIVE_POS : {
                live_pos_in_event_msg *live_pos = event->event.live_pos_event;
                
                vlc_value_t var;
                if( var_Get( m_vlcobj, "livepos", &var ) != VLC_SUCCESS ) {
                    msg_P2PLog(m_vlcobj, "[processEngineMessage]: cannot find p2p_livepos variable to set" );
                    delete live_pos;
                    return;
                }
                var.p2p_livepos.is_live = live_pos->is_live;
                var.p2p_livepos.live_first = live_pos->live_first;
                var.p2p_livepos.live_last = live_pos->live_last;
                var.p2p_livepos.pos = live_pos->pos;
                var.p2p_livepos.first_ts = live_pos->first_ts;
                var.p2p_livepos.last_ts = live_pos->last_ts;
                var.p2p_livepos.last = live_pos->last;
                var.p2p_livepos.buffer_pieces = live_pos->buffer_pieces;
                var_Set( m_vlcobj, "livepos", var );
                delete live_pos;
            }
            break;
        case IN_EVENT_MSG_SHOW_URL : {
                show_url_in_event_msg *show_url = event->event.url_event;
                p2p_showurl_item_t p_item;
                p_item.url = show_url->url.c_str();
                p_item.text = show_url->text.c_str();
                p_item.type = show_url->type;
                p_item.width = show_url->width;
                p_item.height = show_url->height;
                p_item.left = show_url->left;
                p_item.top = show_url->top;
                p_item.right = show_url->right;
                p_item.bottom = show_url->bottom;
                var_SetAddress( m_vlcobj, "showurl", &p_item );
                delete show_url;
            }
            break;
        case IN_EVENT_MSG_SHOW_DIALOG : {
                show_dialog_in_event_msg *show_dialog = event->event.dialog_event;
                p2p_showdialog_item_t p_item;
                p_item.title = show_dialog->title.c_str();
                
                size_t pos = show_dialog->text.find("{{provider_name}}");
                if(pos != string::npos)
                    show_dialog->text.replace(pos, 17, show_dialog->provider_name);

                pos = show_dialog->text.find("{{provider_url}}");
                if(pos != string::npos)
                    show_dialog->text.replace(pos, 16, show_dialog->provider_url);

                pos = show_dialog->text.find("{{premium_activate_url}}");
                if(pos != string::npos)
                    show_dialog->text.replace(pos, 24, show_dialog->premium_activate_url);
                p_item.text = show_dialog->text.c_str();
                
                var_SetAddress( m_vlcobj, "showdialog", &p_item );
                //var_SetString( m_vlcobj, "show-error-dialog", decodedtext.c_str() );
                delete show_dialog;
            }
            break;
        }
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_EVENT: type: %d", event->event_type );
        break;
    }
    case IN_MSG_PRELOAD_PAUSE_ADS : {
        preload_pause_ads_in_msg *pause_ads = static_cast<preload_pause_ads_in_msg *>(in_msg);
        m_pause_items.clear();
        m_pause_items = pause_ads->items;
        preparePauseItems();
        requestPauseInteractiveAd();
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_PRELOAD_PAUSE_ADS: items_count=%d", pause_ads->items.size() );
        break;
    }
    case IN_MSG_PRELOAD_NONLINEAR_ADS : {
        preload_nonlinear_ads_in_msg *nonlinear_ads = static_cast<preload_nonlinear_ads_in_msg *>(in_msg);
        m_nonlinear_items.clear();
        m_nonlinear_items = nonlinear_ads->items;
        prepareNonLinearItems();
        requestNonLinearInteractiveAd();
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_PRELOAD_NONLINEAR_ADS: items_count=%d", nonlinear_ads->items.size() );
        break;
    }
    case IN_MSG_PRELOAD_STOP_ADS : {
        preload_stop_ads_in_msg *stop_ads = static_cast<preload_stop_ads_in_msg *>(in_msg);
        m_stop_items.clear();
        m_stop_items = stop_ads->items;
        prepareStopItems();
        requestStopInteractiveAd();
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_PRELOAD_STOP_ADS: items_count=%d", stop_ads->items.size() );
        break;
    }
    default :
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineMessage]: IN_MSG_UNDF: unknown income message, maybe empty..." );
    }
}

int Control::processEngineLoadMessage( load_in_msg *load, async_loading_item load_item )
{
    std::map<p2p_command_callback_type, std::pair<p2p_common_callback, void*> >::iterator callbackIt = m_vlcobj->p_sys->callbacks->find(P2P_LOAD_CALLBACK);
    if( callbackIt == m_vlcobj->p_sys->callbacks->end() ) {
        msg_P2PLog(m_vlcobj, "[control.cpp::processEngineLoadMessage]: no P2P_LOAD_CALLBACK specified" );
        return VLC_EGENERIC;
    }
    
    int group = generate_new_group_id(m_vlcobj);
    int ret = VLC_EGENERIC;
    if( group <= 0 )
        return VLC_EGENERIC;

    p2p_load_item_t p_event_item;

    input_item_t *p_input_item = input_item_New( load_item.id.c_str(), NULL );
    if( !p_input_item )
        return VLC_EGENERIC;
    
    if( load ) {
        if( load->files.size() > 1 ) {
            var_TriggerCallback( m_vlcobj, "show-playlist" );
        }
        for( map<string, int>::iterator it = load->files.begin(); it != load->files.end(); ++it ) {
            p2p_data_t *p_p2pdt = input_item_NewP2PData( NULL );
            p_p2pdt->psz_url = strdup( load_item.id.c_str() );
            p_p2pdt->psz_infohash = strdup( load->infohash.c_str() );
            p_p2pdt->psz_checksum = strdup( load->checksum.c_str() );
            
            for( vector<string>::iterator qit = load->qualities.begin(); qit != load->qualities.end(); ++qit ) {
                string qualit = *qit;
                input_item_AddP2PQuality( p_p2pdt, qualit.c_str() );
            }
            
            bool b_def_quality_ok = false;
            if(load->default_quality >= 0 && load->default_quality < load->qualities.size()) {
                p_p2pdt->i_current_quality = load->default_quality;
                b_def_quality_ok = true;
            }
            else
                p_p2pdt->i_current_quality = 0;

            p_p2pdt->i_type = load_item.type;
            p_p2pdt->i_group = group;
            p_p2pdt->i_group_size = load->files.size();
            p_p2pdt->i_index = it->second;
            p_p2pdt->i_developer = load_item.developer;
            p_p2pdt->i_affiliate = load_item.affiliate;
            p_p2pdt->i_zone = load_item.zone;            
            string name = it->first;
            p_p2pdt->psz_filename = strdup( name.c_str() );
            
            if( it == load->files.begin() ) {
                if( load_item.options.size() > 0 ) {
                    for( size_t i = 0; i < load_item.options.size(); ++i ) {
                        if( !load_item.options.at(i).empty() )
                            input_item_AddOption( p_input_item, load_item.options.at(i).c_str(), VLC_INPUT_OPTION_TRUSTED );
                    }
                }
                
                if( m_db && m_db_enabled ) {
                    map<string, string> saved_options = m_db->retrieve( load->infohash );
                    if( saved_options.size() > 0 ) {
                        for( map<string, string>::iterator optit = saved_options.begin(); optit != saved_options.end(); ++optit ) {
                            if( !b_def_quality_ok && optit->first == "quality-id" && load->qualities.size() > 0) {
                                msg_P2PLog( m_vlcobj, "[processEngineLoadMessage]: restoring quality %s", optit->second.c_str() );
                                p_p2pdt->i_current_quality = atoi( optit->second.c_str() );
                            }
                            else {
                                bool b_found = false;
                                for( size_t i = 0; i < load_item.options.size(); ++i ) {
                                    if( load_item.options.at(i).find( optit->first ) != string::npos ) {
                                        b_found = true;
                                        break;
                                    }
                                }
                                if( !b_found ) {
                                    string opt = "";
                                    opt.append(":").append(optit->first).append("=").append(optit->second);
                                    msg_P2PLog( m_vlcobj, "[processEngineLoadMessage]: restoring option %s", opt.c_str() );
                                    input_item_AddOption( p_input_item, opt.c_str(), VLC_INPUT_OPTION_TRUSTED );
                                }
                            }
                        }
                    }
                }
            }

            if( it == load->files.begin() && load_item.name.compare("") ) {
                input_item_SetName( p_input_item, load_item.name.c_str() );
            }
            else {
                //input_item_SetDecodedName( p_input_item, name.c_str() );
                input_item_SetName( p_input_item, name.c_str() );
            }
            input_item_SetP2PData( p_input_item, p_p2pdt );
        
            p_event_item.add_to_playlist = load_item.playlist;
            p_event_item.add_mode = load_item.add_mode;
            p_event_item.need_start = m_vlcobj->p_sys->will_play;
            p_event_item.desktop_add_type = load_item.add_type;
            p_event_item.p_parent_node = load_item.parent;
            p_event_item.p_input_item = p_input_item;
            p_event_item.object_locked = load_item.locked;
            
            msg_P2PLog(m_vlcobj, "[processEngineLoadMessage]: adding %s with index %d and name %s and group %d", load_item.id.c_str(), it->second, name.c_str(), group );
            ret = callbackIt->second.first( m_vlcobj, &p_event_item, callbackIt->second.second );

            if(m_vlcobj->p_sys->will_play)
                m_vlcobj->p_sys->will_play = false;
        }
    }
    else if( load_item.type == P2P_TYPE_EFILE || load_item.type == P2P_TYPE_DIRECT_URL ) {
        p2p_data_t *p_p2pdt = input_item_NewP2PData( NULL );
        p_p2pdt->psz_url = strdup( load_item.id.c_str() );
        p_p2pdt->i_type = load_item.type;
        p_p2pdt->i_group = group;
        p_p2pdt->i_group_size = 1;
        p_p2pdt->i_index = 0;
        p_p2pdt->i_developer = load_item.developer;
        p_p2pdt->i_affiliate = load_item.affiliate;
        p_p2pdt->i_zone = load_item.zone;

        if( load_item.options.size() > 0 ) {
            for( size_t i = 0; i < load_item.options.size(); ++i ) {
                if( !load_item.options.at(i).empty() )
                    input_item_AddOption( p_input_item, load_item.options.at(i).c_str(), VLC_INPUT_OPTION_TRUSTED );
            }
        }
            
        input_item_SetURI( p_input_item, load_item.id.c_str() );
        if( load_item.name != "" )
            input_item_SetName( p_input_item, load_item.name.c_str() );
        input_item_SetP2PData( p_input_item, p_p2pdt );
    
        p_event_item.add_to_playlist = load_item.playlist;
        p_event_item.add_mode = load_item.add_mode;
        p_event_item.need_start = m_vlcobj->p_sys->will_play;
        p_event_item.desktop_add_type = load_item.add_type;
        p_event_item.p_parent_node = load_item.parent;
        p_event_item.p_input_item = p_input_item;
        p_event_item.object_locked = load_item.locked;

        msg_P2PLog(m_vlcobj, "[processEngineLoadMessage]: adding %s with index %d and name %s and group %d", load_item.id.c_str(), 0, load_item.name.c_str(), group );
        ret = callbackIt->second.first( m_vlcobj, &p_event_item, callbackIt->second.second );
        
        if(m_vlcobj->p_sys->will_play)
            m_vlcobj->p_sys->will_play = false;
    }
    else {
        msg_P2PLog(m_vlcobj, "[processEngineLoadMessage]: trying process unknown load" );
    }
    vlc_gc_decref( p_input_item );
    var_SetInteger( m_vlcobj, "state", P2P_STATE_IDLE );
    
    return ret;
}

bool Control::dbSaveOption( string infohash, string name, string value )
{
    bool ret = false;
    if( m_db && m_db_enabled )
        ret = m_db->save( infohash, name, value );
    return ret;
}

#ifdef WIN32
bool Control::readPortFromFile()
{
    static string _port_file = "";
    bool _b_readed = false;
    
    if( _port_file.empty() ) {
        LONG _result; 
        HKEY _hKey; 
        wchar_t _installpathW[1024];
        DWORD _length = sizeof( _installpathW );
        
        _result = RegOpenKeyEx( HKEY_CURRENT_USER, REG_SECTION, 0, KEY_QUERY_VALUE, &_hKey );
        if( _result != ERROR_SUCCESS ) {
            RegCloseKey( _hKey );
            _result = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_SECTION, 0, KEY_QUERY_VALUE, &_hKey );
            if( _result != ERROR_SUCCESS ) {
                RegCloseKey( _hKey );
                msg_P2PLog(m_vlcobj, "[control.cpp::readPortFromFile]: Error: cannot find reg_section");
                return false;
            }
        }
        _result = RegQueryValueEx( _hKey, REG_INSTALL_KEY, NULL, NULL, (LPBYTE)_installpathW, &_length );
        if( _result != ERROR_SUCCESS ) {
            RegCloseKey( _hKey );
            msg_P2PLog(m_vlcobj, "[control.cpp::readPortFromFile]: Error: cannot find reg_install_key");
            return false;
        }
        RegCloseKey( _hKey );
        
        char* _installpath = FromWide(_installpathW);
        if( _installpath ) {
            _port_file.assign( _installpath );
            _port_file.append( PORT_FILE );
            
            msg_P2PLog(m_vlcobj, "[control.cpp::readPortFromFile]: portfile = %s", _port_file.c_str());

            free(_installpath);
        }
    }
    
    if( !_port_file.empty() ) {
        msg_P2PLog(m_vlcobj, "[control.cpp::readPortFromFile]: Trying read portfile" );
        int _portnum = readFile( _port_file );
        if( _portnum > 0 ) {
            msg_P2PLog(m_vlcobj, "[control.cpp::readPortFromFile]: Port = %d", _portnum);
            
            _b_readed = (bool)_portnum;
            if( _b_readed )
                m_connection->setPort( _portnum );
        }
        else {
            msg_P2PLog(m_vlcobj, "[control.cpp::readPortFromFile]: Cannot read portfile" );
        }
    }
    
    return _b_readed;
}

int Control::readFile( string filepath )
{
    int _dataint = -1;
    FILE* _file = fopen( filepath.c_str(), "r" );
    if( _file ) {
        long _file_size; 
         fseek( _file , 0 , SEEK_END );
        _file_size = ftell( _file );
        rewind( _file );
        if( _file_size > 0 ) {
            fscanf(_file, "%d", &_dataint);
        }
        fclose( _file );
    }
    return _dataint;
}
#endif

bool Control::processConnect2Engine()
{
    bool _b_ok = false;
    if( m_connection->connected()) {
        _b_ok = helloEngine();
        if( m_remote_engine )
            _b_ok = true;
    }
	else
        _b_ok = startEngine();

    if( _b_ok ) {
        if( !m_connection->connected() ) {
            bool _b_connect = false;
            int _try_counter = 1200; // takes ~2min
            
            while( !_b_connect && !m_closing ) {
#ifdef WIN32
                _b_connect = readPortFromFile() && m_connection->connect();
#else
                _b_connect = m_connection->connect();
#endif
                --_try_counter;
                if( _try_counter == 0 )
                    break;
                msleep(100000);
            }

            if( !_b_connect ) {
                m_connection->disconnect();
                msg_P2PLog(m_vlcobj, "[control.cpp::processConnect2Engine]:  Cannot connect to engine" );
                _b_ok = false;
            }
            else {
                msg_P2PLog(m_vlcobj, "[control.cpp::processConnect2Engine]:  Connected in %d attempts", 1200 - _try_counter );
                _b_ok = helloEngine();
            }
        }
	}

    return _b_ok;
}

bool Control::helloEngine()
{
    hello_out_msg _msg;
    _msg.version = API_VERSION;
    string _hello_resp = "";
    if( !send( &_msg ) ) 
        msg_Err( m_vlcobj, "[Control]: Failed to send hellobg" );
    else if( !m_connection->recvMsg( _hello_resp ) )
	    msg_Err( m_vlcobj, "[Control]: Failed to receive hellots" );
    else {
        base_in_message *in_msg = In::Parse( _hello_resp );
        bool _ret = false;
        if( in_msg->type != IN_MSG_HELLO ) {
            msg_Err( m_vlcobj, "[Control]: incorrect hellots response \"%s.\"", _hello_resp.c_str());
            msg_P2PLog( m_vlcobj, "[control.cpp::helloEngine]:  incorrect hellots response: %s", _hello_resp.c_str() );
        }
        else {
            hello_in_msg *hello = static_cast<hello_in_msg *>(in_msg);
            var_SetString( m_vlcobj, "engine-version", hello->version.c_str() );
            versionProcess( hello->major, hello->minor, hello->build, hello->revision );
            msg_P2PLog( m_vlcobj, "[control.cpp::helloEngine]:  Support statistics events: %d", m_version_options.support_stat_events );
            msg_P2PLog( m_vlcobj, "[control.cpp::helloEngine]:  Support new format and ages start : %d", m_version_options.support_ten_ages_userinfo );
            //msg_P2PLog( m_vlcobj, "[control.cpp::helloEngine]:  Support extra spaces : %d", m_version_options.support_extra_spaces );
            
            if( hello->hello_key != "" ) {
                string dev_key_str = "";
                char *developer_key = var_CreateGetString(m_vlcobj, "ace-developer-key");
                if(developer_key) {
                    dev_key_str = string(developer_key);
                    free(developer_key);
                }
                
                if(dev_key_str.length() > 0) {
                    vector<string> parts = In::split(dev_key_str, '-');
                    m_ready_key = parts[0];
                    
                    string to_hash = hello->hello_key;
                    to_hash.append(dev_key_str);
                    char *hash = sha1(to_hash.c_str());
                    m_ready_key.append("-").append(hash);
                    free(hash);
                }
                else {
                    m_ready_key = "";
                    msg_Err(m_vlcobj, "No ace-developer-key specified!!!!");
                    msg_P2PLog( m_vlcobj, "[control.cpp::helloEngine]:  No ace-developer-key specified!!!!" );
                }
                /*size_t dev_len = sizeof(developer_key) / sizeof(short);
                char *dev_key = developer_to_string( developer_key, &dev_len );
                string dev_key_str = string(dev_key);
                free(dev_key);
                if( dev_len>0 ) {
                    vector<string> parts = In::split(dev_key_str, '-');
                    m_ready_key = parts[0];
                    
                    string to_hash = hello->hello_key;
                    to_hash.append(dev_key_str);
                    char *hash = sha1(to_hash.c_str());
                    m_ready_key.append("-").append(hash);
                    free(hash);
                }*/
            }
            else
                m_ready_key = "";
            _ret = true;
        }
        
        delete in_msg;
        return _ret;
    }
	return false;
}

bool Control::startEngine()
{
    bool _b_started = false;
    
#ifdef WIN32
    LONG _result;
    HKEY _hKey;
    wchar_t _enginepath[1024];
    DWORD _length = sizeof( _enginepath );
    
	_result = RegOpenKeyEx( HKEY_CURRENT_USER, REG_SECTION, 0, KEY_QUERY_VALUE, &_hKey );
    if( _result != ERROR_SUCCESS ) {
        RegCloseKey( _hKey );
        
        _result = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_SECTION, 0, KEY_QUERY_VALUE, &_hKey );
        if( _result != ERROR_SUCCESS ) {
            RegCloseKey( _hKey );
            msg_P2PLog(m_vlcobj, "[control.cpp::startEngine]: Error: cannot find reg_section");
            return false;
        }
    }
    
    _result = RegQueryValueEx( _hKey, REG_ENGINE_KEY, NULL, NULL, (LPBYTE)_enginepath, &_length );
    if( _result != ERROR_SUCCESS ) {
        RegCloseKey( _hKey );
        msg_P2PLog(m_vlcobj, "[control.cpp::startEngine]: Error: cannot find reg_engine_key");
        return false;
    }
    RegCloseKey( _hKey );
    
    STARTUPINFOW _startup_info;
    PROCESS_INFORMATION _process_info;
    memset( &_startup_info, 0, sizeof( _startup_info ) );
    memset( &_process_info, 0, sizeof( _process_info ) );
    _startup_info.cb = sizeof( _startup_info );
    
    msg_P2PLog(m_vlcobj, "[control.cpp::startEngine]: Starting engine");
    _b_started = CreateProcess( _enginepath, NULL, NULL,
                               NULL, false, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE,
                               NULL, NULL, &_startup_info, &_process_info );

    CloseHandle( _process_info.hProcess );
    CloseHandle( _process_info.hThread );
#else
    struct stat _sts;
    //char *_enginepath = UNIX_PATH;
    if( stat(UNIX_PATH, &_sts) != -1 && errno != ENOENT ) {
        char *_args[] = { (char *)UNIX_PATH, 0 };
        pid_t _sid = 0;
        pid_t _pid = fork();

        if( _pid < 0 ) {
            msg_Err( m_vlcobj, "[Control]: Failed to fork, cannot start engines process..." );
        }
        else if( _pid > 0 ) {
            msg_P2PLog(m_vlcobj, "[control.cpp::startEngine]: Starting engine pid=%d", _pid);
            _b_started = true;
        }
        else {
            umask( 0 );
            _sid = setsid();
            if( _sid < 0 )
                exit(1);

            close( STDIN_FILENO );
            close( STDOUT_FILENO );
            close( STDERR_FILENO );
            
            execv( _args[0], _args );
        }
    }
    else{
        msg_Err( m_vlcobj, "[Control]: Error: Cannot find engine to start" );
    }
#endif
    return _b_started;
}

void Control::versionProcess( int major, int minor, int build, int revision )
{
    m_version_options.support_stat_events = false;
    m_version_options.support_ten_ages_userinfo = false;
    m_version_options.support_extra_spaces = false;
    
    if( major == 0 && minor == 0 && build == 0 && revision == 0)
        return;
    // stat events support only in >= 2.0.7.10
    if( major < 2 && minor < 0 && build < 7 && revision < 10 ) 
        return;
    m_version_options.support_stat_events = true;
    // 9,10 ages only in >= 2.0.8.5
    if( major < 2 && minor < 0 && build < 8 && revision < 5 ) 
        return;
    m_version_options.support_ten_ages_userinfo = true;
    // extra spasec only in >= 2.1.8.0
    if( major < 2 && minor < 1 && build < 8 && revision < 0 ) 
        return;
    m_version_options.support_extra_spaces = true;
}

void Control::preparePauseItems() {
    bool flash_enable = var_GetBool(m_vlcobj->p_libvlc, "flash-enable");
    
    if( !flash_enable ) {
        for( vector<preload_pause_ad_item>::iterator it = m_pause_items.begin(); it != m_pause_items.end(); ) {
            // if(it->min_width == 0)
                // it->min_width = it->width;

            // if(it->min_height == 0)
                // it->min_height = it->height;

            if(it->require_flash)
                m_pause_items.erase(it);
            else
                ++it;
        }
    }
}

void Control::prepareNonLinearItems() {
    bool flash_enable = var_GetBool(m_vlcobj->p_libvlc, "flash-enable");
    
    for( vector<preload_nonlinear_ad_item>::iterator it = m_nonlinear_items.begin(); it != m_nonlinear_items.end(); ) {
        // if(it->min_width == 0)
            // it->min_width = it->width;

        // if(it->min_height == 0)
            // it->min_height = it->height;

        if(it->require_flash && !flash_enable)
            m_nonlinear_items.erase(it);
        else if(it->ad_type.compare("iframe") != 0 && it->ad_type.compare("html") != 0)
            m_nonlinear_items.erase(it);
        else
            ++it;
    }
}

void Control::prepareStopItems() {
    bool flash_enable = var_GetBool(m_vlcobj->p_libvlc, "flash-enable");
    
    if( !flash_enable ) {
        for( vector<preload_stop_ad_item>::iterator it = m_stop_items.begin(); it != m_stop_items.end(); ) {
            // if(it->min_width == 0)
                // it->min_width = it->width;

            // if(it->min_height == 0)
                // it->min_height = it->height;

            if(it->require_flash)
                m_stop_items.erase(it);
            else
                ++it;
        }
    }
}

void Control::requestPauseInteractiveAd() {
    int w, h;    
    var_GetCoords(m_vlcobj, "vout-display-size", &w, &h);
    bool f = var_GetBool(m_vlcobj, "vout-display-fullscreen");
    
    if(m_pause_items.size() && w != 0 && h != 0) {
        int min_impressions = 9999;
        vector<preload_pause_ad_item> matched_items;
        vector<preload_pause_ad_item> matched_nonpreload_items;
        for(vector<preload_pause_ad_item>::iterator it = m_pause_items.begin(); it != m_pause_items.end(); ++it) {
            if((it->min_width == 0 || it->min_width <= w) && 
                (it->min_height == 0 || it->min_height <= h) && 
                (it->fullscreen == 0 
                    || (it->fullscreen == 1 && !f)
                    || (it->fullscreen == 2 && f)
                )) 
            {
                if(it->preload)
                    matched_items.push_back(*it);
                else
                    matched_nonpreload_items.push_back(*it);
                if(it->impressions < min_impressions)
                    min_impressions = it->impressions;
            }
        }
        
        preload_pause_ad_item selected_item;
        bool selected = false;
        for(vector<preload_pause_ad_item>::iterator it = matched_items.begin(); it != matched_items.end(); ++it) {
            if(it->impressions <= min_impressions) {
                selected_item = *it;
                selected = true;
                break;
            }
        }
        if(!selected) {
            for(vector<preload_pause_ad_item>::iterator it = matched_nonpreload_items.begin(); it != matched_nonpreload_items.end(); ++it) {
                if(it->impressions <= min_impressions) {
                    selected_item = *it;
                    selected = true;
                    break;
                }
            }
        }

        if(selected) {
            p2p_preload_pause_url_item_t p_item;
            p_item.url = selected_item.url.c_str();
            p_item.id = selected_item.id.c_str();
            p_item.preload = selected_item.preload;
            p_item.width = selected_item.width;
            p_item.height = selected_item.height;
            p_item.left = selected_item.left;
            p_item.top = selected_item.top;
            p_item.right = selected_item.right;
            p_item.bottom = selected_item.bottom;
            p_item.allow_dialogs = selected_item.allow_dialogs;
            p_item.enable_flash = selected_item.enable_flash;
            p_item.cookies = selected_item.cookies;
            p_item.embed_script = selected_item.embed_script.c_str();
            var_SetAddress( m_vlcobj, "preload-pause-url", &p_item );
        }
        else {
            var_SetAddress( m_vlcobj, "preload-pause-url", NULL );
        }
        matched_items.clear();
        matched_nonpreload_items.clear();
    }
}

void Control::requestNonLinearInteractiveAd() {
    //int w, h;
    //var_GetCoords(m_vlcobj, "vout-display-size", &w, &h);
    
    if(m_nonlinear_items.size() /*&& w != 0 && h != 0*/) {
        preload_nonlinear_ad_item selected_item;
        bool selected = false;
        for(vector<preload_nonlinear_ad_item>::iterator it = m_nonlinear_items.begin(); it != m_nonlinear_items.end(); ++it) {
            if((!it->ad_type.compare("iframe") || !it->ad_type.compare("html"))
                && it->height != 0 /*&& it->width <= w && it->height <= h * 0.5*/) {
                selected_item = *it;
                selected = true;
                break;
            }
        }

        if(selected) {
            p2p_preload_nonlinear_url_item_t p_item;
            p_item.url = selected_item.url.c_str();
            p_item.id = selected_item.id.c_str();
            p_item.type = selected_item.ad_type.c_str();
            p_item.creative_type = selected_item.creative_type.c_str();
            p_item.click_url = selected_item.click_url.c_str();

            p_item.width = selected_item.width;
            p_item.height = selected_item.height;
            p_item.left = selected_item.left;
            p_item.top = selected_item.top;
            p_item.right = selected_item.right;
            p_item.bottom = selected_item.bottom;
            p_item.allow_dialogs = selected_item.allow_dialogs;
            p_item.enable_flash = selected_item.enable_flash;
            p_item.cookies = selected_item.cookies;
            p_item.embed_script = selected_item.embed_script.c_str();
            var_SetAddress( m_vlcobj, "preload-nonlinear-url", &p_item );
        }
        else {
            var_SetAddress( m_vlcobj, "preload-nonlinear-url", NULL );
        }
    }
}

void Control::requestStopInteractiveAd() {
    int w, h;
    var_GetCoords(m_vlcobj, "vout-display-size", &w, &h);
    bool f = var_GetBool(m_vlcobj, "vout-display-fullscreen");
    
    if(m_stop_items.size() && w != 0 && h != 0) {
        vector<preload_stop_ad_item> matched_items;
        vector<preload_stop_ad_item> matched_nonpreload_items;
        for(vector<preload_stop_ad_item>::iterator it = m_stop_items.begin(); it != m_stop_items.end(); ++it) {
            if((it->min_width == 0 || it->min_width <= w) && 
                (it->min_height == 0 || it->min_height <= h) && 
                (it->fullscreen == 0 
                    || (it->fullscreen == 1 && !f)
                    || (it->fullscreen == 2 && f)
                )) 
            {
                if(it->preload) {
                    matched_items.push_back(*it);
                }
                else {
                    matched_nonpreload_items.push_back(*it);
                }
            }
        }
        
        preload_stop_ad_item selected_item;
        bool selected = false;
        if(matched_items.size() > 0) {
            selected_item = matched_items.front();
            selected = true;
        }
        if(!selected) {
            if(matched_nonpreload_items.size() > 0) {
                selected_item = matched_nonpreload_items.front();
                selected = true;
            }
        }

        if(selected) {
            p2p_preload_stop_url_item_t p_item;
            p_item.url = selected_item.url.c_str();
            p_item.id = selected_item.id.c_str();
            p_item.preload = selected_item.preload;
            p_item.fullscreen = selected_item.fullscreen;
            p_item.width = selected_item.width;
            p_item.height = selected_item.height;
            p_item.left = selected_item.left;
            p_item.top = selected_item.top;
            p_item.right = selected_item.right;
            p_item.bottom = selected_item.bottom;
            p_item.allow_dialogs = selected_item.allow_dialogs;
            p_item.enable_flash = selected_item.enable_flash;
            p_item.cookies = selected_item.cookies;
            p_item.embed_script = selected_item.embed_script.c_str();
            var_SetAddress( m_vlcobj, "preload-stop-url", &p_item );
        }
        else {
            var_SetAddress( m_vlcobj, "preload-stop-url", NULL );
        }
        matched_items.clear();
        matched_nonpreload_items.clear();
    }
}

void Control::registerShowInteractiveAd(string id) {
    if(id != "") {
        if(m_pause_items.size()) {
            for(vector<preload_pause_ad_item>::iterator it = m_pause_items.begin(); it != m_pause_items.end(); ) {
                if(!id.compare(it->id)) {
                    it->impressions += 1;
                    if(it->impressions >= it->max_impressions)
                        m_pause_items.erase(it);
                    break;
                }
                else
                    ++it;
            }
        }
        
        nonlinear_ad_event_out_msg _msg;
        _msg.event_type = std::string("impression");
        char *enc_id = encode_URI_component(id.c_str());
        _msg.id = std::string(enc_id);
        if( !send( &_msg ) ) {
            msg_P2PLog(m_vlcobj, "[p2p_access.cpp::registerImpression] sending impression event error");
        }
        free(enc_id);
    }
}

void Control::registerCloseInteractiveAd(string id) {
    if(id != "") {
        nonlinear_ad_event_out_msg _msg;
        _msg.event_type = std::string("close");
        char *enc_id = encode_URI_component(id.c_str());
        _msg.id = std::string(enc_id);
        if( !send( &_msg ) ) {
            msg_P2PLog(m_vlcobj, "[p2p_access.cpp::registerClose] sending close event error");
        }
        free(enc_id);
    }
}

void Control::clearPauseItems() {
    m_pause_items.clear();
}

void Control::clearNonLinearItems() {
    m_nonlinear_items.clear();
}

void Control::clearStopItems() {
    m_stop_items.clear();
}