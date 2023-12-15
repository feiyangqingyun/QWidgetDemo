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
#include "c/VideoFrame.h"
#include <algorithm>

MDK_NS_BEGIN

enum class PixelFormat
{
    Unknown = 0,
    YUV420P,
    NV12,
    YUV422P,
    YUV444P,
    P010LE,
    P016LE,
    YUV420P10LE,
    UYVY422,
    RGB24,
    RGBA,
    RGBX,
    BGRA,
    BGRX,
    RGB565LE,
    RGB48LE,
    RGB48 = RGB48LE,
    GBRP,
    GBRP10LE,
    XYZ12LE,
    YUVA420P,
    BC1,
    BC3,
    RGBA64,         // name: "rgba64le"
    BGRA64,         // name: "bgra64le"
    RGBP16,         // name: "rgbp16le"
    RGBPF32,        // name: "rgbpf32le"
    BGRAF32,        // name: "bgraf32le"
};

static inline bool operator!(PixelFormat f) { return f == PixelFormat::Unknown; }

class VideoFrame
{
public:
    VideoFrame(const VideoFrame&) = delete;
    VideoFrame& operator=(const VideoFrame&) = delete;
    VideoFrame(VideoFrame&& that) {
        std::swap(p, that.p);
        std::swap(owner_, that.owner_);
    }
    VideoFrame& operator=(VideoFrame&& that)  {
        std::swap(p, that.p);
        std::swap(owner_, that.owner_);
        return *this;
    }

    VideoFrame() = default;
/*!
  \brief VideoFrame
  Construct a video frame for given format, size. If strides is not null, a single contiguous memory for all planes will be allocated.
  If data is not null, data is copied to allocated memory.
  \param width visual width
  \param height visual height
  \sa setBuffers
   NOTE: Unkine setBuffers(), no memory is allocated for null strides.
 */
    VideoFrame(int width, int height, PixelFormat format, int* strides/*in/out*/ = nullptr, uint8_t const** const data/*in/out*/ = nullptr) {
        p = mdkVideoFrameAPI_new(width, height, MDK_PixelFormat(int(format) - 1));
        if (data)
            MDK_CALL(p, setBuffers, data, strides);
    }

    VideoFrame(mdkVideoFrameAPI* pp) : p(pp) {}

    ~VideoFrame() {
        if (owner_)
            mdkVideoFrameAPI_delete(&p);
    }

// isValid() is true for EOS frame, but no data and timestamp() is TimestampEOS.
    bool isValid() const { return !!p; }
    explicit operator bool() const { return isValid();}

    void attach(mdkVideoFrameAPI* api) {
        if (owner_)
            mdkVideoFrameAPI_delete(&p);
        p = api;
        owner_ = false;
    }

    mdkVideoFrameAPI* detach() {
        auto ptr = p;
        p = nullptr;
        return ptr;
    }

    mdkVideoFrameAPI* toC() const {
        return p;
    }

    int planeCount() const { return MDK_CALL(p, planeCount); }

    int width(int plane = -1) const {
        return MDK_CALL(p, width, plane);
    }

    int height(int plane = -1) const {
        return MDK_CALL(p, height, plane);
    }

    PixelFormat format() const {
        return (PixelFormat)(int(MDK_CALL(p, format)) + 1);
    }
/*!
  \brief addBuffer
  Add an external buffer to nth plane, store external buffer data ptr. The old buffer will be released.
  \param data external buffer data ptr
  \param stride stride of data. if <=0, it's the stride of current format at this plane
  \param buf external buffer ptr. user should ensure the buffer is alive before frame is destroyed.
  \param bufDeleter to delete buf when frame is destroyed
 */
    bool addBuffer(const uint8_t* data, int stride, void* buf, void (*bufDeleter)(void** pBuf), int plane = -1) {
        return MDK_CALL(p, addBuffer, data, stride, buf, bufDeleter, plane);
    }
/*
   \brief setBuffers
   Add buffers with data copied from given source. Unlike constructor, a single contiguous memory for all planes is always allocated.
   If data is not null, data is copied to allocated memory.
   \param data array of source data planes, array size MUST >= plane count of format if not null. Can be null and allocate memory without copying.
   NOTE: data[i] will be filled with allocated plane address if necessary(data != null && strides != null).
   If data[0] != null but data[i] == null, assume copying from contiguous source data.
   \param strides array of plane strides, size MUST >= plane count of format if not null. Can be null and strides[i] can be <=0 indicating no padding bytes (for plane i).
   NOTE: strides[i] will be filled with allocated plane i stride if necessary(strides[i] <= 0)
 */
    void setBuffers(uint8_t const** const data, int* strides/*in/out*/ = nullptr) {
        MDK_CALL(p, setBuffers, data, strides);
    }

    const uint8_t* bufferData(int plane = 0) const {
        return MDK_CALL(p, bufferData, plane);
    }

    int bytesPerLine(int plane = 0) const {
        return MDK_CALL(p, bytesPerLine, plane);
    }

    void setTimestamp(double t) {
        return MDK_CALL(p, setTimestamp, t);
    }

    double timestamp() const {
        if (!p)
            return -1;
        return MDK_CALL(p, timestamp);
    }
/*!
  \brief to
  The result frame data is always on host memory. If it's already on host memory and the same as target format, return the current frame.
  NOTE: compressed input/output formats are not supported
  \param fmt output format. if invalid, same as format()
  \param width output width. if invalid(<=0), same as width()
  \param height output height. if invalid(<=0), same as height()
  if all output parameters(invalid) are the same as input, return self
  \return Invalid frame if failed
 */
    VideoFrame to(PixelFormat format, int width = -1, int height = -1) {
        return VideoFrame(MDK_CALL(p, to, MDK_PixelFormat(int(format)-1), width, height));
    }
/*!
  \brief save
  Saves the frame to the file with the given fileName, using the given image file format and quality factor.
  Save the original frame data if:
  - fileName extension is the same as format().name()
  - fileName has no extension, and format is null
  - format is the same as format().name()
  \param format if null, guess the format by fileName's suffix
  \param quality must be in the range 0.0 to 1.0 or -1. Specify 0 to obtain small compressed files, 100 for large uncompressed files, and -1 (the default) to use the default settings.
  \returns true if the frame was successfully saved; otherwise returns false.
 */
    bool save(const char* fileName, const char* format = nullptr, float quality = -1) const {
        return MDK_CALL(p, save, fileName, format, quality);
    }
private:
    mdkVideoFrameAPI* p = nullptr;
    bool owner_ = true;
};

MDK_NS_END