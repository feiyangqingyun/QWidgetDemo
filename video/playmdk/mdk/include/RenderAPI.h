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
#include <cstring>

MDK_NS_BEGIN

/*!
  \brief RenderAPI
  use concrete types in user code, for example D3D11RenderAPI
 */
struct RenderAPI {
    enum Type {
        Invalid,
        OpenGL = 1,
        Vulkan = 2,
        Metal = 3,
        D3D11 = 4,
        D3D12 = 5,
    };

    //Type type() const { return Type(type_ & 0xffff);}
protected:
    Type type_ = Type::Invalid; // high 16 bits: major + minor version, to unbreak abi for my flawed design

    Type versioned(Type t) const { return Type(t | (MDK_VERSION >> 8 << 16));}
};


struct GLRenderAPI final: RenderAPI {
    GLRenderAPI() {
        type_ = versioned(RenderAPI::OpenGL);
        memset(reserved, 0, sizeof(reserved));
    }
/*** Render Context Resources. Foreign context (provided by user) only ***/
    int fbo = -1; // if >=0, will draw in given fbo. no need to bind in user code
    int unused = 0;
/*
  \brief getProcAddress
  optional. can be null and then standard gl libraries will be searched.
  if not null, it's used to load gl functions
  \param name gl function name
  \param opaque user data, e.g. gl context handle
*/
    void* (*getProcAddress)(const char* name, void* opaque) = nullptr;
    void* (*getCurrentNativeContext)(void* opaque) = nullptr;
/*!
  \brief opaque
  optional. getProcAddress user data, e.g. a gl context handle.
*/
    void* opaque = nullptr;

/***
  Render Context Creation Options.
  as input, they are desired values to create an internal context(ignored if context is provided by user). as output, they are result values(if context is not provided by user)
***/
    enum class Profile : uint8_t {
        No,
        Core,
        Compatibility,
    };

    bool debug = false; /* default false. NOT IMPLENETED */
    int8_t egl = -1; /* default -1. -1: auto. 0: no, 1: try */
/* if any one of opengl and opengles is 0, then another is treated as 1 */
    int8_t opengl = -1; /* default -1. -1: auto. 0: no, 1: try */
    int8_t opengles = -1; /* default -1. -1: auto. 0: no, 1: try */
    Profile profile = Profile::Core; /* default 3. 0: no profile, 1: core profile, 2: compatibility profile */
    float version = 0; /* default 0, ignored if < 2.0. requested version major.minor. result version may < requested version if not supported */
    int8_t reserved[32];
};

struct MetalRenderAPI final: RenderAPI {
    MetalRenderAPI() {
        type_ = versioned(RenderAPI::Metal);
        memset(reserved, 0, sizeof(reserved));
    }
/*** Render Context Resources. Foreign context (provided by user) only ***/
// id<?> => void*: to be compatible with c++
    const void* device = nullptr; // MUST set if metal is provided by user
    const void* cmdQueue = nullptr; // optional. if not null, device can be null. currentQueue callback to share the same command buffer?
/* one of texture and currentRenderTarget MUST be set if metal is provided by user */
    const void* texture = nullptr; // optional. id<MTLTexture>. if not null, device can be null. usually for offscreen rendering. render target for MTLRenderPassDescriptor if encoder is not provided by user. set once for offscreen rendering
    const void* opaque = nullptr; // optional. callback opaque
    const void* (*currentRenderTarget)(const void* opaque) = nullptr; // optional. usually for on screen rendering. return id<MTLTexture>.
    // no encoder because we need own render pass
    const void* layer = nullptr; // optional. CAMetalLayer only used for appling colorspace parameters for hdr/sdr videos.
    const void* reserved[1];

/***
  Render Context Creation Options.
  as input, they are desired values to create an internal context(ignored if context is provided by user). as output, they are result values(if context is not provided by user)
***/
    // device options: macOS only
    int device_index = -1; // -1 will use system default device. callback with index+name?
};

/*!
  NOTE: include d3d11.h first to use D3D11RenderAPI
 */
#if defined(D3D11_SDK_VERSION)
struct D3D11RenderAPI : RenderAPI {
    D3D11RenderAPI(ID3D11DeviceContext* c = nullptr, ID3D11DeviceChild* r = nullptr) : context(c), rtv(r) {
        type_ = versioned(RenderAPI::D3D11);
        memset(reserved, 0, sizeof(reserved));
    }
/*** Render Context Resources. Foreign context (provided by user) only ***/
/*
  context and rtv can be set by user if user can provide. then rendering becomes foreign context mode.
  if rtv is not null, no need to set context
  \sa Player.setRenderAPI()
 */
    ID3D11DeviceContext* context = nullptr;
    // rtv or texture. usually user can provide a texture from gui easly, no d3d code to create a view
    ID3D11DeviceChild* rtv = nullptr; // optional. the render target(view). ID3D11RenderTargetView or ID3D11Texture2D. can be null if context is not null. if not null, no need to set context
    void* reserved[2];

/***
  Render Context Creation Options.
  as input, they are desired values to create an internal context(ignored if context is provided by user). as output, they are result values(if context is not provided by user)
***/
    bool debug = false;
    int buffers = 2; /* UWP must >= 2. */
    int adapter = 0; /* adapter index */
    float feature_level = 0; /* 0 is the highest */
    const char* vendor = nullptr; /* gpu vendor name */
};
#endif

/*!
  NOTE: include d3d12.h first to use D3D12RenderAPI
 */
#if defined(__d3d12_h__)// D3D12_SDK_VERSION: not defined in 19041
struct D3D12RenderAPI : RenderAPI {
    D3D12RenderAPI(ID3D12CommandQueue* cq = nullptr, ID3D12Resource* r = nullptr) : cmdQueue(cq), rt(r) {
        type_ = versioned(RenderAPI::D3D12);
    }
/*** Render Context Resources. Foreign context (provided by user) only ***/
    ID3D12CommandQueue* cmdQueue = nullptr; // optional. will create an internal queue if null.
    ID3D12Resource* rt = nullptr; // optional. the render target
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {}; // optional
    void* reserved[2] = {};

    const void* opaque = nullptr; // optional. callback opaque
    ID3D12Resource* (*currentRenderTarget)(const void* opaque, UINT* index, UINT* count, D3D12_RESOURCE_STATES* state) = nullptr; // optional. usually for on screen rendering.
    void* reserved2[2] = {};

/***
  Render Context Creation Options.
  as input, they are desired values to create an internal context(ignored if context is provided by user). as output, they are result values(if context is not provided by user)
***/
    bool debug = false;
    int buffers = 2; /* must >= 2. */
    int adapter = 0; /* adapter index */
    float feature_level = 0; /* 0 is the highest */
    const char* vendor = nullptr; /* gpu vendor name */
};
#endif


// always declare
struct VulkanRenderAPI final : RenderAPI {
    VulkanRenderAPI() {
        type_ = versioned(RenderAPI::Vulkan);
        memset(reserved, 0, sizeof(reserved));
        memset(reserved_opt, 0, sizeof(reserved_opt));
    }

#if (VK_VERSION_1_0+0)
    VkInstance instance = VK_NULL_HANDLE; // OPTIONAL. shared instance. for internal created context but not foreign context, to load instance extensions
    VkPhysicalDevice phy_device = VK_NULL_HANDLE; // Optional to create internal context. MUST not null for foreign context. Must set if logical device is provided to create internal context.
    VkDevice device = VK_NULL_HANDLE; // Optional to create internal context as shared device. Required for foreign context.
    VkQueue graphics_queue = VK_NULL_HANDLE; // OPTIONAL. If null, will use gfx_queue_index. NOT required if vk is create internally
/*!
  \brief rt
  Used by offscreen rendering.
 */
    VkImage rt = VK_NULL_HANDLE; // VkImage? so can use qrhitexture.nativeTexture().object
    VkRenderPass render_pass = VK_NULL_HANDLE; // optional. If null(usually for offscreen rendering), final image layout is VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    void* opaque = nullptr;
/*!
  \brief renderTargetInfo
  Get render target image size
  \param format image format. MUST be set if framebuffer from beginFrame() is null
  \param finalLayout image final layout. No transition if undefined. Transition can also be in endFrame() callback if needed, then finalLayout here can be undefined.
  NOTE: assume transition is in the same graphics queue family.
  \return (render target)image count, e.g. swapchain image count.
 */
    int (*renderTargetInfo)(void* opaque, int* w, int* h, VkFormat* format, VkImageLayout* finalLayout); // return count
/*!
  \brief beginFrame
  Optional. Can be null(or not) for offscreen rendering if rt is not null.
  MUST be paired with endFrame()
  \param fb can be null, then will create internally. if not null, MUST set render_pass
  \param imgSem from present queue. can be null if fulfill any of
  // TODO: VkImage?
  1. present queue == gfx queue
  2. getCommandBuffer() is provided and submit in user code
  \return image index.
*/
    int (*beginFrame)(void* opaque, VkImageView* view/* = nullptr*/, VkFramebuffer* fb/*= nullptr*/, VkSemaphore* imgSem/* = nullptr*/) = nullptr;
    // int getNextImageView(); // not fbo, fbo is bound to render pass(can be dummy tmp). image view can also be used by compute pipeline. return index
/*!
  \brief currentCommandBuffer()
  if null, create pool internally(RTT)
 */
    VkCommandBuffer (*currentCommandBuffer)(void* opaque) = nullptr;
/*!
  \brief endFrame
  Optional. If null, frame is guaranteed to be rendered to image before executing the next command buffer in user code.
  If not null, user can wait for drawSem before using the image.
  MUST be paired with beginFrame()
  \param drawSem from gfx queue. can be null if fulfill any of
  1. present queue == gfx queue
  2. getCommandBuffer() is provided and submit in user code
  3. RTT offscreen rendering, i.e. rtv is set and beginFrame is null(user should wait for draw finish too)
 */
    void (*endFrame)(void* opaque, VkSemaphore* drawSem/* = nullptr*/) = nullptr; // can be null if offscreen. wait drawSem before present
#endif // (VK_VERSION_1_0+0)
    void* reserved[2];
/*
  Set by user and used internally even if device is provided by user
 */
    int graphics_family = -1; // MUST if graphics and transfer queue family are different
    int compute_family = -1; // optional. it's graphics_family if not set
    int transfer_family = -1; // optional. it's graphics_family if not set
    int present_family = -1; // optional. Must set if logical device is provided to create internal context
/***
  Render Context Creation Options.
  as input, they are desired values to create an internal context(ignored if context is provided by user). as output, they are result values(if context is not provided by user)
***/
    bool debug = false;
    uint8_t buffers = 2; // 2 for double-buffering
    int device_index = -1;
    uint32_t max_version = 0; // requires vulkan 1.1
    int gfx_queue_index = 0; // OPTIONAL
    int transfer_queue_index = -1; // OPTIONAL. if not set, will use gfx queue
    int compute_queue_index = -1; // OPTIONAL. if not set, will use gfx queue

    int depth = 8;
    //const char*
    uint8_t reserved_opt[32]; // color space etc.
};
MDK_NS_END