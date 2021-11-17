
/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/

/****************************************************************************
 **
 ** Copyright (C) Marcin Jakubowski <mjakubowski (at) gmail (dot) com
 **
 ** Defines some of required CoreGraphics private parts. This code may
 ** break at any time and uses *undocumented* features of Mac OS X.
 **
 ** Also defines QCFString, part of Qt code found in src/corelib/kernel
 ** qcoremac_p.h file.
 ****************************************************************************/

#ifndef QXTWINDOWSYSTEM_MAC_H
#define QXTWINDOWSYSTEM_MAC_H

//
//  CGSPrivate.h
//  Header file for undocumented CoreGraphics SPI
//
//  Arranged by Nicholas Jitkoff
//  Based on CGSPrivate.h by Richard Wareham
//
//  Contributors:
//    Austin Sarner: Shadows
//    Jason Harris: Filters, Shadows, Regions
//    Kevin Ballard: Warping
//    Steve Voida: Workspace notifications
//    Tony Arnold: Workspaces notifications enum filters
//    Ben Gertzfield: CGSRemoveConnectionNotifyProc
//
//  Changes:
//    2.3 - Added the CGSRemoveConnectionNotifyProc method with the help of Ben Gertzfield
//    2.2 - Moved back to CGSPrivate, added more enums to the CGSConnectionNotifyEvent
//    2.1 - Added spaces notifications
//    2.0 - Original Release

#include <Carbon/Carbon.h>

#define CGSConnectionID CGSConnection
#define CGSWindowID CGSWindow
#define CGSDefaultConnection _CGSDefaultConnection()

#ifdef __cplusplus
extern "C" {
#endif

  typedef int CGSConnection;
  typedef int CGSWindow;
  typedef int CGSWorkspace;
  typedef void* CGSValue;

  /* Get the default connection for the current process. */
  extern CGSConnection _CGSDefaultConnection(void);

  /* /MJakubowski/ Get connection for given PSN */
  extern CGError CGSGetConnectionIDForPSN(const CGSConnection connection, ProcessSerialNumber * psn, CGSConnection * targetConnection);

  // Get on-screen window counts and lists.
  extern CGError CGSGetOnScreenWindowCount(const CGSConnection cid, CGSConnection targetCID, int* outCount);
  extern CGError CGSGetOnScreenWindowList(const CGSConnection cid, CGSConnection targetCID, int count, int* list, int* outCount);
  // Position
  extern CGError CGSGetWindowBounds(CGSConnection cid, CGSWindowID wid, CGRect *outBounds);
  extern CGError CGSGetScreenRectForWindow(const CGSConnection cid, CGSWindow wid, CGRect *outRect);

  // Properties
  extern CGError CGSGetWindowProperty(const CGSConnection cid, CGSWindow wid, CGSValue key, CGSValue *outValue);


#ifdef __cplusplus
}
#endif

/* QCFString from Qt */
#include <QString>
template <typename T>
class QCFType
{
public:
    inline QCFType(const T &t = 0) : type(t) {}
    inline QCFType(const QCFType &helper) : type(helper.type) { if (type) CFRetain(type); }
    inline ~QCFType() { if (type) CFRelease(type); }
    inline operator T() { return type; }
    inline QCFType operator =(const QCFType &helper)
    {
        if (helper.type)
            CFRetain(helper.type);
        CFTypeRef type2 = type;
        type = helper.type;
        if (type2)
            CFRelease(type2);
        return *this;
    }
    inline T *operator&() { return &type; }
    static QCFType constructFromGet(const T &t)
    {
        CFRetain(t);
        return QCFType<T>(t);
    }
protected:
    T type;
};

class QCFString : public QCFType<CFStringRef>
{
public:
    inline QCFString(const QString &str) : QCFType<CFStringRef>(0), string(str) {}
    inline QCFString(const CFStringRef cfstr = 0) : QCFType<CFStringRef>(cfstr) {}
    inline QCFString(const QCFType<CFStringRef> &other) : QCFType<CFStringRef>(other) {}
    operator QString() const;
    operator CFStringRef() const;
    static QString toQString(CFStringRef cfstr);
    static CFStringRef toCFStringRef(const QString &str);
private:
    QString string;
};

#endif // QXTWINDOWSYSTEM_MAC_H

