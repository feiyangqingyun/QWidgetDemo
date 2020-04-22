/******************************************************************************* *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2010 - 2011 Intel Corporation. All Rights Reserved.

File Name: mfxjpeg.h

*******************************************************************************/
#ifndef __MFX_JPEG_H__
#define __MFX_JPEG_H__

#include "mfxdefs.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* CodecId */
enum {
    MFX_CODEC_JPEG    = MFX_MAKEFOURCC('J','P','E','G')
};

/* CodecProfile, CodecLevel */
enum
{
    MFX_PROFILE_JPEG_BASELINE      = 1
};

enum
{
    MFX_ROTATION_0      = 0,
    MFX_ROTATION_90     = 1,
    MFX_ROTATION_180    = 2,
    MFX_ROTATION_270    = 3
};

#ifdef __cplusplus
} // extern "C" 
#endif /* __cplusplus */

#endif // __MFX_JPEG_H__
