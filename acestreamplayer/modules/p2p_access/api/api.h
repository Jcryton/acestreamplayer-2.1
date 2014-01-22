#ifndef __P2P_PLUGIN_API__HEADER__
#define __P2P_PLUGIN_API__HEADER__

#include <p2p_object.h>
#include <string>
#include <vector>
#include <map>

#define API_VERSION 4

enum outcome_message_type {
    OUT_MSG_UNDF = -1,
    OUT_MSG_HELLO,
    OUT_MSG_READY,
    OUT_MSG_LOAD,
    OUT_MSG_LOAD_ASYNC,
    OUT_MSG_START,
    OUT_MSG_STOP,
    OUT_MSG_DURATION,
    OUT_MSG_PLAYBACK,
    OUT_MSG_SAVE,
    OUT_MSG_GET_PID,
    OUT_MSG_GET_CID,
    OUT_MSG_GET_AD_URL,
    OUT_MSG_LIVE_SEEK,
    OUT_MSG_USER_DATA,
    OUT_MSG_SHUTDOWN,
    OUT_MSG_STAT_EVENT,
    OUT_MSG_NONLINEAR_AD_EVENT,
    OUT_MSG_MAX = 17
};

struct base_out_message {
    outcome_message_type type;

    virtual ~base_out_message() {}
};

struct hello_out_msg : base_out_message {
    int version;
    
    hello_out_msg() : base_out_message() {
        type = OUT_MSG_HELLO;
    }
};

struct ready_out_msg : base_out_message {
    std::string key;
    bool support_spaces;
    
    ready_out_msg() : base_out_message() {
        type = OUT_MSG_READY;
        support_spaces = false;
    }
};

struct load_out_msg : base_out_message {
    p2p_uri_id_type_t content_type;
    std::string id;
    int developer_id;
    int affiliate_id;
    int zone_id;
    
    load_out_msg() : base_out_message() {
        type = OUT_MSG_LOAD;
    }
};

struct load_async_out_msg : base_out_message {
    int load_id;
    p2p_uri_id_type_t content_type;
    std::string id;
    int developer_id;
    int affiliate_id;
    int zone_id;
    
    load_async_out_msg() : base_out_message() {
        type = OUT_MSG_LOAD_ASYNC;
    }
};

struct start_out_msg : base_out_message {
    p2p_uri_id_type_t content_type;
    std::string id;
    std::string indexes;
    int developer_id;
    int affiliate_id;
    int zone_id;
    int quality;
    int position;
    bool support_spaces;
    
    start_out_msg() : base_out_message() {
        type = OUT_MSG_START;
        support_spaces = false;
    }
};

struct stop_out_msg : base_out_message {
    stop_out_msg() : base_out_message() {
        type = OUT_MSG_STOP;
    }
};

struct duration_out_msg : base_out_message {
    std::string url;
    int duration;
    
    duration_out_msg() : base_out_message() {
        type = OUT_MSG_DURATION;
    }
};

struct playback_out_msg: base_out_message {
    std::string url;
    p2p_playback_value_t playback;
    
    playback_out_msg() : base_out_message() {
        type = OUT_MSG_PLAYBACK;
    }
};

struct save_out_msg : base_out_message {
    std::string save_to;
    std::string infohash;
    int index;
    
    save_out_msg() : base_out_message() {
        type = OUT_MSG_SAVE;
    }
};

struct get_pid_out_msg : base_out_message {
    std::string infohash;
    int developer_id;
    int affiliate_id;
    int zone_id;
    
    get_pid_out_msg() : base_out_message() {
        type = OUT_MSG_GET_PID;
    }
};

struct get_cid_out_msg : base_out_message {
    std::string infohash;
    std::string checksum;
    int developer_id;
    int affiliate_id;
    int zone_id;
    
    get_cid_out_msg() : base_out_message() {
        type = OUT_MSG_GET_CID;
    }
};

struct get_ad_url_out_msg : base_out_message {
    std::string infohash;
    std::string action;
    int width;
    int height;
    
    get_ad_url_out_msg() : base_out_message() {
        type = OUT_MSG_GET_AD_URL;
    }
};

struct live_seek_out_msg : base_out_message {
    int pos;
    
    live_seek_out_msg() : base_out_message() {
        type = OUT_MSG_LIVE_SEEK;
    }
};

struct user_data_out_msg : base_out_message {
    int gender;
    int age;
    
    user_data_out_msg() : base_out_message() { 
        type = OUT_MSG_USER_DATA;
    }
};

struct shutdown_out_msg : base_out_message {
    shutdown_out_msg() : base_out_message() { 
        type = OUT_MSG_SHUTDOWN; 
    }
};

struct stat_event_out_msg : base_out_message {
    p2p_statistics_event_type_t stat_event_type;
    int position;
    
    stat_event_out_msg() : base_out_message() { 
        type = OUT_MSG_STAT_EVENT; 
    }
};

struct nonlinear_ad_event_out_msg : base_out_message {
    std::string id;
    std::string event_type;
    
    nonlinear_ad_event_out_msg() : base_out_message() { 
        type = OUT_MSG_NONLINEAR_AD_EVENT; 
    }
};

enum income_event_message_type {
    IN_EVENT_MSG_UNDF = -1,
    IN_EVENT_MSG_CANSAVE,
    IN_EVENT_MSG_SHOW_URL,
    IN_EVENT_MSG_LIVE_POS,
    IN_EVENT_MSG_GET_USER_DATA,
    IN_EVENT_MSG_SHOW_DIALOG
};

struct base_in_event_message {
    income_event_message_type event_type;    
    
    virtual ~base_in_event_message() {}
};

struct cansave_in_event_msg : base_in_event_message {
    std::string infohash;
    int index;
    p2p_save_format_t format;

    cansave_in_event_msg() : base_in_event_message() { 
        event_type = IN_EVENT_MSG_CANSAVE; 
    }
};

struct show_url_in_event_msg: base_in_event_message {
    std::string url;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    p2p_show_url_type type;
    std::string text;

    show_url_in_event_msg() : base_in_event_message() { 
        event_type = IN_EVENT_MSG_SHOW_URL; 
    }
};

struct live_pos_in_event_msg: base_in_event_message {
    bool is_live;
    int live_first;
    int live_last;
    int pos;
    int first_ts;
    int last_ts;
    int last;
    int buffer_pieces;
    
    live_pos_in_event_msg() : base_in_event_message() { 
        event_type = IN_EVENT_MSG_LIVE_POS; 
    }
};

struct user_data_in_event_msg : base_in_event_message {
    user_data_in_event_msg() : base_in_event_message() { 
        event_type = IN_EVENT_MSG_GET_USER_DATA; 
    }
};

struct show_dialog_in_event_msg : base_in_event_message {
    std::string text;
    std::string title;
    std::string provider_url;
    std::string provider_name;
    std::string premium_activate_url;
    
    show_dialog_in_event_msg() : base_in_event_message() { 
        event_type = IN_EVENT_MSG_SHOW_DIALOG; 
    }
};

enum income_message_type {
    IN_MSG_UNDF = -1,
    IN_MSG_HELLO,
    IN_MSG_PLAY,
    IN_MSG_PAUSE,
    IN_MSG_RESUME,
    IN_MSG_INFO,
    IN_MSG_SHUTDOWN,
    IN_MSG_AUTH,
    IN_MSG_STATUS,
    IN_MSG_STATE,
    IN_MSG_STOP,
    IN_MSG_LOAD,
    IN_MSG_EVENT,
    IN_MSG_GET_PID_RESP,
    IN_MSG_GET_CID_RESP,
    IN_MSG_PRELOAD_PAUSE_ADS,
    IN_MSG_PRELOAD_NONLINEAR_ADS,
    IN_MSG_PRELOAD_STOP_ADS,
    IN_MSG_NOTREADY
};

struct base_in_message {
    income_message_type type;
    std::string raw;
    
    virtual ~base_in_message() {}
};

struct hello_in_msg : base_in_message {
    int major;
    int minor;
    int build;
    int revision;
    std::string version;
    std::string hello_key;
    
    hello_in_msg() : base_in_message() { 
        type = IN_MSG_HELLO; 
    }
};

struct auth_in_msg : base_in_message {
    int value;
    
    auth_in_msg() : base_in_message() { 
        type = IN_MSG_AUTH; 
    }
};

struct status_in_msg : base_in_message {
    std::string value;
    bool is_error;
    
    status_in_msg() : base_in_message() { 
        type = IN_MSG_STATUS; 
    }
};

struct state_in_msg : base_in_message {
    p2p_state_t value;
    
    state_in_msg() : base_in_message() { 
        type = IN_MSG_STATE; 
    }
};

struct info_in_msg : base_in_message {
    std::string value;
    
    info_in_msg() : base_in_message() { 
        type = IN_MSG_INFO; 
    }
};

struct play_in_msg : base_in_message {
    std::string url;
    int start_position;
    p2p_play_content_type_t play_type;
    std::string clickurl;
    std::string clicktext;
    std::string skipoffset;
    std::string noadsurl;
    int volume;
    std::string noadstext;
    std::string adinfotext;
    
    play_in_msg() : base_in_message() { 
        type = IN_MSG_PLAY; 
        play_type = P2P_PLAY_UNDF; 
    }
};

struct pause_in_msg : base_in_message {
    pause_in_msg() : base_in_message() { 
        type = IN_MSG_PAUSE; 
    }
};

struct resume_in_msg : base_in_message {
    resume_in_msg() : base_in_message() { 
        type = IN_MSG_RESUME; 
    }
};

struct stop_in_msg : base_in_message {
    stop_in_msg() : base_in_message() { 
        type = IN_MSG_STOP; 
    }
};

enum income_load_status_type {
    IN_LOAD_UNDF = -1,
    IN_LOAD_NO_MEDIA = 0,
    IN_LOAD_SINGLE = 1,
    IN_LOAD_MULTI = 2,
    IN_LOAD_ERROR = 100
};

struct load_in_msg : base_in_message {
    int load_id;
    income_load_status_type status;
    std::string err_message;
    std::string infohash;
    std::string checksum;
    std::vector<std::string> qualities;
    std::map<std::string, int> files;
    int default_quality;

    load_in_msg() : base_in_message() { 
        type = IN_MSG_LOAD;
    }
};

struct shutdown_in_msg : base_in_message {
    shutdown_in_msg() : base_in_message() { 
        type = IN_MSG_SHUTDOWN; 
    }
};

struct event_in_msg : base_in_message {
    union {
        cansave_in_event_msg *cansave_event;
        show_url_in_event_msg *url_event;
        live_pos_in_event_msg *live_pos_event;
        user_data_in_event_msg *user_data_event;
        show_dialog_in_event_msg *dialog_event;
    } event;
    income_event_message_type event_type;
    
    event_in_msg() : base_in_message() { 
        type = IN_MSG_EVENT; 
        event_type = IN_EVENT_MSG_UNDF; 
    }
};

struct get_pid_req_in_msg : base_in_message {
    std::string value;
    
    get_pid_req_in_msg() : base_in_message() {
        type = IN_MSG_GET_PID_RESP;
    }
};

struct get_cid_req_in_msg : base_in_message {
    std::string value;
    
    get_cid_req_in_msg() : base_in_message() {
        type = IN_MSG_GET_CID_RESP;
    }
};

struct notready_in_msg : base_in_message {
    notready_in_msg() : base_in_message() {
        type = IN_MSG_NOTREADY;
    }
};

struct preload_pause_ad_item {
    std::string id;
    std::string url;
    bool require_flash;
    bool preload;
    int max_impressions;
    int impressions;
    int fullscreen;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int min_width;
    int min_height;
    bool allow_dialogs;
    bool enable_flash;
    int cookies;
    std::string embed_script;
};

struct preload_pause_ads_in_msg : base_in_message {
    std::vector<preload_pause_ad_item> items;
    
    preload_pause_ads_in_msg() : base_in_message() { 
        type = IN_MSG_PRELOAD_PAUSE_ADS; 
    }
};

struct preload_nonlinear_ad_item {
    std::string id;
    std::string url;
    std::string ad_type;
    std::string creative_type;
    std::string click_url;
    bool require_flash;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int min_width;
    int min_height;
    bool allow_dialogs;
    bool enable_flash;
    int cookies;
    std::string embed_script;
};

struct preload_nonlinear_ads_in_msg : base_in_message {
    std::vector<preload_nonlinear_ad_item> items;
    
    preload_nonlinear_ads_in_msg() : base_in_message() { 
        type = IN_MSG_PRELOAD_NONLINEAR_ADS; 
    }
};

struct preload_stop_ad_item {
    std::string id;
    std::string url;
    bool require_flash;
    bool preload;
    int fullscreen;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int min_width;
    int min_height;
    bool allow_dialogs;
    bool enable_flash;
    int cookies;
    std::string embed_script;
};

struct preload_stop_ads_in_msg : base_in_message {
    std::vector<preload_stop_ad_item> items;
    
    preload_stop_ads_in_msg() : base_in_message() { 
        type = IN_MSG_PRELOAD_STOP_ADS; 
    }
};

#endif