// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DLNA_H
#define _DLNA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "upnp.h"
#include "upnp_service.h"
#include "upnp_notify.h"

/**
 * DLNA Renderer request type
 */
typedef enum {
    RCS_GET_MUTE = 0x01,        /* Request get mute properties, type integer (0-1) */
    RCS_SET_MUTE,               /* Request set mute properties */
    RCS_GET_VOL,                /* Request get volume properties */
    RCS_SET_VOL,                /* Request set volume properties */
    AVT_PLAY,                   /* Request play track */
    AVT_NEXT,                   /* Request next track */
    AVT_PREV,                   /* Request previous track */
    AVT_STOP,                   /* Request stop player */
    AVT_PAUSE,                  /* Request pause player*/
    AVT_SEEK,                   /* Request seek track */
    AVT_SET_TRACK_URI,          /* Request set track uri */
    AVT_GET_TRACK_URI,          /* Request get track uri */
    AVT_GET_PLAY_SPEED,         /* Request get current playing speed */
    AVT_GET_PLAY_MODE,          /* Request get current playing mode */
    AVT_GET_TRANS_STATUS,       /* Request get transport status, "ERROR_OCCURRED" or "OK" */
    AVT_GET_TRANS_STATE,        /* Request get transport state "STOPPED", "PAUSED_PLAYBACK", "TRANSITIONING", "NO_MEDIA_PRESENT" */
    AVT_GET_TRACK_DURATION,     /* reqquest get track duration, format "hh:mm:ss" */
    AVT_GET_MEDIA_DURATION,     /* reqquest get media duration, format "hh:mm:ss" */
    AVT_GET_TRACK_NO,           /* reqquest get track no, format "hh:mm:ss" */
    AVT_GET_TRACK_METADATA,     /* reqquest get track metadata, format "hh:mm:ss" */
    AVT_GET_POS_RELTIME,        /* reqquest get track relative time, format "hh:mm:ss" */
    AVT_GET_POS_ABSTIME,        /* reqquest get track absolute time, format "hh:mm:ss" */
    AVT_GET_POS_RELCOUNT,       /* reqquest get track relative bytes count */
    AVT_GET_POS_ABSCOUNT,       /* reqquest get track absolute bytes count */
} renderer_request_type_t;

typedef struct esp_dlna* esp_dlna_handle_t;

typedef int (*renderer_request_t)(esp_dlna_handle_t dlna, const upnp_attr_t *attr, int attr_num, char *buffer, int max_buffer_len);

/**
 * DLNA configuration
 */
typedef struct {
    const char              *friendly_name; /*<! Short user-friendly title */
    const char              *udn;           /*<! Unique device name - uuid:UUID  */
    httpd_handle_t          httpd;          /*<! The httpd server to use to listen with */
    int                     httpd_port;     /*<! The httpd server port */
    renderer_request_t      renderer_req;   /*<! Renderer request callback */
    const upnp_file_info_t  logo;           /*<! Logo for the device */
    void                    *user_ctx;      /*<! User context, can be get with `esp_dlna_get_user_ctx` */
    const char              *root_path;     /*<! XML rootpath (must be same with SSDP advertisement) */
} dlna_config_t;

/**
 * @brief      Intialize DLNA (with 3 UPnP services) and start to server request
 *
 * @param      config  The DLNA configuration
 *
 * @return     The dlna handle
 */
esp_dlna_handle_t esp_dlna_start(dlna_config_t *config);

/**
 * @brief      Send notification the UPnP service with name
 *
 * @param[in]  dlna           The dlna handle
 * @param[in]  service_name   The service name
 * @param[in]  delay_send_ms  The delay timeout, after a period of milliseconds the message will be sent
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t esp_dlna_notify(esp_dlna_handle_t dlna, const char *service_name, int delay_send_ms);

/**
 * @brief      Send notification the AVTransport UPnP service
 *
 * @param[in]  dlna           The dlna handle
 * @param[in]  delay_send_ms  The delay timeout, after a period of milliseconds the message will be sent
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t esp_dlna_notify_avt(esp_dlna_handle_t dlna, int delay_send_ms);

/**
 * @brief      Send notification the RenderingControl UPnP service
 *
 * @param[in]  dlna           The dlna handle
 * @param[in]  delay_send_ms  The delay timeout, after a period of milliseconds the message will be sent
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t esp_dlna_notify_rcs(esp_dlna_handle_t dlna, int delay_send_ms);

/**
 * @brief      Send notification the ConnectionManager UPnP service
 *
 * @param[in]  dlna           The dlna handle
 * @param[in]  delay_send_ms  The delay timeout, after a period of milliseconds the message will be sent
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t esp_dlna_notify_cmr(esp_dlna_handle_t dlna, int delay_send_ms);

/**
 * @brief      Get the dlna user context (passed from the configuration)
 *
 * @param[in]  dlna  The dlna handle
 *
 * @return     The context
 */
void *esp_dlna_get_user_ctx(esp_dlna_handle_t dlna);

/**
 * @brief      DLNA Attribute callback, to use in UPnP service definitions
 *
 * @param      user_ctx        The user context (dlna handle)
 * @param[in]  attr            The attribute (the firstone attribute)
 * @param[in]  attr_num        The attribute number - total attribute
 * @param      buffer          The buffer in or out, depends on attribute type
 * @param[in]  max_buffer_len  The maximum buffer length
 *
 * @return     Number of bytes return to the buffer
 */
int esp_dlna_upnp_attr_cb(void *user_ctx, const upnp_attr_t *attr, int attr_num, char *buffer, int max_buffer_len);


/**
 * @brief      Cleanup and destroy DLNA service
 *
 * @param[in]  dlna  The dlna handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t esp_dlna_destroy(esp_dlna_handle_t dlna);


#define CONST_STR(x)                (void *)((const char *)x)
#define ATTR_CB(x)                  ((void *)x)
#define ONE_ATTR_CB(_name, cb) .num_attrs = 1,\
                               .attrs = (const upnp_attr_t[]) {{\
                                    .name = _name,\
                                    .val = ATTR_CB(cb),\
                                    .type = ATTR_CALLBACK\
                                }}

#define ONE_ATTR_CB_TYPE(_name, cb, _type) .num_attrs = 1,\
                                           .attrs = (const upnp_attr_t[]) {{\
                                                .name = _name,\
                                                .val = ATTR_CB(cb),\
                                                .type = (ATTR_CALLBACK | _type)\
                                            }}

#define ONE_ATTR_CONST(_name, cst) .num_attrs = 1,\
                                   .attrs = (const upnp_attr_t[]) {{\
                                        .name = _name,\
                                        .val = CONST_STR(cst),\
                                        .type = (ATTR_TYPE_STR | ATTR_CONST)\
                                    }}


#ifdef __cplusplus
}
#endif


#endif
