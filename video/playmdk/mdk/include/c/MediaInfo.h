/*
 * Copyright (c) 2019-2023 WangBin <wbsecg1 at gmail.com>
 * This file is part of MDK
 * MDK SDK: https://github.com/wang-bin/mdk-sdk
 * Free for opensource softwares or non-commercial use.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 */
#pragma once
#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mdkAudioCodecParameters {
    const char* codec;
    uint32_t codec_tag;
    const uint8_t* extra_data; /* without padding data */
    int extra_data_size;
    int64_t bit_rate;
    int profile;
    int level;
    float frame_rate;

    bool is_float;
    bool is_unsigned;
    bool is_planar;
    int raw_sample_size;

    int channels;
    int sample_rate;
    int block_align;
    int frame_size; /* const samples per channel in a frame */

    char reserved[128]; /* color info etc. */
} mdkAudioCodecParameters;

typedef struct mdkAudioStreamInfo {
    int index;
    int64_t start_time; /* ms */
    int64_t duration; /* ms */
    int64_t frames;

    const void* priv;
} mdkAudioStreamInfo;

MDK_API void MDK_AudioStreamCodecParameters(const mdkAudioStreamInfo*, mdkAudioCodecParameters* p);
/* see document of mdkStringMapEntry */
MDK_API bool MDK_AudioStreamMetadata(const mdkAudioStreamInfo*, mdkStringMapEntry* entry);

typedef struct mdkVideoCodecParameters {
    const char* codec;
    uint32_t codec_tag;
    const uint8_t* extra_data; /* without padding data */
    int extra_data_size;
    int64_t bit_rate;
    int profile;
    int level;
    float frame_rate;
    int format;
    const char* format_name;

    int width;
    int height;
    int b_frames;

    float par;
    char reserved[128];
} mdkVideoCodecParameters;

typedef struct mdkVideoStreamInfo {
    int index;
    int64_t start_time;
    int64_t duration;
    int64_t frames;
    int rotation;

    const void* priv;
    // TODO: struct_size for ABI compatibility
} mdkVideoStreamInfo;

MDK_API void MDK_VideoStreamCodecParameters(const mdkVideoStreamInfo*, mdkVideoCodecParameters* p);
/* see document of mdkStringMapEntry */
MDK_API bool MDK_VideoStreamMetadata(const mdkVideoStreamInfo*, mdkStringMapEntry* entry);
MDK_API const uint8_t* MDK_VideoStreamData(const mdkVideoStreamInfo*, int* len, int flags);

typedef struct mdkSubtitleCodecParameters {
    const char* codec ;
    uint32_t codec_tag;
    const uint8_t* extra_data;
    int extra_data_size;

    int width;  /* display width. bitmap subtitles only */
    int height; /* display height. bitmap subtitles only */
} mdkSubtitleCodecParameters;

typedef struct mdkSubtitleStreamInfo {
    int index;
    int64_t start_time;
    int64_t duration;

    const void* priv;
} mdkSubtitleStreamInfo;

MDK_API void MDK_SubtitleStreamCodecParameters(const mdkSubtitleStreamInfo*, mdkSubtitleCodecParameters* p);
MDK_API bool MDK_SubtitleStreamMetadata(const mdkSubtitleStreamInfo*, mdkStringMapEntry* entry);

typedef struct mdkChapterInfo {
    int64_t start_time;
    int64_t end_time;
    const char* title; /* NULL if no title */

    const void* priv;
} mdkChapterInfo;

typedef struct mdkProgramInfo {
    int id;
    const int* stream; /* stream index */
    int nb_stream;

    const void* priv;
} mdkProgramInfo;

MDK_API bool MDK_ProgramMetadata(const mdkProgramInfo*, mdkStringMapEntry* entry);

typedef struct mdkMediaInfo
{
    int64_t start_time; /* ms */
    int64_t duration;
    int64_t bit_rate;
    int64_t size; /* file size. IGNORE THIS */
    const char* format;
    int streams;

    mdkAudioStreamInfo* audio;
    int nb_audio;
    mdkVideoStreamInfo* video;
    int nb_video;
    mdkSubtitleStreamInfo* subtitle;
    int nb_subtitle;

    const void* priv;

    mdkChapterInfo* chapters;
    int nb_chapters;

    mdkProgramInfo* programs;
    int nb_programs;
} mdkMediaInfo;

/* see document of mdkStringMapEntry */
MDK_API bool MDK_MediaMetadata(const mdkMediaInfo*, mdkStringMapEntry* entry);

#ifdef __cplusplus
}
#endif
