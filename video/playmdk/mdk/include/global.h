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
#include "c/global.h"
#include <cassert>
#include <cfloat>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

#ifndef MDK_NS
#define MDK_NS mdk
#endif
# define MDK_NS_BEGIN namespace MDK_NS {
# define MDK_NS_END }
# define MDK_NS_PREPEND(X) ::MDK_NS::X

#define MDK_CALL(p, FN, ...) (assert(p->FN && "NOT IMPLEMENTED! Runtime version < build version?"), p->FN(p->object, ##__VA_ARGS__))
#define MDK_CALL2(p, FN, ...) (assert(p->size > 0 && offsetof(std::remove_reference<decltype(*p)>::type, FN) < (size_t)p->size && "NOT IMPLEMENTED! Upgrade your runtime library"), p->FN(p->object, ##__VA_ARGS__))

MDK_NS_BEGIN
// vs2013 no constexpr
static const double TimestampEOS = DBL_MAX;
static const double TimeScaleForInt = 1000.0; // ms
static const float IgnoreAspectRatio = 0; // stretch, ROI etc.
// aspect ratio > 0: keep the given aspect ratio and scale as large as possible inside target rectangle
static const float KeepAspectRatio = FLT_EPSILON; // expand using original aspect ratio
// aspect ratio < 0: keep the given aspect ratio and scale as small as possible outside renderer viewport
static const float KeepAspectRatioCrop = -FLT_EPSILON; // expand and crop using original aspect ratio

/*!
  \brief CallbackToken
  A callback can be registered by (member)function onXXX(callback, CallbackToken* token = nullptr). With the returned token we can remove the callback by onXXX(nullptr, token).
  Non-null callback: register a callback and return a token(if not null).
  Null callback + non-null token: can remove the callback of given token.
  Null callback + null token: clear all callbacks.
 */
using CallbackToken = uint64_t;
/*!
 * \brief is_flag
 * if enum E is of enum type, to enable flag(bit) operators, define
 * \code template<> struct is_flag<E> : std::true_type {}; \endcode
 */
template<typename T> struct is_flag; //
template<typename T>
using if_flag = std::enable_if<std::is_enum<T>::value && is_flag<T>::value>;
template<typename E, typename = if_flag<E>>
E operator~(E e1) { return E(~typename std::underlying_type<E>::type(e1));}
template<typename E, typename = if_flag<E>>
E operator|(E e1, E e2) { return E(typename std::underlying_type<E>::type(e1) | typename std::underlying_type<E>::type(e2));}
template<typename E, typename = if_flag<E>>
E operator^(E e1, E e2) { return E(typename std::underlying_type<E>::type(e1) ^ typename std::underlying_type<E>::type(e2));}
template<typename E, typename = if_flag<E>>
E operator&(E e1, E e2) { return E(typename std::underlying_type<E>::type(e1) & typename std::underlying_type<E>::type(e2));}
template<typename E, typename = if_flag<E>>
E& operator|=(E& e1, E e2) { return e1 = e1 | e2;}
template<typename E, typename = if_flag<E>>
E& operator^=(E& e1, E e2) { return e1 = e1 ^ e2;}
template<typename E, typename = if_flag<E>>
E& operator&=(E& e1, E e2) { return e1 = e1 & e2;}
// convenience functions to test whether a flag exists. REQUIRED by scoped enum
template<typename E>
bool test_flag(E e) { return typename std::underlying_type<E>::type(e);}
template<typename E1, typename E2>
bool test_flag(E1 e1, E2 e2) { return test_flag(e1 & e2);}
template<typename E>
bool flags_added(E oldFlags, E newFlags, E testFlags) { return test_flag(newFlags, testFlags) && !test_flag(oldFlags, testFlags);}
template<typename E>
bool flags_removed(E oldFlags, E newFlags, E testFlags) { return !test_flag(newFlags, testFlags) && test_flag(oldFlags, testFlags);}

enum class MediaType : int8_t {
    Unknown = -1,
    Video = 0,
    Audio = 1,
    Subtitle = 3,
};

/*!
  \brief The MediaStatus enum
  Defines the io status of a media stream,
  Use flags_added/removed() to check the change, for example buffering after seek is Loaded|Prepared|Buffering, and changes to Loaded|Prepared|Buffered when seek completed
 */
enum MediaStatus
{
    NoMedia = 0, // initial status, not invalid. // what if set an empty url and closed?
    Unloaded = 1, // unloaded // (TODO: or when a source(url) is set?)
    Loading = 1<<1, // opening and parsing the media
    Loaded = 1<<2, // media is loaded and parsed. player is stopped state. mediaInfo() is available now
    Prepared = 1<<8, // all tracks are buffered and ready to decode frames. tracks failed to open decoder are ignored
    Stalled = 1<<3, // insufficient buffering or other interruptions (timeout, user interrupt)
    Buffering = 1<<4, // when buffering starts
    Buffered = 1<<5, // when buffering ends
    End = 1<<6, // reached the end of the current media, no more data to read
    Seeking = 1<<7,
    Invalid = 1<<31, // failed to load media because of unsupport format or invalid media source
};
template<> struct is_flag<MediaStatus> : std::true_type {};
// MediaStatusCallback

/*!
  \brief The State enum
  Current playback state. Set/Get by user
 */
enum class State : int8_t {
    NotRunning,
    Stopped = NotRunning,
    Running,
    Playing = Running, /// start/resume to play
    Paused,
};
typedef State PlaybackState;

enum class SeekFlag {
    /// choose one of FromX
    From0       = 1,    /// relative to time 0. TODO: remove from api
    FromStart   = 1<<1, /// relative to media start position
    FromNow     = 1<<2, /// relative to current position, the seek position can be negative
    Frame       = 1<<6, /* Seek by frame. Seek target is frame count instead of milliseconds. Currently only FromNow|Frame is supported. BUG: avsync */
    /// combine the above values with one of the following
/* KeyFrame forward seek may fail(permission denied) near the end of media if there's no key frame after seek target position*/
    KeyFrame    = 1<<8, /* fast key-frame seek, forward if Backward is not set. It's accurate seek without this flag. Accurate seek is slow and implies backward seek internally.*/
    Fast        = KeyFrame,
    InCache     = 1 << 10, // try to seek in memory cache first. useful for speeding up network stream seeking.  Target position must be in range (position(), position() + Player.buffered()]

    Backward    = 1 << 16, // for KeyFrame seek only. Accurate seek implies Backward

    Default     = KeyFrame|FromStart|InCache
};
template<> struct is_flag<SeekFlag> : std::true_type {};

static inline int version() {
    return MDK_version();
}

enum LogLevel {
    Off,
    Error,
    Warning,
    Info,
    Debug,
    All
};
#if !MDK_VERSION_CHECK(1, 0, 0)
#if (__cpp_attributes+0)
[[deprecated("use SetGlobalOption(\"log\", LogLevel/*or name*/) instead")]]
#endif
static inline void setLogLevel(LogLevel value) {
    MDK_setLogLevel(MDK_LogLevel(value));
}
#endif

/* \brief setLogHandler
  If log handler is not set, i.e. setLogHandler() was not called, log is disabled.
  Set environment var `MDK_LOG=1` to enable log to stderr.
  If set to non-null handler, logs that >= logLevel() will be passed to the handler.
  If previous handler is set by user and not null, then call setLogHandler(nullptr) will print to stderr, and call setLogHandler(nullptr) again to silence the log
  To disable log, setLogHandler(nullptr) twice is better than simply setLogLevel(LogLevel::Off)
*/
static inline void setLogHandler(std::function<void(LogLevel, const char*)> cb) {
    static std::function<void(LogLevel, const char*)> scb;
    scb = cb;
    mdkLogHandler h;
    h.cb = [](MDK_LogLevel level, const char* msg, void* opaque){
        auto f = (std::function<void(LogLevel, const char*)>*)opaque;
        (*f)(LogLevel(level), msg);
    };
    h.opaque = scb ? (void*)&scb : nullptr;
    MDK_setLogHandler(h);
    static struct LogReset {
        ~LogReset() {
            mdkLogHandler stdh{};
            MDK_setLogHandler(stdh); // reset log handler std to ensure no log go to scb after scb destroyed
        }
    } reset;
}

/*
 keys:
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
static inline void SetGlobalOption(const char* key, const char* value)
{
    MDK_setGlobalOptionString(key, value);
}

/*!
  \return false if no such key
  keys:
  - "ffmpeg.configuration": ffmpeg major version. return false if no ffmpeg api was invoked internally.
 */
static inline bool GetGlobalOption(const char* key, const char** value)
{
    return MDK_getGlobalOptionString(key, value);
}

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
static inline void SetGlobalOption(const char* key, int value)
{
    MDK_setGlobalOptionInt32(key, value);
}

/*
  keys:
  - "sdr.white": sdr white level. usually it's 203, but some obs-studio default value is 300, so let user set the value
*/
static inline void SetGlobalOption(const char* key, float value)
{
    MDK_setGlobalOptionFloat(key, value);
}

/*!
  \return false if no such key
  keys:
  - "ffmpeg.version": ffmpeg major version. return false if no ffmpeg api was invoked internally.
 */
static inline bool GetGlobalOption(const char* key, int* value)
{
    return MDK_getGlobalOptionInt32(key, value);
}

// key: "logLevel"
static inline void SetGlobalOption(const char* key, LogLevel value)
{
    MDK_setGlobalOptionInt32(key, std::underlying_type<LogLevel>::type(value));
}
/*
  keys:
  - "jvm", "JavaVM": JavaVM*. android only. Required if not loaded by System.loadLibrary()
  - "X11Display": Display*
  - "DRMDevice": drm device path, for vaapi
  - "DRMFd": drm fd, for vaapi
 */
static inline void SetGlobalOption(const char* key, void* value)
{
    MDK_setGlobalOptionPtr(key, value);
}

static inline bool GetGlobalOption(const char* key, void** value)
{
    return MDK_getGlobalOptionPtr(key, value);
}

/*!
  \brief javaVM
  Set/Get current java vm
  \param vm null to get current vm
  \return vm before set
 */
#if !MDK_VERSION_CHECK(1, 0, 0)
#if (__cpp_attributes+0)
[[deprecated("use SetGlobalOption(\"jvm\", ptr) instead")]]
#endif
static inline void javaVM(void* vm) {
    return SetGlobalOption("jvm", vm);
}
#endif
/*
  events:
  {timestamp(ms), "render.video", "1st_frame"}: when the first frame is rendererd
  {error, "decoder.audio/video/subtitle", "open", stream}: decoder of a stream is open, or failed to open if error != 0. TODO: do not use "open"?
  {0, "decoder.video", decoderName, stream}: video decoder output size change
  {track, "decoder.video", "size", {width, height}}: video decoder output size change. MediaInfo.video[track].codec.width/height also changes.
  {track, "video", "size", {width, height}}: video frame size change before rendering, e.g. change by a filter. MediaInfo.video[track].codec.width/height does not change.
  {progress 0~100, "reader.buffering"}: error is buffering progress
  {0/1, "thread.audio/video/subtitle", stream}: decoder thread is started (error = 1) and about to exit(error = 0)
  {error, "snapshot", saved_file if no error and error string if error < 0}
  {0, "cc"}: the 1st closed caption data is decoded. can be used in ui to show CC button.
*/
class MediaEvent {
public:
    int64_t error = 0; // result <0: error code(fourcc?). >=0: special value depending on event
    std::string category;
    std::string detail; // if error, detail can be error string

    union {
        struct {
            int stream;
        } decoder;
        struct {
            int width;
            int height;
        } video;
    } /* TODO: data */;
};

/*!
  \brief VideoEffect
  per video renderer effect. set via Player.set(VideoEffect effect, const float&);
 */
enum class VideoEffect {
    Brightness,   /* [-1.0f, 1.0f], default 0 */
    Contrast,     /* [-1.0f, 1.0f], default 0 */
    Hue,          /* [-1.0f, 1.0f], default 0 */
    Saturation,   /* [-1.0f, 1.0f], default 0 */
};

enum ColorSpace {
    ColorSpaceUnknown,
    ColorSpaceBT709,
    ColorSpaceBT2100_PQ,
// scRGB, linear sRGB in extended component range. Scene-referred white level, D65 is 80nit. Used on windows
    ColorSpaceSCRGB,
    ColorSpaceExtendedLinearDisplayP3,
// sRGB in extended component range, sRGB transfer function. Available for macOS displays
    ColorSpaceExtendedSRGB,
// linear sRGB in extended component range. Display-referred white level
    ColorSpaceExtendedLinearSRGB,
};

MDK_NS_END
