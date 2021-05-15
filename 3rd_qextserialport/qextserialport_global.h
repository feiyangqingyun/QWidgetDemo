/****************************************************************************
** Copyright (c) 2000-2003 Wayne Roth
** Copyright (c) 2004-2007 Stefan Sander
** Copyright (c) 2007 Michal Policht
** Copyright (c) 2008 Brandon Fosdick
** Copyright (c) 2009-2010 Liam Staskawicz
** Copyright (c) 2011 Debao Zhang
** All right reserved.
** Web: http://code.google.com/p/qextserialport/
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

#ifndef QEXTSERIALPORT_GLOBAL_H
#define QEXTSERIALPORT_GLOBAL_H

#include <QtCore/QtGlobal>

#ifdef QEXTSERIALPORT_BUILD_SHARED
#  define QEXTSERIALPORT_EXPORT Q_DECL_EXPORT
#elif defined(QEXTSERIALPORT_USING_SHARED)
#  define QEXTSERIALPORT_EXPORT Q_DECL_IMPORT
#else
#  define QEXTSERIALPORT_EXPORT
#endif

// ### for compatible with old version. should be removed in QESP 2.0
#ifdef _TTY_NOWARN_
#  define QESP_NO_WARN
#endif
#ifdef _TTY_NOWARN_PORT_
#  define QESP_NO_PORTABILITY_WARN
#endif

/*if all warning messages are turned off, flag portability warnings to be turned off as well*/
#ifdef QESP_NO_WARN
#  define QESP_NO_PORTABILITY_WARN
#endif

/*macros for warning and debug messages*/
#ifdef QESP_NO_PORTABILITY_WARN
#  define QESP_PORTABILITY_WARNING  while (false)qWarning
#else
#  define QESP_PORTABILITY_WARNING qWarning
#endif /*QESP_NOWARN_PORT*/

#ifdef QESP_NO_WARN
#  define QESP_WARNING while (false)qWarning
#else
#  define QESP_WARNING qWarning
#endif /*QESP_NOWARN*/

#endif // QEXTSERIALPORT_GLOBAL_H

