// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _ESPRESSIF_RESAMPLE_H_
#define _ESPRESSIF_RESAMPLE_H_

#define PCM_INOUT_NUM_RESTRICT (2)
#define COMPLEXITY_MAX_NUM  (5)

#if (PCM_INOUT_NUM_RESTRICT < 0) || (PCM_INOUT_NUM_RESTRICT > 64)
#error input/output PCM number restrict setup error
#endif

typedef enum {
    RESAMPLE_SUCCESS                 = 1,    /*!< Any positive value indicates success resampling. Here, the vaule 1 is just an example. The function will return the actual length of the PCM file (in bytes). */
    RESAMPLE_FAIL                    = 0,    /*!< Error occurs during the resampling. */
    RESAMPLE_SRC_RATE_CHANGE         = -1,   /*!< The sampling rate of the source file has changed. */
    RESAMPLE_SRC_CH_CHANGE           = -2,   /*!< The number of channel(s) of the source file has changed. */
    RESAMPLE_DEST_RATE_CHANGE        = -3,   /*!< The sampling rate of the destination file has changed.*/
    RESAMPLE_DEST_CH_CHANGE          = -4,   /*!< The number of channel(s) of the destination file has changed. */
    RESAMPLE_MODE_CHANGE             = -5,   /*!< The resampling mode has changed. */
    RESAMPLE_TYPE_CHANGE             = -6,   /*!< The resampling type has changed. */
    RESAMPLE_COMPLEXITY_CHANGE       = -7,   /*!< The complexity parameter has changed. */
    RESAMPLE_IN_DATA_OVER_LENGTH     = -8,   /*!< The size of the input data exceeds the set buffer size. */
    RESAMPLE_SAMPLE_BITS_NOT_SUPPORT = -9,   /*!< The only supported bit width is 16 bits. The function will return -9 if other bit widths are used. */
    RESAMPLE_IN_DATA_BUF_TOO_SMALL   = -10,  /*!< The size of the input buffer is not enough. This error may occur when the buffer of the output PCM file is set too big. Only valid in encoing mode. */
    RESAMPLE_IN_DATA_NUM_ERROR       = -11,  /*!< The actual size of the input data, which equals the size of the input buffer multiplied by the number of channel(s) of the input data, exceeds the maximum value set in the configuration. */
    RESAMPLE_OUT_DATA_NUM_ERROR      = -12,  /*!< The actual size of the output buffer is not equal to the output buffer size set in the configuration.*/
    RESAMPLE_IN_BUF_ALIGNED_ERROR    = -13,  /*!< The buffer of the input PCM file is not byte aligned. */
    RESAMPLE_DOWN_CH_IDX_CHANGE      = -14,  /*!< The currently selected channel (right or left) is not equal to the channel selected in configuration. This error is only triggered when the complexity parameter is set to 0 and the number of channel(s) of the input file has changed from dual to mono. */
} esp_resp_err_t;

typedef enum {
    ESP_RESAMPLE_TYPE_AUTO     = -1,        /*!<automatically select the following type */
    ESP_RESAMPLE_TYPE_DECIMATE = 0,         /*!<only down-sampling, for integer time down-sampling case, like 48000 to 24000 or 12000 or 8000 */
    ESP_RESAMPLE_TYPE_INTERP   = 1,         /*!<only up-sampling, for interger time up-sampling case, like 8000 to 16000 or 24000 or 48000 */
    ESP_RESAMPLE_TYPE_RESAMPLE = 2,         /*!<first up-sampling, then down-sampling, for non-integer case, like 44100 to 48000 */
    ESP_RESAMPLE_TYPE_BYPASS   = 3,         /*!<source sample rate and target sample rate equal case, just by pass */
}esp_resample_type_t;

/**
*  @brief     Resampling supports two modes
*             - Mode 0: decoding mode
*             - Mode 1: encoding mode
*/
typedef enum {
    RESAMPLE_DECODE_MODE = 0,  /*!<In decoding mode, the buffer size of the input stream data is a constant, while the size of the output stream data changes from the size of the input stream data */
    RESAMPLE_ENCODE_MODE = 1,  /*!<In encoding mode, the buffer size of the output file is a constant, while the size of the input file changes from the output file. Note that, you have to store enough data in the input buffer, the minumum requirement of which is relevant to the parameter RESAMPLING_POINT_NUM. The resampling returns the size of the input file that is actually used. */
} esp_resample_mode_t;

/**
 * @brief      Resampling Filter Configuration
 */
typedef struct {
    int src_rate;            /*!< The sampling rate of the source PCM file */
    int src_ch;              /*!< The number of channel(s) of the source PCM file */
    int dest_rate;           /*!< The sampling rate of the destination PCM file */
    int dest_ch;             /*!< The number of channel(s) of the destination PCM file */
    int sample_bits;         /*!< The bit width of the PCM file. Currently, the only supported bit width is 16 bits. */
    int mode;                /*!< The resampling mode (the encoding mode and the decoding mode) */
    int max_indata_bytes;    /*!< The maximum buffer size of the input PCM (in bytes) */
    int out_len_bytes;       /*!< The buffer length of the output stream data. This parameter must be configured in encoding mode. */
    int type;                /*!< The resampling type (Automatic, Upsampling and Downsampling), which can be selected manually at the integer multiples of the resampling cycle. */
    int complexity;          /*!< Indicates the complexity of the resampling. This parameter is only valid when a FIR filter is used. Range: 0~4; O indicates the lowest complexity, which means the accuracy is the lowest and the speed is the fastest; Meanwhile, 4 indicates the highest complexity, which means the accuracy is the highest and the speed is the slowest.*/
    int down_ch_idx;         /*!< Indicates the channel that is selected (the right channel or the left channel). This parameter is only valid when the complexity parameter is set to 0 and the number of channel(s) of the input file has changed from dual to mono. */
} resample_info_t;

/**
 * @brief      Initializes the resampling parameters
 *
 * @param      info           the information of resampling parameters
 * @param      p_in           the buffer of the input PCM file
 * @param      p_out          the buffer of the output PCM file
 * 
 * @return     the resampling handle
 */
void *esp_resample_create(void *info, unsigned  char **p_in, unsigned  char **p_out);

/**
 * @brief      Changes the sampling rate of the source file to that of the destination file
 *
 * @param      handle           the resampling handle
 * @param      info             the information of resampling parameters
 * @param      p_in             the buffer of the input PCM file
 * @param      p_out            the buffer of the output PCM file
 * @param      in_data_size     the length of the PCM stream data in p_in (in bytes), which equals to in_data_size divided by 2 when the data type of the input data is set to short type; and equals to in_data_size divided by 4 when the data type of the input data is set to float type.
 * @param      out_data_size    the length of the PCM stream data in p_out (in bytes), which equals to out_data_size divided by 2 when the data type of the input data is set to short type; and equals to out_data_size divided by 4 when the data type of the input data is set to float type. 
 *
 * @return     esp_resp_err_t   Positive value: the length of the input PCM stream data; Others: error occurs (Please refer to esp_resp_err_t)
 */
esp_resp_err_t esp_resample_run(void *handle, void *info, unsigned char *p_in, unsigned char *p_out, int in_data_size, int *out_data_size);

/**
 * @brief      Destroys the resampling handle
 *
 * @param      handle         the resampling handle
 */
void esp_resample_destroy(void *handle);

#endif
