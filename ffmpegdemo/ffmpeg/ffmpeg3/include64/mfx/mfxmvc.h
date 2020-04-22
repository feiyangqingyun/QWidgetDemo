/******************************************************************************* *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2010 - 2011 Intel Corporation. All Rights Reserved.

File Name: mfxmvc.h

*******************************************************************************/
#ifndef __MFXMVC_H__
#define __MFXMVC_H__

#include "mfxdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CodecProfile, CodecLevel */
enum {
    /* MVC profiles */
    MFX_PROFILE_AVC_MULTIVIEW_HIGH =118,
    MFX_PROFILE_AVC_STEREO_HIGH    =128
};

/* Extended Buffer Ids */
enum {
    MFX_EXTBUFF_MVC_SEQ_DESC =   MFX_MAKEFOURCC('M','V','C','D'),
    MFX_EXTBUFF_MVC_TARGET_VIEWS    =   MFX_MAKEFOURCC('M','V','C','T')
};

typedef struct  {
    mfxU16 ViewId;
    
    mfxU16 NumAnchorRefsL0;
    mfxU16 NumAnchorRefsL1;
    mfxU16 AnchorRefL0[16];
    mfxU16 AnchorRefL1[16];
    
    mfxU16 NumNonAnchorRefsL0;
    mfxU16 NumNonAnchorRefsL1;
    mfxU16 NonAnchorRefL0[16];
    mfxU16 NonAnchorRefL1[16];
} mfxMVCViewDependency;

typedef struct {
    mfxU16 TemporalId;  /* operation point temporal ID */
    mfxU16 LevelIdc; 

    mfxU16 NumViews; /* total number of views, including "depend on" views */
    mfxU16 NumTargetViews; /* number of the output views for the current operation point */
    mfxU16 *TargetViewId;  /* array of target view ID, it points to mfxExtMVCSeqDesc::ViewId, set by SDK */
} mfxMVCOperationPoint;

typedef struct  {
    mfxExtBuffer Header;

    mfxU32 NumView; /* number of view in the stream, set by SDK */
    mfxU32 NumViewAlloc; /* number of allocated elements, set by application */
    mfxMVCViewDependency *View; /* view ID and list of views used as reference for this view, allocated by application */

    mfxU32 NumViewId; /* num of views IDs, to simplify copying, set by SDK */
    mfxU32 NumViewIdAlloc; /* number of allocated elements, set by application */
    mfxU16 *ViewId; /* allocated by application */

    mfxU32 NumOP; /* number of operation points in OP array, set by SDK */
    mfxU32 NumOPAlloc; /* number of allocated elements in OP array, set by application */
    mfxMVCOperationPoint *OP; /* allocated by application */

    mfxU16 NumRefsTotal; /* total number of reference frames in the sequence */
    mfxU32 Reserved[16];

} mfxExtMVCSeqDesc;

typedef struct {
    mfxExtBuffer    Header;

    mfxU16 TemporalId;
    mfxU32 NumView; /* number of view to decode */
    mfxU16 ViewId[1024];
} mfxExtMVCTargetViews ;


#ifdef __cplusplus
} // extern "C" 
#endif

#endif

