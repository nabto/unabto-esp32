/**
 *  uNabto application logic implementation
 */
#include "demo_application.h"
#include <string.h>
#include "unabto/unabto_app.h"
#include <modules/fingerprint_acl/fp_acl_ae.h>
#include <modules/fingerprint_acl/fp_acl_memory.h>

typedef enum { HPM_COOL = 0,
               HPM_HEAT = 1,
               HPM_CIRCULATE = 2,
               HPM_DEHUMIDIFY = 3} heatpump_mode_t;

static uint8_t heatpump_state_ = 1;
static int32_t heatpump_room_temperature_ = 19;
static int32_t heatpump_target_temperature_ = 23;
static uint32_t heatpump_mode_ = HPM_HEAT;

#define DEVICE_NAME_DEFAULT "AMP stub"
#define MAX_DEVICE_NAME_LENGTH 50
static char device_name_[MAX_DEVICE_NAME_LENGTH];
static const char* device_product_ = "ACME 9002 Heatpump";
static const char* device_icon_ = "img/chip-small.png";

static struct fp_acl_db db_;
struct fp_mem_persistence fp_file_;

#define REQUIRES_GUEST FP_ACL_PERMISSION_NONE
#define REQUIRES_OWNER FP_ACL_PERMISSION_ADMIN

#define LED0_PATH "/sys/class/leds/led0/"

void updateLed() {

#if 0
    // Blink LED0 to reflect target temperature and heat pump state
    // (Primary intended for Raspberry Pi)
    if(heatpump_state_) {
        unsigned int delay_off = 100 + (30 - heatpump_target_temperature_) * 50;
        char delay_off_str[5];
        sprintf(delay_off_str, "%u", delay_off);
        writeFile(LED0_PATH "trigger", "timer");
        writeFile(LED0_PATH "delay_on", "100");
        writeFile(LED0_PATH "delay_off", delay_off_str);
    } else {
        writeFile(LED0_PATH "brightness", "0");
    }
#endif

}

void debug_dump_acl() {
    void* it = db_.first();
    while (it != NULL) {
        struct fp_acl_user user;
        fp_acl_db_status res = db_.load(it, &user);
        if (res != FP_ACL_DB_OK) {
            NABTO_LOG_WARN(("ACL error %d\n", res));
            return;
        }
        NABTO_LOG_INFO(("%s [%02x:%02x:%02x:%02x:...]: %04x",
                        user.name,
                        user.fp[0], user.fp[1], user.fp[2], user.fp[3],
                        user.permissions));
        it = db_.next(it);
    }
}

void demo_init() {
    struct fp_acl_settings default_settings;
    default_settings.systemPermissions =
        FP_ACL_SYSTEM_PERMISSION_PAIRING |
        FP_ACL_SYSTEM_PERMISSION_LOCAL_ACCESS;
    default_settings.defaultUserPermissions =
        FP_ACL_PERMISSION_LOCAL_ACCESS;
    default_settings.firstUserPermissions =
        FP_ACL_PERMISSION_ADMIN |
        FP_ACL_PERMISSION_LOCAL_ACCESS |
        FP_ACL_PERMISSION_REMOTE_ACCESS;


    fp_mem_init(&db_, &default_settings, &fp_file_);
    fp_acl_ae_init(&db_);
    snprintf(device_name_, sizeof(device_name_), DEVICE_NAME_DEFAULT);
    updateLed();
}

void demo_application_set_device_name(const char* name) {
    strncpy(device_name_, name, MAX_DEVICE_NAME_LENGTH);
}

void demo_application_set_device_product(const char* product) {
    device_product_ = product;
}

void demo_application_set_device_icon_(const char* icon) {
    device_icon_ = icon;
}

void demo_application_tick() {
#ifndef WIN32
    static time_t time_last_update_ = 0;
    time_t now = time(0);
    if (now - time_last_update_ > 2) {
        if (heatpump_room_temperature_ < heatpump_target_temperature_) {
            heatpump_room_temperature_++;
        } else if (heatpump_room_temperature_ > heatpump_target_temperature_) {
            heatpump_room_temperature_--;
        }
        time_last_update_ = now;
    }
#else
    static size_t ticks_ = 0;
    if ((ticks_ % 200) == 0) {
        if (heatpump_room_temperature_ < heatpump_target_temperature_) {
	    heatpump_room_temperature_++;
        } else if (heatpump_room_temperature_ > heatpump_target_temperature_) {
	    heatpump_room_temperature_--;
        }
    }
    ticks_++;
#endif
}

int copy_buffer(unabto_query_request* read_buffer, uint8_t* dest, uint16_t bufSize, uint16_t* len) {
    uint8_t* buffer;
    if (!(unabto_query_read_uint8_list(read_buffer, &buffer, len))) {
        return AER_REQ_TOO_SMALL;
    }
    if (*len > bufSize) {
        return AER_REQ_TOO_LARGE;
    }
    memcpy(dest, buffer, *len);
    return AER_REQ_RESPONSE_READY;
}

int copy_string(unabto_query_request* read_buffer, char* dest, uint16_t destSize) {
    uint16_t len;
    int res = copy_buffer(read_buffer, (uint8_t*)dest, destSize-1, &len);
    if (res != AER_REQ_RESPONSE_READY) {
        return res;
    }
    dest[len] = 0;
    return AER_REQ_RESPONSE_READY;
}

int write_string(unabto_query_response* write_buffer, const char* string) {
    return unabto_query_write_uint8_list(write_buffer, (uint8_t *)string, strlen(string));
}

bool allow_client_access(nabto_connect* connection) {
    bool local = connection->isLocal;
    bool allow = fp_acl_is_connection_allowed(connection) || local;
    NABTO_LOG_INFO(("Allowing %s connect request: %s", (local ? "local" : "remote"), (allow ? "yes" : "no")));
    debug_dump_acl();
    return allow;    
}

application_event_result application_event(application_request* request,
                                           unabto_query_request* query_request,
                                           unabto_query_response* query_response) {

    NABTO_LOG_INFO(("Nabto application_event: %u", request->queryId));
    debug_dump_acl();

    // handle requests as defined in interface definition shared with
    // client - for the default demo, see
    // https://github.com/nabto/ionic-starter-nabto/blob/master/www/nabto/unabto_queries.xml

    application_event_result res;
    res = AER_REQ_INV_QUERY_ID;


    switch (request->queryId) {
    case 10000:
        // get_public_device_info.json
        if (!write_string(query_response, device_name_)) return AER_REQ_RSP_TOO_LARGE;
        if (!write_string(query_response, device_product_)) return AER_REQ_RSP_TOO_LARGE;
        if (!write_string(query_response, device_icon_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint8(query_response, fp_acl_is_pair_allowed(request))) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint8(query_response, fp_acl_is_user_paired(request))) return AER_REQ_RSP_TOO_LARGE; 
        if (!unabto_query_write_uint8(query_response, fp_acl_is_user_owner(request))) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    case 10010:
        // set_device_info.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_OWNER)) return AER_REQ_NO_ACCESS;
        int res = copy_string(query_request, device_name_, sizeof(device_name_));
        if (res != AER_REQ_RESPONSE_READY) return res;
        if (!write_string(query_response, device_name_)) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    case 11000:
        // get_users.json
        return fp_acl_ae_users_get(request, query_request, query_response); // implied admin priv check
        
    case 11010: 
        // pair_with_device.json
        if (!fp_acl_is_pair_allowed(request)) return AER_REQ_NO_ACCESS;
        res = fp_acl_ae_pair_with_device(request, query_request, query_response); 
        debug_dump_acl();
        return res;

    case 11020:
        // get_current_user.json
        return fp_acl_ae_user_me(request, query_request, query_response); 

    case 11030:
        // get_system_security_settings.json
        return fp_acl_ae_system_get_acl_settings(request, query_request, query_response); // implied admin priv check

    case 11040:
        // set_system_security_settings.json
        return fp_acl_ae_system_set_acl_settings(request, query_request, query_response); // implied admin priv check

    case 11050:
        // set_user_permissions.json
        return fp_acl_ae_user_set_permissions(request, query_request, query_response); // implied admin priv check

    case 11060:
        // set_user_name.json
        return fp_acl_ae_user_set_name(request, query_request, query_response); // implied admin priv check

    case 11070:
        // remove_user.json
        return fp_acl_ae_user_remove(request, query_request, query_response); // implied admin priv check

    case 20000: 
        // heatpump_get_full_state.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_write_uint8(query_response, heatpump_state_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint32(query_response, heatpump_mode_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint32(query_response, (uint32_t)heatpump_target_temperature_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint32(query_response, (uint32_t)heatpump_room_temperature_)) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    case 20010:
        // heatpump_set_activation_state.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_read_uint8(query_request, &heatpump_state_)) return AER_REQ_TOO_SMALL;
        if (!unabto_query_write_uint8(query_response, heatpump_state_)) return AER_REQ_RSP_TOO_LARGE;
        NABTO_LOG_INFO(("Got (and returned) state %d", heatpump_state_));
        updateLed();
        return AER_REQ_RESPONSE_READY;

    case 20020:
        // heatpump_set_target_temperature.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_read_uint32(query_request, (uint32_t*)(&heatpump_target_temperature_))) return AER_REQ_TOO_SMALL;
        if (!unabto_query_write_uint32(query_response, (uint32_t)heatpump_target_temperature_)) return AER_REQ_RSP_TOO_LARGE;
        updateLed();
        return AER_REQ_RESPONSE_READY;

    case 20030:
        // heatpump_set_mode.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_read_uint32(query_request, &heatpump_mode_)) return AER_REQ_TOO_SMALL;
        if (!unabto_query_write_uint32(query_response, heatpump_mode_)) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    default:
        NABTO_LOG_WARN(("Unhandled query id: %u", request->queryId));
        return AER_REQ_INV_QUERY_ID;
    }
}
