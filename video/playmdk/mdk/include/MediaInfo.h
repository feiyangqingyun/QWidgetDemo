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
#include "c/MediaInfo.h"
#include <cstring>
#include <unordered_map>
#include <vector>

MDK_NS_BEGIN

struct AudioCodecParameters {
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
};

struct AudioStreamInfo {
    int index;
    int64_t start_time; /* ms */
    int64_t duration; /* ms */
    int64_t frames;

// stream language is metadata["language"]
    std::unordered_map<std::string, std::string> metadata;
    AudioCodecParameters codec;
};

struct VideoCodecParameters {
    const char* codec;
    uint32_t codec_tag;
    const uint8_t* extra_data; /* without padding data */
    int extra_data_size;
    int64_t bit_rate;
    int profile;
    int level;
    float frame_rate;
    int format; /* pixel format */
    const char* format_name; /* pixel format name */

    int width;
    int height;
    int b_frames;
    float par;
};

struct VideoStreamInfo {
    int index;
    int64_t start_time;
    int64_t duration;
    int64_t frames;
    int rotation; // degree need to rotate clockwise

// stream language is metadata["language"]
    std::unordered_map<std::string, std::string> metadata;
    VideoCodecParameters codec;

    const uint8_t* image_data = nullptr; // audio cover art image data, can be jpeg, png etc.
    int image_size = 0;
};

struct SubtitleCodecParameters {
    const char* codec;
    uint32_t codec_tag = 0;
    const uint8_t* extra_data; /* without padding data */
    int extra_data_size;

    int width = 0;  /* display width. bitmap subtitles only */
    int height = 0; /* display height. bitmap subtitles only */
};

struct SubtitleStreamInfo {
    int index;
    int64_t start_time;
    int64_t duration;

// stream language is metadata["language"]
    std::unordered_map<std::string, std::string> metadata;
    SubtitleCodecParameters codec;
};

struct ChapterInfo {
    int64_t start_time = 0;
    int64_t end_time = 0;
    std::string title;
};

struct ProgramInfo {
    int id;
    std::vector<int> stream; // stream index
    std::unordered_map<std::string, std::string> metadata; // "service_name", "service_provider" etc.
};

struct MediaInfo
{
    int64_t start_time; // ms
    int64_t duration;
    int64_t bit_rate;
    int64_t size; // file size. IGNORE ME!
    const char* format;
    int streams;

    std::vector<ChapterInfo> chapters;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<AudioStreamInfo> audio;
    std::vector<VideoStreamInfo> video;
    std::vector<SubtitleStreamInfo> subtitle;
    std::vector<ProgramInfo> program;
};

// the following functions MUST be built into user's code because user's c++ stl abi is unknown
// used by Player.mediaInfo()
static void from_c(const mdkMediaInfo* cinfo, MediaInfo* info)
{
    *info = MediaInfo();
    if (!cinfo)
        return;
    info->start_time = cinfo->start_time;
    info->duration = cinfo->duration;
    info->bit_rate = cinfo->bit_rate;
    info->size = cinfo->size;
    info->format = cinfo->format;
    info->streams = cinfo->streams;

    mdkStringMapEntry entry{};
    while (MDK_MediaMetadata(cinfo, &entry))
        info->metadata[entry.key] = entry.value;
    for (int i = 0; i < cinfo->nb_chapters; ++i) {
        const auto& cci = cinfo->chapters[i];
        ChapterInfo ci;
        ci.start_time = cci.start_time;
        ci.end_time = cci.end_time;
        if (cci.title)
            ci.title = cci.title;
        info->chapters.emplace_back(std::move(ci));
    }
    for (int i = 0; i < cinfo->nb_audio; ++i) {
        AudioStreamInfo si{};
        const auto& csi = cinfo->audio[i];
        si.index = csi.index;
        si.start_time = csi.start_time;
        si.duration = csi.duration;
        si.frames = csi.frames;
        MDK_AudioStreamCodecParameters(&csi, (mdkAudioCodecParameters*)&si.codec);
        mdkStringMapEntry e{};
        while (MDK_AudioStreamMetadata(&csi, &e))
            si.metadata[e.key] = e.value;
        info->audio.push_back(std::move(si));
    }
    for (int i = 0; i < cinfo->nb_video; ++i) {
        VideoStreamInfo si{};
        const auto& csi = cinfo->video[i];
        si.index = csi.index;
        si.start_time = csi.start_time;
        si.duration = csi.duration;
        si.frames = csi.frames;
        si.rotation = csi.rotation;
        MDK_VideoStreamCodecParameters(&csi, (mdkVideoCodecParameters*)&si.codec);
        mdkStringMapEntry e{};
        while (MDK_VideoStreamMetadata(&csi, &e))
            si.metadata[e.key] = e.value;
        si.image_data = MDK_VideoStreamData(&csi, &si.image_size, 0);
        info->video.push_back(std::move(si));
    }
    for (int i = 0; i < cinfo->nb_subtitle; ++i) {
        SubtitleStreamInfo si{};
        const auto& csi = cinfo->subtitle[i];
        si.index = csi.index;
        si.start_time = csi.start_time;
        si.duration = csi.duration;
        MDK_SubtitleStreamCodecParameters(&csi, (mdkSubtitleCodecParameters*)&si.codec);
        mdkStringMapEntry e{};
        while (MDK_SubtitleStreamMetadata(&csi, &e))
            si.metadata[e.key] = e.value;
        info->subtitle.push_back(std::move(si));
    }
    for (int i = 0; i < cinfo->nb_programs; ++i) {
        const auto& cpi = cinfo->programs[i];
        ProgramInfo pi;
        pi.id = cpi.id;
        pi.stream.assign(cpi.stream, cpi.stream + cpi.nb_stream);
        mdkStringMapEntry e{};
        while (MDK_ProgramMetadata(&cpi, &e))
            pi.metadata[e.key] = e.value;
        info->program.push_back(std::move(pi));
    }
}

MDK_NS_END
