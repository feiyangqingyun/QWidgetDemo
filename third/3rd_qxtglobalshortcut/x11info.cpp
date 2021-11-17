/*
 * Copyright (C) 2013  Il'inykh Sergey (rion)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "x11info.h"

#ifdef HAVE_QT5
# include <X11/Xlib.h>
# include <xcb/xcb.h>
# include <QtGlobal>
#else
# include <QX11Info>
#endif


Display* X11Info::display()
{
#ifdef HAVE_QT5
	if (!_display) {
		_display = XOpenDisplay(NULL);
	}
	return _display;
#else
	return QX11Info::display();
#endif
}

unsigned long X11Info::appRootWindow(int screen)
{
#ifdef HAVE_QT5
	return screen == -1?
				XDefaultRootWindow(display()) :
				XRootWindowOfScreen(XScreenOfDisplay(display(), screen));
#else
	return QX11Info::appRootWindow(screen);
#endif
}

int X11Info::appScreen()
{
#ifdef HAVE_QT5
	#error X11Info::appScreen not implemented for Qt5! You must skip this plugin.
#else
	return QX11Info::appScreen();
#endif
}

#ifdef HAVE_QT5
xcb_connection_t *X11Info::xcbConnection()
{
	if (!_xcb) {
		_xcb = xcb_connect(NULL, &_xcbPreferredScreen);
		Q_ASSERT(_xcb);
	}
	return _xcb;
}

xcb_connection_t* X11Info::_xcb = 0;
#endif

Display* X11Info::_display = 0;
int X11Info::_xcbPreferredScreen = 0;
