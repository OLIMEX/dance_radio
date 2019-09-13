// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "audio_common.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "wav_encoder.h"
#include "wav_head.h"

static const char *TAG = "WAV_ENCODER";


typedef struct wav_encoder {
    bool parsed_header;
} wav_encoder_t;

static esp_err_t _wav_encoder_destroy(audio_element_handle_t self)
{
    wav_encoder_t *wav = (wav_encoder_t *)audio_element_getdata(self);
    audio_free(wav);
    return ESP_OK;
}
static esp_err_t _wav_encoder_open(audio_element_handle_t self)
{
    ESP_LOGD(TAG, "_wav_encoder_open");
    return ESP_OK;
}

static esp_err_t _wav_encoder_close(audio_element_handle_t self)
{
    ESP_LOGD(TAG, "_wav_encoder_close");
    if (AEL_STATE_PAUSED != audio_element_get_state(self)) {
        audio_element_info_t info = {0};
        audio_element_getinfo(self, &info);
        info.byte_pos = 0;
        info.total_bytes = 0;
        audio_element_setinfo(self, &info);
    }
    return ESP_OK;
}

static int _wav_encoder_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    audio_element_info_t audio_info = { 0 };
    int r_size = audio_element_input(self, in_buffer, in_len);
    int out_len = r_size;
    if (r_size > 0) {
        out_len = audio_element_output(self, in_buffer, r_size);
        audio_element_getinfo(self, &audio_info);
        audio_info.byte_pos += out_len;
        audio_element_setinfo(self, &audio_info);
    }

    return out_len;
}

audio_element_handle_t wav_encoder_init(wav_encoder_cfg_t *config)
{
    wav_encoder_t *wav = audio_calloc(1, sizeof(wav_encoder_t));
    mem_assert(wav);
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.destroy = _wav_encoder_destroy;
    cfg.process = _wav_encoder_process;
    cfg.open = _wav_encoder_open;
    cfg.close = _wav_encoder_close;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    if (cfg.task_stack == 0) {
        cfg.task_stack = WAV_ENCODER_TASK_STACK;
    }
    cfg.tag = "wav";

    audio_element_handle_t el = audio_element_init(&cfg);
    mem_assert(el);
    audio_element_setdata(el, wav);
    ESP_LOGD(TAG, "wav_encoder_init");
    return el;
}
