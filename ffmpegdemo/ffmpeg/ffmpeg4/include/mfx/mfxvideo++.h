/* ****************************************************************************** *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2007-2010 Intel Corporation. All Rights Reserved.


File Name: mfxvideo++.h

\* ****************************************************************************** */

#ifndef __MFXVIDEOPLUSPLUS_H
#define __MFXVIDEOPLUSPLUS_H

#include "mfxvideo.h"

class MFXVideoSession
{
public:
    MFXVideoSession(void) { m_session = (mfxSession) 0; }
    virtual ~MFXVideoSession(void) { Close(); }

    mfxStatus Init(mfxIMPL impl, mfxVersion *ver) { return MFXInit(impl, ver, &m_session); }
    mfxStatus Close(void)
    {
        mfxStatus mfxRes;
        mfxRes = MFXClose(m_session); m_session = (mfxSession) 0;
        return mfxRes;
    }

    mfxStatus QueryIMPL(mfxIMPL *impl) { return MFXQueryIMPL(m_session, impl); }
    mfxStatus QueryVersion(mfxVersion *version) { return MFXQueryVersion(m_session, version); }

    mfxStatus JoinSession(mfxSession child_session) { return MFXJoinSession(m_session, child_session);}
    mfxStatus DisjoinSession( ) { return MFXDisjoinSession(m_session);}
    mfxStatus CloneSession( mfxSession *clone) { return MFXCloneSession(m_session, clone);}
    mfxStatus SetPriority( mfxPriority priority) { return MFXSetPriority(m_session, priority);}
    mfxStatus GetPriority( mfxPriority *priority) { return MFXGetPriority(m_session, priority);}

    mfxStatus SetBufferAllocator(mfxBufferAllocator *allocator) { return MFXVideoCORE_SetBufferAllocator(m_session, allocator); }
    mfxStatus SetFrameAllocator(mfxFrameAllocator *allocator) { return MFXVideoCORE_SetFrameAllocator(m_session, allocator); }
    mfxStatus SetHandle(mfxHandleType type, mfxHDL hdl) { return MFXVideoCORE_SetHandle(m_session, type, hdl); }
    mfxStatus GetHandle(mfxHandleType type, mfxHDL *hdl) { return MFXVideoCORE_GetHandle(m_session, type, hdl); }

    mfxStatus SyncOperation(mfxSyncPoint syncp, mfxU32 wait) { return MFXVideoCORE_SyncOperation(m_session, syncp, wait); }

    operator mfxSession (void) { return m_session; }

protected:

    mfxSession m_session;                                       // (mfxSession) handle to the owning session
};

class MFXVideoENCODE
{
public:

    MFXVideoENCODE(mfxSession session) { m_session = session; }
    virtual ~MFXVideoENCODE(void) { Close(); }

    mfxStatus Query(mfxVideoParam *in, mfxVideoParam *out) { return MFXVideoENCODE_Query(m_session, in, out); }
    mfxStatus QueryIOSurf(mfxVideoParam *par, mfxFrameAllocRequest *request) { return MFXVideoENCODE_QueryIOSurf(m_session, par, request); }
    mfxStatus Init(mfxVideoParam *par) { return MFXVideoENCODE_Init(m_session, par); }
    mfxStatus Reset(mfxVideoParam *par) { return MFXVideoENCODE_Reset(m_session, par); }
    mfxStatus Close(void) { return MFXVideoENCODE_Close(m_session); }

    mfxStatus GetVideoParam(mfxVideoParam *par) { return MFXVideoENCODE_GetVideoParam(m_session, par); }
    mfxStatus GetEncodeStat(mfxEncodeStat *stat) { return MFXVideoENCODE_GetEncodeStat(m_session, stat); }

    mfxStatus EncodeFrameAsync(mfxEncodeCtrl *ctrl, mfxFrameSurface1 *surface, mfxBitstream *bs, mfxSyncPoint *syncp) { return MFXVideoENCODE_EncodeFrameAsync(m_session, ctrl, surface, bs, syncp); }

protected:

    mfxSession m_session;                                       // (mfxSession) handle to the owning session
};

class MFXVideoDECODE
{
public:

    MFXVideoDECODE(mfxSession session) { m_session = session; }
    virtual ~MFXVideoDECODE(void) { Close(); }

    mfxStatus Query(mfxVideoParam *in, mfxVideoParam *out) { return MFXVideoDECODE_Query(m_session, in, out); }
    mfxStatus DecodeHeader(mfxBitstream *bs, mfxVideoParam *par) { return MFXVideoDECODE_DecodeHeader(m_session, bs, par); }
    mfxStatus QueryIOSurf(mfxVideoParam *par, mfxFrameAllocRequest *request) { return MFXVideoDECODE_QueryIOSurf(m_session, par, request); }
    mfxStatus Init(mfxVideoParam *par) { return MFXVideoDECODE_Init(m_session, par); }
    mfxStatus Reset(mfxVideoParam *par) { return MFXVideoDECODE_Reset(m_session, par); }
    mfxStatus Close(void) { return MFXVideoDECODE_Close(m_session); }

    mfxStatus GetVideoParam(mfxVideoParam *par) { return MFXVideoDECODE_GetVideoParam(m_session, par); }

    mfxStatus GetDecodeStat(mfxDecodeStat *stat) { return MFXVideoDECODE_GetDecodeStat(m_session, stat); }
    mfxStatus GetPayload(mfxU64 *ts, mfxPayload *payload) {return MFXVideoDECODE_GetPayload(m_session, ts, payload); }
    mfxStatus SetSkipMode(mfxSkipMode mode) { return MFXVideoDECODE_SetSkipMode(m_session, mode); }
    mfxStatus DecodeFrameAsync(mfxBitstream *bs, mfxFrameSurface1 *surface_work, mfxFrameSurface1 **surface_out, mfxSyncPoint *syncp) { return MFXVideoDECODE_DecodeFrameAsync(m_session, bs, surface_work, surface_out, syncp); }

protected:

    mfxSession m_session;                                       // (mfxSession) handle to the owning session
};

class MFXVideoVPP
{
public:

    MFXVideoVPP(mfxSession session) { m_session = session; }
    virtual ~MFXVideoVPP(void) { Close(); }

    mfxStatus Query(mfxVideoParam *in, mfxVideoParam *out) { return MFXVideoVPP_Query(m_session, in, out); }
    mfxStatus QueryIOSurf(mfxVideoParam *par, mfxFrameAllocRequest request[2]) { return MFXVideoVPP_QueryIOSurf(m_session, par, request); }
    mfxStatus Init(mfxVideoParam *par) { return MFXVideoVPP_Init(m_session, par); }
    mfxStatus Reset(mfxVideoParam *par) { return MFXVideoVPP_Reset(m_session, par); }
    mfxStatus Close(void) { return MFXVideoVPP_Close(m_session); }

    mfxStatus GetVideoParam(mfxVideoParam *par) { return MFXVideoVPP_GetVideoParam(m_session, par); }
    mfxStatus GetVPPStat(mfxVPPStat *stat) { return MFXVideoVPP_GetVPPStat(m_session, stat); }
    mfxStatus RunFrameVPPAsync(mfxFrameSurface1 *in, mfxFrameSurface1 *out, mfxExtVppAuxData *aux, mfxSyncPoint *syncp) { return MFXVideoVPP_RunFrameVPPAsync(m_session, in, out, aux, syncp); }

protected:

    mfxSession m_session;                                       // (mfxSession) handle to the owning session
};

#endif // __MFXVIDEOPLUSPLUS_H
