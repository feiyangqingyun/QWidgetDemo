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

#include <QStringList>

/*!
    \class QxtWindowSystem
    \inmodule QxtWidgets
    \brief The QxtWindowSystem class provides means for accessing native windows.

    \bold {Note:} The underlying window system might or might not allow one to alter
    states of windows belonging to other processes.

    \warning QxtWindowSystem is portable in principle, but be careful while
    using it since you are probably about to do something non-portable.

    \section1 Advanced example usage:
    \code
    class NativeWindow : public QWidget {
        public:
            NativeWindow(WId wid) {
                QWidget::create(wid, false, false); // window, initializeWindow, destroyOldWindow
            }
            ~NativeWindow() {
                QWidget::destroy(false, false); // destroyWindow, destroySubWindows
            }
    };
    \endcode

    \code
    WindowList windows = QxtWindowSystem::windows();
    QStringList titles = QxtWindowSystem::windowTitles();
    bool ok = false;
    QString title = QInputDialog::getItem(0, "Choose Window", "Choose a window to be hid:", titles, 0, false, &ok);
    if (ok)
    {
        int index = titles.indexOf(title);
        if (index != -1)
        {
            NativeWindow window(windows.at(index));
            window.hide();
        }
    }
    \endcode

    \bold {Note:} Currently supported platforms are \bold X11 and \bold Windows.
 */

/*!
    \fn QxtWindowSystem::windows()

    Returns the list of native window system identifiers.

    \sa QApplication::topLevelWidgets(), windowTitles()
 */

/*!
    \fn QxtWindowSystem::activeWindow()

    Returns the native window system identifier of the active window if any.

    \sa QApplication::activeWindow()
 */

/*!
    \fn QxtWindowSystem::findWindow(const QString& title)

    Returns the native window system identifier of the window if any with given \a title.

    Example usage:
    \code
    WId wid = QxtWindowSystem::findWindow("Mail - Kontact");
    QPixmap screenshot = QPixmap::grabWindow(wid);
    \endcode

    \sa QWidget::find()
 */

/*!
    \fn QxtWindowSystem::windowAt(const QPoint& pos)

    Returns the native window system identifier of the window if any at \a pos.

    \sa QApplication::widgetAt()
 */

/*!
    \fn QxtWindowSystem::windowTitle(WId window)

    Returns the title of the native \a window.

    \sa QWidget::windowTitle(), windowTitles()
 */

/*!
    \fn QxtWindowSystem::windowTitles()

    Returns a list of native window titles.

    \sa QWidget::windowTitle(), windowTitle(), windows()
 */

/*!
    \fn QxtWindowSystem::windowGeometry(WId window)

    Returns the geometry of the native \a window.

    \sa QWidget::frameGeometry()
 */

/*!
    \fn QxtWindowSystem::idleTime()

    Returns the system "idle time" ie. the time since last user input
    in milliseconds.
 */

QStringList QxtWindowSystem::windowTitles()
{
    WindowList windows = QxtWindowSystem::windows();
    QStringList titles;
    foreach(WId window, windows)
    titles += QxtWindowSystem::windowTitle(window);
    return titles;
}
