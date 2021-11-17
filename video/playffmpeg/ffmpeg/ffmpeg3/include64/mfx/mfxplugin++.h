/* ****************************************************************************** *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 Intel Corporation. All Rights Reserved.


File Name: mfxplugin++.h

\* ****************************************************************************** */

#ifndef __MFXPLUGINPLUSPLUS_H
#define __MFXPLUGINPLUSPLUS_H

#include "mfxvideo.h"
#include "mfxplugin.h"

class MFXPlugin
{
public:
    virtual mfxStatus mfxPluginInit(mfxCoreInterface *core) = 0;
    virtual mfxStatus mfxPluginClose() = 0;
    virtual mfxStatus mfxGetPluginParam(mfxPluginParam *par) = 0;
    virtual mfxStatus mfxSubmit(const mfxHDL *in, mfxU32 in_num, const mfxHDL *out, mfxU32 out_num, mfxThreadTask *task) = 0;
    virtual mfxStatus mfxExecute(mfxThreadTask task, mfxU32 uid_p, mfxU32 uid_a) = 0;
    virtual mfxStatus mfxFreeResources(mfxThreadTask task, mfxStatus sts) = 0;    
};

/* Class adapter between "C" structure mfxPlugin and C++ interface MFXPlugin */
class MFXPluginAdapter : public mfxPlugin
{
public:
    MFXPluginAdapter(MFXPlugin *pPlugin)
    {
        pthis = pPlugin;
        PluginInit = MFXPluginAdapter::_PluginInit;
        PluginClose = MFXPluginAdapter::_PluginClose;
        GetPluginParam = MFXPluginAdapter::_GetPluginParam;
        Submit = MFXPluginAdapter::_Submit;
        Execute = MFXPluginAdapter::_Execute;
        FreeResources = MFXPluginAdapter::_FreeResources;
    }

private:
    static mfxStatus _PluginInit(mfxHDL pthis, mfxCoreInterface *core) { return ((MFXPlugin*)pthis)->mfxPluginInit(core); }
    static mfxStatus _PluginClose(mfxHDL pthis) { return ((MFXPlugin*)pthis)->mfxPluginClose(); }
    static mfxStatus _GetPluginParam(mfxHDL pthis, mfxPluginParam *par) { return ((MFXPlugin*)pthis)->mfxGetPluginParam(par); }
    static mfxStatus _Submit(mfxHDL pthis, const mfxHDL *in, mfxU32 in_num, const mfxHDL *out, mfxU32 out_num, mfxThreadTask *task) { return ((MFXPlugin*)pthis)->mfxSubmit(in, in_num, out, out_num, task); }
    static mfxStatus _Execute(mfxHDL pthis, mfxThreadTask task, mfxU32 thread_id, mfxU32 call_count) { return ((MFXPlugin*)pthis)->mfxExecute(task, thread_id, call_count); }
    static mfxStatus _FreeResources(mfxHDL pthis, mfxThreadTask task, mfxStatus sts) { return ((MFXPlugin*)pthis)->mfxFreeResources(task, sts); }
};

#endif // __MFXPLUGINPLUSPLUS_H
