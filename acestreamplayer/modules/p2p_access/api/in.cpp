#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <p2p_object.h>
#include <vlc_url.h>
#include "in.h"
#include <sstream>  

using namespace std;

base_in_message *In::Parse( const string &msg )
{
    base_in_message *_msg;

    if( !msg.compare( 0, 8, "NOTREADY" ) )
        _msg = static_cast<base_in_message *>( In::notready( msg ) );
    else if( !msg.compare( 0, 7, "HELLOTS" ) )
        _msg = static_cast<base_in_message *>( In::hello( msg ) );
    else if( !msg.compare( 0, 4, "AUTH" ) )
        _msg = static_cast<base_in_message *>( In::auth( msg ) );
    else if( !msg.compare( 0, 6, "STATUS" ) )
        _msg = static_cast<base_in_message *>( In::status( msg ) );
    else if( !msg.compare( 0, 5, "STATE" ) )
        _msg = static_cast<base_in_message *>( In::state( msg ) );
    else if( !msg.compare( 0, 4, "INFO" ) )
        _msg = static_cast<base_in_message *>( In::info( msg ) );
    else if( !msg.compare( 0, 5, "START" ) )
        _msg = static_cast<base_in_message *>( In::start( msg ) );
    else if( !msg.compare( 0, 4, "PLAY" ) )
        _msg = static_cast<base_in_message *>( In::play( msg ) );
    else if( !msg.compare( 0, 6, "PLAYAD" ) )
        _msg = static_cast<base_in_message *>( In::play_ad( msg ) );
    else if( !msg.compare( 0, 7, "PLAYADI" ) )
        _msg = static_cast<base_in_message *>( In::play_interruptable_ad( msg ) );
    else if( !msg.compare( 0, 5, "PAUSE" ) )
        _msg = static_cast<base_in_message *>( In::pause( msg ) );
    else if( !msg.compare( 0, 6, "RESUME" ) )
        _msg = static_cast<base_in_message *>( In::resume( msg ) );
    else if( !msg.compare( 0, 4, "STOP" ) )
        _msg = static_cast<base_in_message*>( In::stop( msg ) );
    else if( !msg.compare( 0, 8, "LOADRESP" ) )
        _msg = static_cast<base_in_message *>( In::load( msg ) );
    else if( !msg.compare( 0, 8, "SHUTDOWN" ) )
        _msg = static_cast<base_in_message *>( In::shutdown( msg ) );
    else if( !msg.compare( 0, 5, "EVENT" ) )
        _msg = static_cast<base_in_message*>( In::event( msg ) );
    else if( !msg.compare( 0, 8, "LOAD_URL" ) )
        _msg = static_cast<base_in_message*>( In::load_url( msg ) );
    else {
        _msg = new base_in_message;
        _msg->type = IN_MSG_UNDF;
    }

    return _msg;
}

base_in_message *In::ParseSyncLoad( const std::string &msg )
{
    base_in_message *_msg = static_cast<base_in_message*>( In::load( msg, true ) );
    _msg->raw = msg;
    return _msg;
}

base_in_message *In::ParseSyncGetPid( const std::string &msg )
{
    get_pid_req_in_msg *_msg = new get_pid_req_in_msg;
    _msg->value = std::string(msg);
    _msg->raw = msg;
    return static_cast<base_in_message*>(_msg);
}

base_in_message *In::ParseSyncGetCid( const std::string &msg )
{
    get_cid_req_in_msg *_msg = new get_cid_req_in_msg;
    _msg->value = std::string(msg);
    _msg->raw = msg;
    return static_cast<base_in_message*>(_msg);
}

vector<string> In::split( const string &base, char delim )
{
    vector<string> tokens;
   
    stringstream ss( base );
    string item;
    while( getline(ss, item, delim) ) 
        tokens.push_back( item );

    return tokens;
}

string In::decode_url( string text )
{
    char *decoded = (char*)malloc(text.length()+1);
    strncpy(decoded, text.c_str(), text.length());
    decoded[text.length()] = '\0';
    decode_URI(decoded);
    string decoded_str = string(_(decoded));
    free(decoded);
    return decoded_str;
}

notready_in_msg *In::notready( const std::string &msg )
{
    notready_in_msg *_msg = new notready_in_msg;
    _msg->raw = msg;
    return _msg;
}

hello_in_msg *In::hello( const std::string &msg )
{
    hello_in_msg *_msg = new hello_in_msg;
    _msg->raw = msg;
    _msg->version = "";
    _msg->hello_key = "";
   _msg->major = _msg->minor = _msg->build = _msg->revision = 0;
    
    vector<string> _options = In::split(msg, ' ');
    if( _options.size() > 0 ) {
         for( size_t i = 0; i < _options.size(); ++i ) {
            if( !_options[i].compare(0, 8, "version=") ) {
                _msg->version = _options[i].substr(8);
                
                vector<string> versionv = In::split(_msg->version, '.');
                _msg->major = atoi(versionv[0].c_str());
                _msg->minor = atoi(versionv[1].c_str());
                _msg->build = versionv.size() > 2 ? atoi(versionv[2].c_str()) : 1000;
                _msg->revision = versionv.size() > 3 ? atoi(versionv[3].c_str()) : 1000;
            }
            else if( !_options[i].compare(0, 4, "key=") ) {
                _msg->hello_key.assign(_options[i].substr(4));
            }
        }
    }

    return _msg;
}

auth_in_msg *In::auth( const string &msg )
{
    auth_in_msg *_msg = new auth_in_msg;
    _msg->raw = msg;
    _msg->value = atoi( msg.substr(5).c_str() );
    return _msg;
}

status_in_msg *In::status( const string &msg )
{
    status_in_msg *_msg = new status_in_msg;
    _msg->raw = msg;
    _msg->value = "";
    _msg->is_error = false;
    
    string _base = msg.substr(7);
    size_t _pos = _base.find( "|" );    // main|ad - parse only main now
    if( _pos != string::npos ) {
        if( !_base.compare(0, 9, "ad:prebuf") ) {
            _msg->value.assign( _("Starting...") );
            return _msg;
        }
        _base.erase( _pos );
    }

    vector<string> _options = In::split( _base, ';');
    if( _options.size() > 0 ) {
        if( !_options[0].compare("main:dl") || !_options[0].compare("main:idle") )
            ;
        else if( !_options[0].compare("main:buf") ) {
            _msg->value.assign( _("Buffering ") );
            _msg->value.append( _options[1] ).append("%");
        }
        else if( !_options[0].compare("main:prebuf") ) {
            _msg->value.assign( _("Prebuffering ") );
            _msg->value.append( _options[1] ).append("% (").append( _("connected to ") ).append( _options[8] );
            _msg->value.append( _options[8].compare("1") ? _(" streams") : _(" stream") );
			_msg->value.append(")");
        }
        else if( !_options[0].compare("main:check") ) {
            _msg->value.assign( _("Checking ") );
			_msg->value.append( _options[1] ).append( "%" );
        }
        else if( !_options[0].compare("main:wait") )
            _msg->value.assign( _("Insufficient download speed to play without interruption") );
		else if( !_options[0].compare("main:err") ) {
            _msg->is_error = true;
			_msg->value.assign( _options[2] );
        }
        else if( !_options[0].compare("main:starting") )
            _msg->value.assign( _("Starting...") );
        else if( !_options[0].compare("main:loading") )
            _msg->value.assign( _("Loading...") );
    }
    return _msg;
}

state_in_msg *In::state( const string &msg )
{
    state_in_msg *_msg = new state_in_msg;
    _msg->raw = msg;
    _msg->value = (p2p_state_t)atoi( msg.substr(6).c_str() );
    return _msg;
}

info_in_msg *In::info( const string &msg )
{
    info_in_msg *_msg = new info_in_msg;
    _msg->raw = msg;
    _msg->value = "";
    
    string _base = msg.substr(5);
    vector<string> _options = In::split( _base, ';' );
    if( _options.size() > 0 ) {
        int _info_id = atoi( _options[0].c_str() );
        
        if( _info_id == 0 ) {
            if( _options.size() == 2 ) 
                _msg->value = _options[1];
        }
        else if( _info_id == 1) 
            _msg->value.assign( _("There are no active peers and streams at this moment") );
        else if( _info_id == 2 )
            _msg->value.assign( _("Advertising video") );
        else if( _info_id == 3 )
            _msg->value.assign( _("Main content") );
    }
    
    return _msg;
}

play_in_msg *In::start( const string &msg )
{
    play_in_msg*_msg = new play_in_msg;
    _msg->raw = msg;
    _msg->url = "";
    _msg->start_position = 0;
    _msg->play_type = P2P_PLAY_UNDF;
    _msg->clickurl = "";
    _msg->clicktext = "";
    _msg->skipoffset = "";
    _msg->noadsurl = "";
    _msg->volume = 15;
    _msg->noadstext = "";
    _msg->adinfotext = "";
    
    string _base = msg.substr(6);
    vector<string> _options = In::split( _base, ' ');
    if( _options.size() > 0 ) {
        bool _ad = false;
        bool _int_ad = false;
        bool _stream = false;
        
        for( size_t i = 0; i < _options.size(); ++i ) {
            if( !_options[i].compare(0, 3, "ad=") )
                _ad = (bool)atoi( _options[i].substr(3, 1).c_str() );
            else if( !_options[i].compare(0, 14, "interruptable=") )
                _int_ad = (bool)atoi( _options[i].substr(14, 1).c_str() );
            else if( !_options[i].compare(0, 4, "pos=") )
                _msg->start_position = atoi( _options[i].substr(4, _options[i].length() - 4).c_str() );
            else if( !_options[i].compare(0, 7, "stream=") )
                _stream = (bool)atoi( _options[i].substr(7, 1).c_str() );
            else if( !_options[i].compare(0, 9, "clickurl=") )
                _msg->clickurl.assign(decode_url(_options[i].substr(9)));
            else if( !_options[i].compare(0, 10, "clicktext=") )
                _msg->clicktext.assign(decode_url(_options[i].substr(10)));
            else if( !_options[i].compare(0, 11, "skipoffset=") ) {
                _msg->skipoffset.assign(decode_url(_options[i].substr(11)));
                size_t per_pos = _msg->skipoffset.find("%");
                if(per_pos == string::npos && _msg->skipoffset.size() == 5) {
                    string new_skipoff = "00:" + _msg->skipoffset;
                    _msg->skipoffset.assign(new_skipoff);
                }
            }
            else if( !_options[i].compare(0, 11, "noads_link=") )
                _msg->noadsurl.assign(decode_url(_options[i].substr(11)));
            else if( !_options[i].compare(0, 4, "url=") )
                _msg->url.assign(decode_url(_options[i].substr(4)));
            else if( !_options[i].compare(0, 7, "volume=") )
                _msg->volume = atoi( _options[i].substr(7, _options[i].length() - 7).c_str() );
            else if( !_options[i].compare(0, 11, "noads_text=") )
                _msg->noadstext.assign(decode_url(_options[i].substr(11)));
            else if( !_options[i].compare(0, 13, "ad_info_text=") )
                _msg->adinfotext.assign(decode_url(_options[i].substr(13)));
            else if( _options[i].length() && !_msg->url.compare("") )
                _msg->url.assign(decode_url(_options[i]));
        }
        
        if( _int_ad )
            _msg->play_type = P2P_PLAY_INTERRUPTABLE_AD;
        else if( _ad )
            _msg->play_type = P2P_PLAY_AD;
        else if( _stream )
            _msg->play_type = P2P_PLAY_STREAM;
        else
            _msg->play_type = P2P_PLAY_MAIN;
    }
    
    return _msg;
}

play_in_msg *In::play( const string &msg )
{
    play_in_msg *_msg = new play_in_msg;
    _msg->raw = msg;
    _msg->url = msg.substr(5);
    _msg->play_type = P2P_PLAY_MAIN;
    return _msg;
}

play_in_msg *In::play_ad( const string &msg )
{
    play_in_msg *_msg = new play_in_msg;
    _msg->raw = msg;
    _msg->url = msg.substr(7);
    _msg->play_type = P2P_PLAY_AD;
    return _msg;
}

play_in_msg *In::play_interruptable_ad( const string &msg )
{
    play_in_msg *_msg = new play_in_msg;
    _msg->raw = msg;
    _msg->url = msg.substr(8);
    _msg->play_type = P2P_PLAY_INTERRUPTABLE_AD;
    return _msg;
}

pause_in_msg *In::pause( const string &msg )
{
    pause_in_msg *_msg = new pause_in_msg;
    _msg->raw = msg;
    return _msg;
}

resume_in_msg *In::resume( const string &msg )
{
    resume_in_msg *_msg = new resume_in_msg;
    _msg->raw = msg;
    return _msg;
}

stop_in_msg *In::stop( const string &msg )
{
    stop_in_msg *_msg = new stop_in_msg;
    _msg->raw = msg;
    return _msg;
}

load_in_msg *In::load( const string &msg, bool sync )
{
    load_in_msg *_msg = new load_in_msg;
    _msg->raw = msg;
    _msg->load_id = 0;
    _msg->status = IN_LOAD_UNDF;
    _msg->err_message = "";
    _msg->infohash = "";
    _msg->checksum = "";
    _msg->default_quality = -1;

    string _base = sync ? msg : msg.substr(9);
    size_t _pos;
    if( !sync ) {
        _pos = _base.find("{");
        if( _pos != string::npos )
            _msg->load_id = atoi( _base.substr(0, _pos - 1).c_str() );
        _base = _base.substr( _pos+1, _base.length() - 1 );
    }
    
    _pos = _base.find( "\"status\":" );
    if( _pos != string::npos ) {
        size_t _pos_end = _base.find( ",", _pos );
        if( _pos_end != string::npos )
            _msg->status = (income_load_status_type)atoi(_base.substr( _pos + 10, _pos_end - _pos - 10).c_str());
        
        if( _msg->status == IN_LOAD_SINGLE || _msg->status == IN_LOAD_MULTI ) {
            _pos = _base.find("\"infohash\":");
            if( _pos != string::npos ) {
                 _pos_end = _base.find("\"", _pos + 13);
                 if( _pos_end != string::npos ) {
                    _msg->infohash = _base.substr( _pos + 13, _pos_end - _pos - 13 );
                    
                    _pos = _base.find("\"checksum\":");
                    if( _pos != string::npos ) {
                        _pos_end = _base.find("\"", _pos + 13);
                        if( _pos_end != string::npos )
                            _msg->checksum = _base.substr( _pos + 13, _pos_end - _pos - 13 );
                    }
                    
                    _pos = _base.find("\"default_quality\":");
                    if( _pos != string::npos ) {
                        _pos_end = _base.find( ",", _pos );
                        
                        if( _pos_end != string::npos ) {
                            _msg->default_quality = atoi(_base.substr( _pos + 19, _pos_end - _pos - 19).c_str());
                        }
                        else {
                            _pos_end = _base.find( "}", _pos );
                            if( _pos_end != string::npos )
                                _msg->default_quality = atoi(_base.substr( _pos + 19, _pos_end - _pos - 19).c_str());
                        }
                    }
                    
                    _pos = _base.find("\"qualities\":");
                    if( _pos != string::npos ) {
                        _pos_end = _base.find("}]", _pos + 15);
                        if( _pos_end != string::npos ) {
                            string q_str = _base.substr( _pos + 15, _pos_end - _pos - 15 );
                            do {
                                _pos = q_str.find("}, {");
                                string _item = ( _pos == string::npos ) ? q_str : q_str.substr( 0, _pos );
                                q_str.erase( 0, _pos+4 );
                                
                                size_t _pos_delim = _item.find(", ");
                                string _name = "";
                                string _bitrate = "";
                                if( _pos_delim != string::npos) {
                                    string _first = _item.substr( 0, _pos_delim );
                                    string _second = _item.substr( _pos_delim + 2, _item.length() - 1 );
                                    if( _first.find("\"bitrate\":") != string::npos )
                                        _bitrate = _first.substr( 11, _first.length() - 11 );
                                    else
                                        _name = _first.substr( 9, _first.length() - 10 );
                                        
                                    if( _second.find("\"name\": ") != string::npos )
                                        _name = _second.substr( 9, _second.length() - 10 );
                                    else
                                        _bitrate = _second.substr( 11, _second.length() - 11 );
                                }
                                
                                _msg->qualities.push_back( _name + " " + _bitrate + " Kbit/s" );
                            }
                            while( _pos != string::npos );
                        }
                    }
                    
                    _pos =  _base.find( "\"files\":" );
                    if( _pos != string::npos ) {
                        _pos_end = _base.find("]]");
                        if( _pos_end != string::npos )
                            _base = _base.substr( _pos + 11, _pos_end - _pos - 11 );

                        do {
                            _pos = _base.find("], [");
                            string _item = ( _pos == string::npos ) ? _base : _base.substr( 0, _pos );
                            _base.erase( 0, _pos + 4 );
                                                  
                            size_t _pos_delim = _item.find(", ");
                            if( _pos_delim != string::npos ) {
                                string _title, _index;

                                _title = _item.substr( 1, _pos_delim - 2 );
                                _title = In::decode_url(_title);
                                _index = _item.erase( 0, _pos_delim + 2 );
                                _msg->files.insert( pair<string, int>( _title, atoi(_index.c_str()) ) );
                            }
                        }
                        while( _pos != string::npos );
                    }
                    else {
                        _msg->status = IN_LOAD_ERROR;
                        _msg->err_message = "File without content.";
                    }
                 }
                 else {
                    _msg->status = IN_LOAD_ERROR;
                    _msg->err_message = "No infohash specified.";
                 }
            }
            else {
                _msg->status = IN_LOAD_ERROR;
                _msg->err_message = "No infohash specified.";
            }
        }
        else {
            if( _msg->status == IN_LOAD_ERROR ) {
                _pos = _base.find("\"message\":");
                if( _pos != string::npos ) {
                    _pos_end = _base.find("\"", _pos + 12);
                    if( _pos_end != string::npos )
                        _msg->err_message = _base.substr( _pos + 12, _pos_end - _pos - 12 );
                }
            }
            else {
                _msg->status = IN_LOAD_ERROR;
                _msg->err_message = "Cannot load transport file";
            }
        }
    }
    else {
        _msg->status = IN_LOAD_ERROR;
        _msg->err_message = "Cannot load transport file";
    }

    return _msg;
}

shutdown_in_msg *In::shutdown( const string &msg )
{
    shutdown_in_msg *_msg = new shutdown_in_msg;
    _msg->raw = msg;
    return _msg;
}

event_in_msg *In::event( const string &msg )
{
    event_in_msg *_msg = new event_in_msg;
    _msg->raw = msg;
    _msg->event_type = IN_EVENT_MSG_UNDF;
    _msg->event.cansave_event = NULL;
    _msg->event.dialog_event = NULL;
    _msg->event.live_pos_event = NULL;
    _msg->event.user_data_event = NULL;
    
    string _base = msg.substr(6);
    vector<string> _options = In::split( _base, ' ' );
    
    if( !_options[0].compare( "cansave" ) ) {
        _msg->event_type = IN_EVENT_MSG_CANSAVE;
        _msg->event.cansave_event = new cansave_in_event_msg;
        _msg->event.cansave_event->infohash = "";
        _msg->event.cansave_event->index = -1;
        _msg->event.cansave_event->format = P2P_SAVE_UNSAVEABLE;
        
        for( size_t _i = 1; _i < _options.size(); ++_i ) {
            if( !_options[_i].compare(0, 8, "infohash") )
                _msg->event.cansave_event->infohash = _options[_i].substr(9);
            else if( !_options[_i].compare(0, 5, "index") )
                _msg->event.cansave_event->index = atoi(_options[_i].substr(6).c_str());
            else if( !_options[_i].compare(0, 6, "format") ) {
                string _format = _options[_i].substr(7);
                if( !_format.compare("plain") )
                    _msg->event.cansave_event->format = P2P_SAVE_PLAIN;
                else if( !_format.compare("encrypted") )
                    _msg->event.cansave_event->format = P2P_SAVE_ENCRYPTED;
            }
        }
    }
    else if( !_options[0].compare( "showdialog" ) ) {
        _msg->event_type = IN_EVENT_MSG_SHOW_DIALOG;
        _msg->event.dialog_event = new show_dialog_in_event_msg;
        _msg->event.dialog_event->text = "";
        _msg->event.dialog_event->title = "";
        _msg->event.dialog_event->provider_name = "";
        _msg->event.dialog_event->provider_url = "";
        _msg->event.dialog_event->premium_activate_url = "";
        
        for( size_t _i = 1; _i < _options.size(); ++_i ) {
            if( !_options[_i].compare(0, 4, "text") )
                _msg->event.dialog_event->text = decode_url(_options[_i].substr(5));
            else if( !_options[_i].compare(0, 5, "title") )
                _msg->event.dialog_event->title = decode_url(_options[_i].substr(6));
            else if( !_options[_i].compare(0, 13, "provider_name") )
                _msg->event.dialog_event->provider_name = decode_url(_options[_i].substr(14));
            else if( !_options[_i].compare(0, 12, "provider_url") )
                _msg->event.dialog_event->provider_url = decode_url(_options[_i].substr(13));
            else if( !_options[_i].compare(0, 20, "premium_activate_url") )
                _msg->event.dialog_event->premium_activate_url = decode_url(_options[_i].substr(21));
        }
    }
    else if( !_options[0].compare( "livepos" ) ) {
        _msg->event_type = IN_EVENT_MSG_LIVE_POS;
        _msg->event.live_pos_event = new live_pos_in_event_msg;
        _msg->event.live_pos_event->live_first = 0;
        _msg->event.live_pos_event->live_last = 0;
        _msg->event.live_pos_event->pos = 0;
        _msg->event.live_pos_event->first_ts = 0;
        _msg->event.live_pos_event->last_ts = 0;
        _msg->event.live_pos_event->last = 0;
        _msg->event.live_pos_event->buffer_pieces = 0;

        for( size_t _i = 1; _i < _options.size(); ++_i ) {
            if( !_options[_i].compare(0, 10, "live_first") )
                _msg->event.live_pos_event->live_first = atoi(_options[_i].substr(11).c_str());
            else if( !_options[_i].compare(0, 9, "live_last") )
                _msg->event.live_pos_event->live_last = atoi(_options[_i].substr(10).c_str());
            else if( !_options[_i].compare(0, 3, "pos") )
                _msg->event.live_pos_event->pos = atoi(_options[_i].substr(4).c_str());
            else if( !_options[_i].compare(0, 8, "first_ts") )
                _msg->event.live_pos_event->first_ts = atoi(_options[_i].substr(9).c_str());
            else if( !_options[_i].compare(0, 7, "last_ts") )
                _msg->event.live_pos_event->last_ts = atoi(_options[_i].substr(8).c_str());
            else if( !_options[_i].compare(0, 4, "last") )
                _msg->event.live_pos_event->last = atoi(_options[_i].substr(5).c_str());
            else if( !_options[_i].compare(0, 13, "buffer_pieces") )
                _msg->event.live_pos_event->buffer_pieces = atoi(_options[_i].substr(14).c_str());
            else if( !_options[_i].compare(0, 7, "is_live") )
                _msg->event.live_pos_event->is_live = (bool)atoi(_options[_i].substr(8).c_str());
        }
    }
    else if( !_options[0].compare( "getuserdata" ) ) {
        _msg->event_type = IN_EVENT_MSG_GET_USER_DATA;
        _msg->event.user_data_event = new user_data_in_event_msg;
    }
    
    return _msg;
}

load_url_msg *In::load_url( const string &msg )
{
    load_url_msg *_msg = new load_url_msg;
    _msg->raw = msg;
    
    string _base = decode_url(msg);
    size_t _pos;
    _pos = _base.find("items=[{");
    
    if( _pos != string::npos ) {
        _base = _base.substr(_pos + 8, _base.rfind("}]") - _pos - 8);
        do {
            _pos = _base.find("}, {");
            string _item_str = ( _pos == string::npos ) ? _base : _base.substr( 0, _pos );
            _base.erase( 0, _pos + 4 );
            
            size_t _delim_pos;
            load_url_item item;
            item.type = P2P_LOAD_URL_UNDF;
            item.id = "";
            item.url = "";
            item.require_flash = false;
            item.width = 0;
            item.height = 0;
            item.left = -1;
            item.top = -1;
            item.right = -1;
            item.bottom = -1;
            item.min_width = 0;
            item.min_height = 0;
            item.allow_dialogs = false;
            item.enable_flash = true;
            item.cookies = 1;
            //item.embed_scripts; -> vector
            item.embed_code = "";
            
            item.preload = true;
            item.fullscreen = 0;

            item.max_impressions = 0;
            item.impressions = 0;

            item.content_type = "";
            item.creative_type = "";
            item.click_url = "";
            
            item.user_agent = 1;
            item.close_after_seconds = 0;
            
            do {
                bool _isArray = false;
                if( !_item_str.compare(0, 16, "embedScripts\": [") ) {
                    _delim_pos = _item_str.find("], \"");
                    _isArray = true;
                }
                else
                    _delim_pos = _item_str.find(", \"");
                string _param_str = ( _delim_pos == string::npos ) ? _item_str : _item_str.substr( 0, _delim_pos );
                _item_str.erase( 0, _isArray ? _delim_pos + 3 : _delim_pos + 2 );
                
                if( !_param_str.compare( 0, 8, "\"type\": " ) ) {
                    string val = _param_str.substr(9, _param_str.length() - 10);
                    if( !val.compare("overlay") )
                        item.type = P2P_LOAD_URL_OVERLAY;
                    else if( !val.compare("interactive-pause") )
                        item.type = P2P_LOAD_URL_PAUSE;
                    else if( !val.compare("interactive-stop") )
                        item.type = P2P_LOAD_URL_STOP;
                    else if( !val.compare("interactive-preroll") )
                        item.type = P2P_LOAD_URL_PREROLL;
                    else if( !val.compare("slider") )
                        item.type = P2P_LOAD_URL_SLIDER;
                    else if( !val.compare("interactive-hidden") )
                        item.type = P2P_LOAD_URL_HIDDEN;
                }
                else if( !_param_str.compare( 0, 6, "\"id\": " ) )
                    item.id = _param_str.substr(7, _param_str.length() - 8);
                else if( !_param_str.compare( 0, 7, "\"url\": " ) )
                    item.url = _param_str.substr(8, _param_str.length() - 9);
                else if( !_param_str.compare( 0, 16, "\"requireFlash\": " ) ) {
                    string val = _param_str.substr(16, _param_str.length() - 16);
                    item.require_flash = !val.compare("true");
                }
                else if( !_param_str.compare( 0, 9, "\"width\": " ) )
                    item.width = atoi(_param_str.substr(9, _param_str.length() - 9).c_str());
                else if( !_param_str.compare( 0, 10, "\"height\": " ) )
                    item.height = atoi(_param_str.substr(10, _param_str.length() - 10).c_str());
                else if( !_param_str.compare( 0, 8, "\"left\": " ) )
                    item.left = atoi(_param_str.substr(8, _param_str.length() - 8).c_str());
                else if( !_param_str.compare( 0, 7, "\"top\": " ) )
                    item.top = atoi(_param_str.substr(7, _param_str.length() - 7).c_str());
                else if( !_param_str.compare( 0, 9, "\"right\": " ) )
                    item.right = atoi(_param_str.substr(9, _param_str.length() - 9).c_str());
                else if( !_param_str.compare( 0, 10, "\"bottom\": " ) )
                    item.bottom = atoi(_param_str.substr(10, _param_str.length() - 10).c_str());
                else if( !_param_str.compare( 0, 12, "\"minWidth\": " ) )
                    item.min_width = atoi(_param_str.substr(12, _param_str.length() - 12).c_str());
                else if( !_param_str.compare( 0, 13, "\"minHeight\": " ) )
                    item.min_height = atoi(_param_str.substr(13, _param_str.length() - 13).c_str());
                else if( !_param_str.compare( 0, 16, "a\"llowDialogs\": " ) ) {
                    string val = _param_str.substr(16, _param_str.length() - 16);
                    item.allow_dialogs = !val.compare("true");
                }
                else if( !_param_str.compare( 0, 15, "\"enableFlash\": " ) ) {
                    string val = _param_str.substr(15, _param_str.length() - 15);
                    item.enable_flash = !val.compare("true");
                }
                else if( !_param_str.compare( 0, 11, "\"cookies\": " ) )
                    item.cookies = atoi(_param_str.substr(11, _param_str.length() - 11).c_str());
                else if( !_param_str.compare( 0, 17, "\"embedScripts\": [" ) ) {
                    if( _delim_pos == 17 ) continue;
                    string _arr_str = _param_str.substr(18, _param_str.length() - 19);
                    size_t _arr_delim_pos;
                    do {
                        _arr_delim_pos = _arr_str.find("\", \"");
                        string _arr_item_str = ( _arr_delim_pos == string::npos ) ? _arr_str.substr(0, _arr_str.length()-1) : _arr_str.substr( 0, _arr_delim_pos );
                        _arr_str.erase( 0, _arr_delim_pos + 4 );
                        item.embed_scripts.push_back(_arr_item_str);
                    }
                    while( _arr_delim_pos != string::npos );
                }
                else if( !_param_str.compare( 0, 13, "\"embedCode\": " ) )
                    item.embed_code = _param_str.substr(14, _param_str.length() - 15);
                else if( !_param_str.compare( 0, 11, "\"preload\": " ) ) {
                    string val = _param_str.substr(11, _param_str.length() - 11);
                    item.preload = !val.compare("true");
                }
                else if( !_param_str.compare( 0, 14, "\"fullscreen\": " ) )
                    item.fullscreen = atoi(_param_str.substr(14, _param_str.length() - 14).c_str());
                else if( !_param_str.compare( 0, 18, "\"maxImpressions\": " ) ) {
                    int val = atoi(_param_str.substr(18, _param_str.length() - 18).c_str());
                    item.max_impressions = val == 0 ? 9999 : val;
                }
                else if( !_param_str.compare( 0, 15, "\"contentType\": " ) )
                    item.content_type = _param_str.substr(16, _param_str.length() - 17);
                else if( !_param_str.compare( 0, 16, "\"creativeType\": " ) )
                    item.creative_type = _param_str.substr(17, _param_str.length() - 18);
                else if( !_param_str.compare( 0, 12, "\"clickUrl\": " ) )
                    item.click_url = _param_str.substr(13, _param_str.length() - 14);
                else if( !_param_str.compare( 0, 13, "\"userAgent\": " ) )
                    item.user_agent = atoi(_param_str.substr(13, _param_str.length() - 13).c_str());
                else if( !_param_str.compare( 0, 14, "\"closeAfter\": " ) )
                    item.close_after_seconds = atoi(_param_str.substr(14, _param_str.length() - 14).c_str());
            }
            while( _delim_pos != string::npos );
            
            _msg->items.push_back(item);
        }
        while( _pos != string::npos );
    }
    
    return _msg;
}
