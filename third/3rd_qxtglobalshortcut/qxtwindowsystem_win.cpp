#include "qxtwindowsystem.h"
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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#include <qt_windows.h>
#include <qglobal.h> // QT_WA

static WindowList qxt_Windows;

BOOL CALLBACK qxt_EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    Q_UNUSED(lParam);
    if (::IsWindowVisible(hwnd))
        qxt_Windows += (WId)hwnd;
    return true;
}

WindowList QxtWindowSystem::windows()
{
    qxt_Windows.clear();
    HDESK hdesk = ::OpenInputDesktop(0, false, DESKTOP_READOBJECTS);
    ::EnumDesktopWindows(hdesk, qxt_EnumWindowsProc, 0);
    ::CloseDesktop(hdesk);
    return qxt_Windows;
}

WId QxtWindowSystem::activeWindow()
{
    return (WId)::GetForegroundWindow();
}

WId QxtWindowSystem::findWindow(const QString& title)
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QT_WA({
        return (WId)::FindWindow(NULL, (TCHAR*)title.utf16());
    }, {
        return (WId)::FindWindowA(NULL, title.toLocal8Bit());
    });
#else
    return (WId)::FindWindow(NULL, (TCHAR*)title.utf16());
#endif
}

WId QxtWindowSystem::windowAt(const QPoint& pos)
{
    POINT pt;
    pt.x = pos.x();
    pt.y = pos.y();
    return (WId)::WindowFromPoint(pt);
}

QString QxtWindowSystem::windowTitle(WId window)
{
    QString title;
    int len = ::GetWindowTextLength((HWND)window);
    if (len >= 0)
    {
        TCHAR* buf = new TCHAR[len+1];
        len = ::GetWindowText((HWND)window, buf, len+1);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        QT_WA({
            title = QString::fromUtf16((const ushort*)buf, len);
        }, {
            title = QString::fromLocal8Bit((const char*)buf, len);
        });
#else
            title = QString::fromUtf16((const ushort*)buf, len);
#endif
        delete[] buf;
    }
    return title;
}

QRect QxtWindowSystem::windowGeometry(WId window)
{
    RECT rc;
    QRect rect;
    if (::GetWindowRect((HWND)window, &rc))
    {
        rect.setTop(rc.top);
        rect.setBottom(rc.bottom);
        rect.setLeft(rc.left);
        rect.setRight(rc.right);
    }
    return rect;
}

uint QxtWindowSystem::idleTime()
{
    uint idle = -1;
    LASTINPUTINFO info;
    info.cbSize = sizeof(LASTINPUTINFO);
    if (::GetLastInputInfo(&info))
        idle = ::GetTickCount() - info.dwTime;
    return idle;
}
