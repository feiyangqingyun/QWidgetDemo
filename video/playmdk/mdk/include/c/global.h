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
#include <stdbool.h> /* for swift/dart ffi gen. old swift may report error if -fcxx-module and can be workaround by #import <Metal/Metal.h> ifdef __OBJC__ */
#include <stdint.h>

#define MDK_VERSION_INT(major, minor, patch) \
    (((major&0xff)<<16) | ((minor&0xff)<<8) | (patch&0xff))
#define MDK_MAJOR 0
#define MDK_MINOR 23
#define MDK_MICRO 1
#define MDK_VERSION MDK_VERSION_INT(MDK_MAJOR, MDK_MINOR, MDK_MICRO)
#define MDK_VERSION_CHECK(a, b, c) (MDK_VERSION >= MDK_VERSION_INT(a, b, c))

#if defined(_WIN32)
#define MDK_EXPORT __declspec(dllexport)
#define MDK_IMPORT __declspec(dllimport)
#else
#define MDK_EXPORT __attribute__((visibility("default")))
#define MDK_IMPORT __attribute__((visibility("default")))
#endif

#ifdef BUILD_MDK_STATIC
# define MDK_API
#else
# if defined(BUILD_MDK_LIB)
#  define MDK_API MDK_EXPORT
# else
#  define MDK_API MDK_IMPORT
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: global consts or macros for ffi */

/*!
  \brief CallbackToken
  A callback can be registered by (member)function onXXX(obj, callback, CallbackToken* token = nullptr). With the returned token we can remove the callback by onXXX(nullptr, token).
  Non-null callback(.opaque != null): register a callback and return a token(if not null).
  Null callback(.opaque == null) + non-null token: can remove the callback of given token.
  Null callback(.opaque == null) + null token: clear all callbacks.
 */
typedef uint64_t MDK_CallbackToken;

typedef enum MDK_MediaType {
    MDK_MediaType_Unknown = -1,
    MDK_MediaType_Video = 0,
    MDK_MediaType_Audio = 1,
    MDK_MediaType_Subtitle = 3,
} MDK_MediaType;

/*!
  \brief The MediaStatus enum
  Defines the io status of a media stream,
  Use flags_added/removed() to check the change, for example buffering after seek is Loaded|Prepared|Buffering, and changes to Loaded|Prepared|Buffered when seek completed
 */
typedef enum MDK_MediaStatus
{
    MDK_MediaStatus_NoMedia = 0, /* initial status, not invalid. // what if set an empty url and closed?*/
    MDK_MediaStatus_Unloaded = 1, /* unloaded // (TODO: or when a source(url) is set?)*/
    MDK_MediaStatus_Loading = 1<<1, /* opening and parsing the media */
    MDK_MediaStatus_Loaded = 1<<2, /* media is loaded and parsed. player is stopped state. mediaInfo() is available now */
    MDK_MediaStatus_Prepared = 1<<8, /* all tracks are buffered and ready to decode frames. tracks failed to open decoder are ignored*/
    MDK_MediaStatus_Stalled = 1<<3, /* insufficient buffering or other interruptions (timeout, user interrupt)*/
    MDK_MediaStatus_Buffering = 1<<4, /* when buffering starts */
    MDK_MediaStatus_Buffered = 1<<5, /* when buffering ends */
    MDK_MediaStatus_End = 1<<6, /* reached the end of the current media, no more data to read */
    MDK_MediaStatus_Seeking = 1<<7,
    MDK_MediaStatus_Invalid = 1<<31, /* failed to load media because of unsupport format or invalid media source */
} MDK_MediaStatus;

typedef struct mdkMediaStatusChangedCallback {
    bool (*cb)(MDK_MediaStatus, void* opaque);
    void* opaque;
} mdkMediaStatusChangedCallback;

typedef struct mdkMediaStatusCallback {
    bool (*cb)(MDK_MediaStatus oldValue, MDK_MediaStatus newValue, void* opaque);
    void* opaque;
} mdkMediaStatusCallback;
/*!
  \brief The State enum
  Current playback state. Set/Get by user
 */
typedef enum MDK_State {
    MDK_State_NotRunning,
    MDK_State_Stopped = MDK_State_NotRunning,
    MDK_State_Running,
    MDK_State_Playing = MDK_State_Running, /* start/resume to play*/
    MDK_State_Paused,
} MDK_State;
typedef MDK_State MDK_PlaybackState;

typedef struct mdkStateChangedCallback {
    void (*cb)(MDK_State, void* opaque);
    void* opaque;
} mdkStateChangedCallback;

typedef enum MDKSeekFlag {
    /* choose one of FromX */
    MDK_SeekFlag_From0       = 1,    /* relative to time 0*/
    MDK_SeekFlag_FromStart   = 1<<1, /* relative to media start position*/
    MDK_SeekFlag_FromNow     = 1<<2, /* relative to current position, the seek position can be negative*/
    MDK_SeekFlag_Frame       = 1<<6, /* Seek by frame. Seek target is frame count instead of milliseconds. Currently only FromNow|Frame is supported. BUG: avsync */
    /* combine the above values with one of the following*/
/* KeyFrame forward seek may fail(permission denied) near the end of media if there's no key frame after seek target position*/
    MDK_SeekFlag_KeyFrame    = 1<<8, /* fast key-frame seek, forward if Backward is not set. It's accurate seek without this flag. Accurate seek is slow and implies backward seek internally.*/
    MDK_SeekFlag_Fast        = MDK_SeekFlag_KeyFrame,
    MDK_SeekFlag_InCache     = 1 << 10, // try to seek in memory cache first. useful for speeding up network stream seeking.  Target position must be in range (position(), position() + Player.buffered()]

    MDK_SeekFlag_Backward    = 1 << 16,

    MDK_SeekFlag_Default     = MDK_SeekFlag_KeyFrame|MDK_SeekFlag_FromStart|MDK_SeekFlag_InCache
} MDK_SeekFlag;

/*!
  \brief VideoEffect
  per video renderer effect. set via Player.setVideoEffect(MDK_VideoEffect effect, const float*);
 */
enum MDK_VideoEffect {
    MDK_VideoEffect_Brightness,   /* [-1.0f, 1.0f], default 0 */
    MDK_VideoEffect_Contrast,     /* [-1.0f, 1.0f], default 0 */
    MDK_VideoEffect_Hue,          /* [-1.0f, 1.0f], default 0 */
    MDK_VideoEffect_Saturation,   /* [-1.0f, 1.0f], default 0 */
};

enum MDK_ColorSpace {
    MDK_ColorSpace_Unknown,
    MDK_ColorSpace_BT709,
    MDK_ColorSpace_BT2100_PQ,
    MDK_ColorSpace_scRGB,
    MDK_ColorSpace_ExtendedLinearDisplayP3,
    MDK_ColorSpace_ExtendedSRGB,
    MDK_ColorSpace_ExtendedLinearSRGB,
};

MDK_API int MDK_version();
/*!
  \brief javaVM
  deprecated. use MDK_setGlobalOptionPtr("jvm",..) or MDK_setGlobalOptionPtr("JavaVM",..) instead
  Set/Get current java vm
  \param vm null to get current vm
  \return vm before set
 */
MDK_API void* MDK_javaVM(void* vm);

typedef enum MDK_LogLevel {
    MDK_LogLevel_Off,
    MDK_LogLevel_Error,
    MDK_LogLevel_Warning,
    MDK_LogLevel_Info,
    MDK_LogLevel_Debug,
    MDK_LogLevel_All
} MDK_LogLevel;
MDK_API void MDK_setLogLevel(MDK_LogLevel value);
MDK_API MDK_LogLevel MDK_logLevel();
/* \brief setLogHandler
  If log handler is not set, i.e. setLogHandler() was not called, log is disabled.
  Set environment var `MDK_LOG=1` to enable log to stderr.
  If set to non-null handler, logs that >= logLevel() will be passed to the handler.
  If previous handler is set by user and not null, then call setLogHandler(nullptr) will print to stderr, and call setLogHandler(nullptr) again to silence the log
  To disable log, setLogHandler(nullptr) twice is better than simply setLogLevel(LogLevel::Off)
*/
typedef struct mdkLogHandler {
    void (*cb)(MDK_LogLevel, const char*, void* opaque);
    void* opaque;
} mdkLogHandler;
MDK_API void MDK_setLogHandler(mdkLogHandler);

/*
 keys for string/const char* value:
 - "avutil_lib", "avcodec_lib", "avformat_lib", "swresample_lib", "avfilter_lib": path to ffmpeg runtime libraries
 - "plugins_dir": plugins directory. MUST set before "plugins" if not in default dirs
 - "plugins": plugin filenames or paths in pattern "p1:p2:p3"
 - "MDK_KEY": license key for your product
 - "MDK_KEY_CODE_PAGE": license key code page used internally(windows only)
 - "ffmpeg.loglevel" or "ffmpeg.log": ffmpeg log level names, "trace", "debug", "verbose", "info", "warning", "error", "fatal", "panic", "quiet", or "${level}=${avclass}" to set AVClass log level(can be multiple times), e.g. "debug=http"
 - "ffmpeg.cpuflags": cpuflags for ffmpeg
 - "logLevel" or "log": can be "Off", "Error", "Warning", "Info", "Debug", "All". same as SetGlobalOption("logLevel", int(LogLevel))
 - "profiler.gpu": "0" or "1"
 - "R3DSDK_DIR": R3D dlls dir. default dir is working dir
*/
MDK_API void MDK_setGlobalOptionString(const char* key, const char* value);
/*
  keys:
  - "videoout.clear_on_stop": 0/1. clear renderer using background color if playback stops
  - "videoout.buffer_frames": N. max buffered frames to in the renderer
  - "videoout.hdr": auto send hdr metadata to display. overrides Player.set(ColorSpace)
  - "logLevel" or "log": raw int value of LogLevel
  - "profiler.gpu": true/false, 0/1
  - "demuxer.io": use io module for demuxer
        - 0: use demuxer's internal io
        - 1: default. prefer io module
        - 2: always use io module for all protocols
  - "demuxer.live_eos_timeout": read error if no data for the given milliseconds for a live stream. default is 5000

 */
MDK_API void MDK_setGlobalOptionInt32(const char* key, int value);

/*
  keys:
  - "sdr.white": sdr white level. usually it's 203, but some obs-studio default value is 300, so let user set the value
*/
MDK_API void MDK_setGlobalOptionFloat(const char* key, float value);
/*
  keys:
  - "jvm", "JavaVM": JavaVM*. android only
 */
MDK_API void MDK_setGlobalOptionPtr(const char* key, void* value);

MDK_API bool MDK_getGlobalOptionString(const char* key, const char** value);
MDK_API bool MDK_getGlobalOptionInt32(const char* key, int* value);
MDK_API bool MDK_getGlobalOptionPtr(const char* key, void** value);
/*
  events:
  {timestamp(ms), "render.video", "1st_frame"}: when the first frame is rendererd
  {error, "decoder.audio/video/subtitle", "open", stream}: decoder of a stream is open, or failed to open if error != 0. TODO: do not use "open"?
  {track, "decoder.video", "size", {width, height}}: video decoder output size change
  {0, "decoder.video", decoderName, stream}: video decoder output size change
  {track, "decoder.video", "size", {width, height}}: video decoder output size change. MediaInfo.video[track].codec.width/height also changes.
  {track, "video", "size", {width, height}}: video frame size change before rendering, e.g. change by a filter. MediaInfo.video[track].codec.width/height does not change.
  {progress 0~100, "reader.buffering"}: error is buffering progress
  {0/1, "thread.audio/video/subtitle", stream}: decoder thread is started (error = 1) and about to exit(error = 0)
  {error, "snapshot", saved_file if no error and error string if error < 0}
  {0, "cc"}: the 1st closed caption data is decoded. can be used in ui to show CC button.
*/
typedef struct mdkMediaEvent {
    int64_t error; /* result <0: error code(fourcc?). >=0: special value depending on event*/
    const char* category;
    const char* detail; /* if error, detail can be error string*/

    union {
        struct {
            int stream;
        } decoder;
        struct {
            int width;
            int height;
        } video;
    };
} mdkMediaEvent;

/*
bool MDK_SomeFunc(SomeStruct*, mdkStringMapEntry* entry)
entry: in/out, can not be null.
Input entry->priv is null:
The result entry points to the first entry containing the same key as entry->key, or the first entry if entry->key is null.
The result entry->priv is set to a new value by api.
Input entry->priv is not null(set by the api): the result entry points to the next entry.
return: true if entry is found, false if not.
*/
typedef struct mdkStringMapEntry {
    const char* key;    /* input: set by user to query .value field if priv is null
                           output: set by api if priv is not null (set by api) */
    const char* value;  /* output: set by api, or not touched if no such key */

    void* priv; /* input/output: set by api */
} mdkStringMapEntry;

/*
  \brief MDK_strdup
  Always use this if a duplicated string is needed. DO NOT call strdup() directly because may fail to free() it in mdk, for example
  if user code is built against msvc debug crt but mdk uses release crt, then free() in mdk will crash
 */
MDK_API char* MDK_strdup(const char* strSource);

#ifdef __cplusplus
}
#endif
