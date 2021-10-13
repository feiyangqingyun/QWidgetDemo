#ifndef QXTWINDOWSYSTEM_MAC_CPP
#define QXTWINDOWSYSTEM_MAC_CPP

#include "qxtwindowsystem.h"
#include "qxtwindowsystem_mac.h"

// WId to return when error
#define WINDOW_NOT_FOUND (WId)(0)

WindowList qxt_getWindowsForPSN(ProcessSerialNumber *psn)
{
    static CGSConnection connection = _CGSDefaultConnection();

    WindowList wlist;
    if (!psn) return wlist;

    CGError err((CGError)noErr);

    // get onnection for given process psn
    CGSConnection procConnection;
    err = CGSGetConnectionIDForPSN(connection, psn, &procConnection);
    if (err != noErr) return wlist;

    /* get number of windows open by given process
       in Mac OS X an application may have multiple windows, which generally
       represent documents. It is also possible that there is no window even
       though there is an application, it can simply not have any documents open. */

    int windowCount(0);
    err = CGSGetOnScreenWindowCount(connection, procConnection, &windowCount);
    // if there are no windows open by this application, skip
    if (err != noErr || windowCount == 0) return wlist;

    // get list of windows
    int windowList[windowCount];
    int outCount(0);
    err = CGSGetOnScreenWindowList(connection, procConnection, windowCount, windowList, &outCount);

    if (err != noErr || outCount == 0) return wlist;

    for (int i=0; i<outCount; ++i)
    {
        wlist += windowList[i];
    }

    return wlist;
}


WindowList QxtWindowSystem::windows()
{
    WindowList wlist;
    ProcessSerialNumber psn = {0, kNoProcess};

    // iterate over list of processes
    OSErr err;
    while ((err = ::GetNextProcess(&psn)) == noErr)
    {
        wlist += qxt_getWindowsForPSN(&psn);
    }

    return wlist;
}

WId QxtWindowSystem::activeWindow()
{
    ProcessSerialNumber psn;
    OSErr err(noErr);
    err = ::GetFrontProcess(&psn);
    if (err != noErr) return WINDOW_NOT_FOUND;

    // in Mac OS X, first window for given PSN is always the active one
    WindowList wlist = qxt_getWindowsForPSN(&psn);

    if (wlist.count() > 0)
        return wlist.at(0);

    return WINDOW_NOT_FOUND;
}

QString QxtWindowSystem::windowTitle(WId window)
{
    CGSValue windowTitle;
    CGError err((CGError)noErr);
    static CGSConnection connection = _CGSDefaultConnection();

    // This code is so dirty I had to wash my hands after writing it.

    // most of CoreGraphics private definitions ask for CGSValue as key but since
    // converting strings to/from CGSValue was dropped in 10.5, I use CFString, which
    // apparently also works.

    // FIXME: Not public API function. Can't compile with OS X 10.8
    // err = CGSGetWindowProperty(connection, window, (CGSValue)CFSTR("kCGSWindowTitle"), &windowTitle);
    if (err != noErr) return QString();

    // this is UTF8 encoded
    return QCFString::toQString((CFStringRef)windowTitle);
}

QRect QxtWindowSystem::windowGeometry(WId window)
{
    CGRect rect;
    static CGSConnection connection = _CGSDefaultConnection();

    CGError err = CGSGetWindowBounds(connection, window, &rect);
    if (err != noErr) return QRect();

    return QRect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

/* This method is the only one that is not a complete hack
   from Quartz Event Services
   http://developer.apple.com/library/mac/#documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html
*/
uint QxtWindowSystem::idleTime()
{
    // CGEventSourceSecondsSinceLastEventType returns time in seconds as a double
    // also has extremely long name
    double idle = 1000 * ::CGEventSourceSecondsSinceLastEventType(kCGEventSourceStateCombinedSessionState, kCGAnyInputEventType);
    return (uint)idle;
}


// these are copied from X11 implementation
WId QxtWindowSystem::findWindow(const QString& title)
{
    WId result = 0;
    WindowList list = windows();
    foreach(const WId &wid, list)
    {
        if (windowTitle(wid) == title)
        {
            result = wid;
            break;
        }
    }
    return result;
}

WId QxtWindowSystem::windowAt(const QPoint& pos)
{
    WId result = 0;
    WindowList list = windows();
    for (int i = list.size() - 1; i >= 0; --i)
    {
        WId wid = list.at(i);
        if (windowGeometry(wid).contains(pos))
        {
            result = wid;
            break;
        }
    }
    return result;
}

#endif // QXTWINDOWSYSTEM_MAC_CPP

