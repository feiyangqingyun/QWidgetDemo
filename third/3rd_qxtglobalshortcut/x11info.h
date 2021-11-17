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

#ifndef X11INFO_H
#define X11INFO_H

typedef struct _XDisplay Display;
#ifdef HAVE_QT5
typedef struct xcb_connection_t xcb_connection_t;
#endif

class X11Info
{
	static Display *_display;
#ifdef HAVE_QT5
	static xcb_connection_t *_xcb;
#endif
	static int _xcbPreferredScreen;

public:
	static Display* display();
	static unsigned long appRootWindow(int screen = -1);
	static int appScreen();
#ifdef HAVE_QT5
	static xcb_connection_t* xcbConnection();
	static inline int xcbPreferredScreen() { return _xcbPreferredScreen; }
#endif
};

#endif // X11INFO_H
