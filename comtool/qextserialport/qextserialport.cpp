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

#include "qextserialport.h"
#include "qextserialport_p.h"
#include <stdio.h>
#include <QtCore/QDebug>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>

/*!
    \class PortSettings

    \brief The PortSettings class contain port settings

    Structure to contain port settings.

    \code
    BaudRateType BaudRate;
    DataBitsType DataBits;
    ParityType Parity;
    StopBitsType StopBits;
    FlowType FlowControl;
    long Timeout_Millisec;
    \endcode
*/

QextSerialPortPrivate::QextSerialPortPrivate(QextSerialPort *q)
	: lock(QReadWriteLock::Recursive), q_ptr(q)
{
	lastErr = E_NO_ERROR;
	settings.BaudRate = BAUD9600;
	settings.Parity = PAR_NONE;
	settings.FlowControl = FLOW_OFF;
	settings.DataBits = DATA_8;
	settings.StopBits = STOP_1;
	settings.Timeout_Millisec = 10;
	settingsDirtyFlags = DFE_ALL;

	platformSpecificInit();
}

QextSerialPortPrivate::~QextSerialPortPrivate()
{
	platformSpecificDestruct();
}

void QextSerialPortPrivate::setBaudRate(BaudRateType baudRate, bool update)
{
	switch (baudRate) {
#ifdef Q_OS_WIN

		//Windows Special
		case BAUD14400:
		case BAUD56000:
		case BAUD128000:
		case BAUD256000:
			QESP_PORTABILITY_WARNING() << "QextSerialPort Portability Warning: POSIX does not support baudRate:" << baudRate;
#elif defined(Q_OS_UNIX)

		//Unix Special
		case BAUD50:
		case BAUD75:
		case BAUD134:
		case BAUD150:
		case BAUD200:
		case BAUD1800:
#  ifdef B76800
		case BAUD76800:
#  endif
#  if defined(B230400) && defined(B4000000)
		case BAUD230400:
		case BAUD460800:
		case BAUD500000:
		case BAUD576000:
		case BAUD921600:
		case BAUD1000000:
		case BAUD1152000:
		case BAUD1500000:
		case BAUD2000000:
		case BAUD2500000:
		case BAUD3000000:
		case BAUD3500000:
		case BAUD4000000:
#  endif
			QESP_PORTABILITY_WARNING() << "QextSerialPort Portability Warning: Windows does not support baudRate:" << baudRate;
#endif

		case BAUD110:
		case BAUD300:
		case BAUD600:
		case BAUD1200:
		case BAUD2400:
		case BAUD4800:
		case BAUD9600:
		case BAUD19200:
		case BAUD38400:
		case BAUD57600:
		case BAUD115200:
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
		default:
#endif
			settings.BaudRate = baudRate;
			settingsDirtyFlags |= DFE_BaudRate;

			if (update && q_func()->isOpen()) {
				updatePortSettings();
			}

			break;
#if !(defined(Q_OS_WIN) || defined(Q_OS_MAC))

		default:
			QESP_WARNING() << "QextSerialPort does not support baudRate:" << baudRate;
#endif
	}
}

void QextSerialPortPrivate::setParity(ParityType parity, bool update)
{
	switch (parity) {
		case PAR_SPACE:
			if (settings.DataBits == DATA_8) {
#ifdef Q_OS_WIN
				QESP_PORTABILITY_WARNING("QextSerialPort Portability Warning: Space parity with 8 data bits is not supported by POSIX systems.");
#else
				QESP_WARNING("Space parity with 8 data bits is not supported by POSIX systems.");
#endif
			}

			break;

#ifdef Q_OS_WIN

		/*mark parity - WINDOWS ONLY*/
		case PAR_MARK:
			QESP_PORTABILITY_WARNING("QextSerialPort Portability Warning:  Mark parity is not supported by POSIX systems");
			break;
#endif

		case PAR_NONE:
		case PAR_EVEN:
		case PAR_ODD:
			break;

		default:
			QESP_WARNING() << "QextSerialPort does not support Parity:" << parity;
	}

	settings.Parity = parity;
	settingsDirtyFlags |= DFE_Parity;

	if (update && q_func()->isOpen()) {
		updatePortSettings();
	}
}

void QextSerialPortPrivate::setDataBits(DataBitsType dataBits, bool update)
{
	switch (dataBits) {

		case DATA_5:
			if (settings.StopBits == STOP_2) {
				QESP_WARNING("QextSerialPort: 5 Data bits cannot be used with 2 stop bits.");
			} else {
				settings.DataBits = dataBits;
				settingsDirtyFlags |= DFE_DataBits;
			}

			break;

		case DATA_6:
#ifdef Q_OS_WIN
			if (settings.StopBits == STOP_1_5) {
				QESP_WARNING("QextSerialPort: 6 Data bits cannot be used with 1.5 stop bits.");
			} else
#endif
			{
				settings.DataBits = dataBits;
				settingsDirtyFlags |= DFE_DataBits;
			}

			break;

		case DATA_7:
#ifdef Q_OS_WIN
			if (settings.StopBits == STOP_1_5) {
				QESP_WARNING("QextSerialPort: 7 Data bits cannot be used with 1.5 stop bits.");
			} else
#endif
			{
				settings.DataBits = dataBits;
				settingsDirtyFlags |= DFE_DataBits;
			}

			break;

		case DATA_8:
#ifdef Q_OS_WIN
			if (settings.StopBits == STOP_1_5) {
				QESP_WARNING("QextSerialPort: 8 Data bits cannot be used with 1.5 stop bits.");
			} else
#endif
			{
				settings.DataBits = dataBits;
				settingsDirtyFlags |= DFE_DataBits;
			}

			break;

		default:
			QESP_WARNING() << "QextSerialPort does not support Data bits:" << dataBits;
	}

	if (update && q_func()->isOpen()) {
		updatePortSettings();
	}
}

void QextSerialPortPrivate::setStopBits(StopBitsType stopBits, bool update)
{
	switch (stopBits) {

		/*one stop bit*/
		case STOP_1:
			settings.StopBits = stopBits;
			settingsDirtyFlags |= DFE_StopBits;
			break;

#ifdef Q_OS_WIN

		/*1.5 stop bits*/
		case STOP_1_5:
			QESP_PORTABILITY_WARNING("QextSerialPort Portability Warning: 1.5 stop bit operation is not supported by POSIX.");

			if (settings.DataBits != DATA_5) {
				QESP_WARNING("QextSerialPort: 1.5 stop bits can only be used with 5 data bits");
			} else {
				settings.StopBits = stopBits;
				settingsDirtyFlags |= DFE_StopBits;
			}

			break;
#endif

		/*two stop bits*/
		case STOP_2:
			if (settings.DataBits == DATA_5) {
				QESP_WARNING("QextSerialPort: 2 stop bits cannot be used with 5 data bits");
			} else {
				settings.StopBits = stopBits;
				settingsDirtyFlags |= DFE_StopBits;
			}

			break;

		default:
			QESP_WARNING() << "QextSerialPort does not support stop bits: " << stopBits;
	}

	if (update && q_func()->isOpen()) {
		updatePortSettings();
	}
}

void QextSerialPortPrivate::setFlowControl(FlowType flow, bool update)
{
	settings.FlowControl = flow;
	settingsDirtyFlags |= DFE_Flow;

	if (update && q_func()->isOpen()) {
		updatePortSettings();
	}
}

void QextSerialPortPrivate::setTimeout(long millisec, bool update)
{
	settings.Timeout_Millisec = millisec;
	settingsDirtyFlags |= DFE_TimeOut;

	if (update && q_func()->isOpen()) {
		updatePortSettings();
	}
}

void QextSerialPortPrivate::setPortSettings(const PortSettings &settings, bool update)
{
	setBaudRate(settings.BaudRate, false);
	setDataBits(settings.DataBits, false);
	setStopBits(settings.StopBits, false);
	setParity(settings.Parity, false);
	setFlowControl(settings.FlowControl, false);
	setTimeout(settings.Timeout_Millisec, false);
	settingsDirtyFlags = DFE_ALL;

	if (update && q_func()->isOpen()) {
		updatePortSettings();
	}
}


void QextSerialPortPrivate::_q_canRead()
{
	qint64 maxSize = bytesAvailable_sys();

	if (maxSize > 0) {
		char *writePtr = readBuffer.reserve(size_t(maxSize));
		qint64 bytesRead = readData_sys(writePtr, maxSize);

		if (bytesRead < maxSize) {
			readBuffer.chop(maxSize - bytesRead);
		}

		Q_Q(QextSerialPort);
		Q_EMIT q->readyRead();
	}
}

/*! \class QextSerialPort

    \brief The QextSerialPort class encapsulates a serial port on both POSIX and Windows systems.

    \section1 Usage
    QextSerialPort offers both a polling and event driven API.  Event driven
    is typically easier to use, since you never have to worry about checking
    for new data.

    \bold Example
    \code
    QextSerialPort *port = new QextSerialPort("COM1");
    connect(port, SIGNAL(readyRead()), myClass, SLOT(onDataAvailable()));
    port->open();

    void MyClass::onDataAvailable()
    {
        QByteArray data = port->readAll();
        processNewData(usbdata);
    }
    \endcode

    \section1 Compatibility
    The user will be notified of errors and possible portability conflicts at run-time
    by default.

    For example, if a application has used BAUD1800, when it is runing under unix, you
    will get following message.

    \code
    QextSerialPort Portability Warning: Windows does not support baudRate:1800
    \endcode

    This behavior can be turned off by defining macro QESP_NO_WARN (to turn off all warnings)
    or QESP_NO_PORTABILITY_WARN (to turn off portability warnings) in the project.


    \bold Author: Stefan Sander, Michal Policht, Brandon Fosdick, Liam Staskawicz, Debao Zhang
*/

/*!
  \enum QextSerialPort::QueryMode

  This enum type specifies query mode used in a serial port:

  \value Polling
     asynchronously read and write
  \value EventDriven
     synchronously read and write
*/

/*!
    \fn void QextSerialPort::dsrChanged(bool status)
    This signal is emitted whenever dsr line has changed its state. You may
    use this signal to check if device is connected.

    \a status true when DSR signal is on, false otherwise.
 */


/*!
    \fn QueryMode QextSerialPort::queryMode() const
    Get query mode.
 */

/*!
    Default constructor.  Note that the name of the device used by a QextSerialPort is dependent on
    your OS. Possible naming conventions and their associated OS are:

    \code

    OS Constant       Used By         Naming Convention
    -------------     -------------   ------------------------
    Q_OS_WIN          Windows         COM1, COM2
    Q_OS_IRIX         SGI/IRIX        /dev/ttyf1, /dev/ttyf2
    Q_OS_HPUX         HP-UX           /dev/tty1p0, /dev/tty2p0
    Q_OS_SOLARIS      SunOS/Slaris    /dev/ttya, /dev/ttyb
    Q_OS_OSF          Digital UNIX    /dev/tty01, /dev/tty02
    Q_OS_FREEBSD      FreeBSD         /dev/ttyd0, /dev/ttyd1
    Q_OS_OPENBSD      OpenBSD         /dev/tty00, /dev/tty01
    Q_OS_LINUX        Linux           /dev/ttyS0, /dev/ttyS1
    <none>                            /dev/ttyS0, /dev/ttyS1
    \endcode

    This constructor assigns the device name to the name of the first port on the specified system.
    See the other constructors if you need to open a different port. Default \a mode is EventDriven.
    As a subclass of QObject, \a parent can be specified.
*/

QextSerialPort::QextSerialPort(QextSerialPort::QueryMode mode, QObject *parent)
	: QIODevice(parent), d_ptr(new QextSerialPortPrivate(this))
{
#ifdef Q_OS_WIN
	setPortName(QLatin1String("COM1"));

#elif defined(Q_OS_IRIX)
	setPortName(QLatin1String("/dev/ttyf1"));

#elif defined(Q_OS_HPUX)
	setPortName(QLatin1String("/dev/tty1p0"));

#elif defined(Q_OS_SOLARIS)
	setPortName(QLatin1String("/dev/ttya"));

#elif defined(Q_OS_OSF) //formally DIGITAL UNIX
	setPortName(QLatin1String("/dev/tty01"));

#elif defined(Q_OS_FREEBSD)
	setPortName(QLatin1String("/dev/ttyd1"));

#elif defined(Q_OS_OPENBSD)
	setPortName(QLatin1String("/dev/tty00"));

#else
	setPortName(QLatin1String("/dev/ttyS0"));
#endif
	setQueryMode(mode);
}

/*!
    Constructs a serial port attached to the port specified by name.
    \a name is the name of the device, which is windowsystem-specific,
    e.g."COM1" or "/dev/ttyS0". \a mode
*/
QextSerialPort::QextSerialPort(const QString &name, QextSerialPort::QueryMode mode, QObject *parent)
	: QIODevice(parent), d_ptr(new QextSerialPortPrivate(this))
{
	setQueryMode(mode);
	setPortName(name);
}

/*!
    Constructs a port with default name and specified \a settings.
*/
QextSerialPort::QextSerialPort(const PortSettings &settings, QextSerialPort::QueryMode mode, QObject *parent)
	: QIODevice(parent), d_ptr(new QextSerialPortPrivate(this))
{
	Q_D(QextSerialPort);
	setQueryMode(mode);
	d->setPortSettings(settings);
}

/*!
    Constructs a port with specified \a name , \a mode and \a settings.
*/
QextSerialPort::QextSerialPort(const QString &name, const PortSettings &settings, QextSerialPort::QueryMode mode, QObject *parent)
	: QIODevice(parent), d_ptr(new QextSerialPortPrivate(this))
{
	Q_D(QextSerialPort);
	setPortName(name);
	setQueryMode(mode);
	d->setPortSettings(settings);
}

/*!
    Opens a serial port and sets its OpenMode to \a mode.
    Note that this function does not specify which device to open.
    Returns true if successful; otherwise returns false.This function has no effect
    if the port associated with the class is already open.  The port is also
    configured to the current settings, as stored in the settings structure.
*/
bool QextSerialPort::open(OpenMode mode)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (mode != QIODevice::NotOpen && !isOpen()) {
		d->open_sys(mode);
	}

	return isOpen();
}


/*! \reimp
    Closes a serial port.  This function has no effect if the serial port associated with the class
    is not currently open.
*/
void QextSerialPort::close()
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (isOpen()) {
		// Be a good QIODevice and call QIODevice::close() before really close()
		//  so the aboutToClose() signal is emitted at the proper time
		QIODevice::close(); // mark ourselves as closed
		d->close_sys();
		d->readBuffer.clear();
	}
}

/*!
    Flushes all pending I/O to the serial port.  This function has no effect if the serial port
    associated with the class is not currently open.
*/
void QextSerialPort::flush()
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (isOpen()) {
		d->flush_sys();
	}
}

/*! \reimp
    Returns the number of bytes waiting in the port's receive queue.  This function will return 0 if
    the port is not currently open, or -1 on error.
*/
qint64 QextSerialPort::bytesAvailable() const
{
	QWriteLocker locker(&d_func()->lock);

	if (isOpen()) {
		qint64 bytes = d_func()->bytesAvailable_sys();

		if (bytes != -1) {
			return bytes + d_func()->readBuffer.size()
			       + QIODevice::bytesAvailable();
		} else {
			return -1;
		}
	}

	return 0;
}

/*! \reimp

*/
bool QextSerialPort::canReadLine() const
{
	QReadLocker locker(&d_func()->lock);
	return QIODevice::canReadLine() || d_func()->readBuffer.canReadLine();
}

/*!
 * Set desired serial communication handling style. You may choose from polling
 * or event driven approach. This function does nothing when port is open; to
 * apply changes port must be reopened.
 *
 * In event driven approach read() and write() functions are acting
 * asynchronously. They return immediately and the operation is performed in
 * the background, so they doesn't freeze the calling thread.
 * To determine when operation is finished, QextSerialPort runs separate thread
 * and monitors serial port events. Whenever the event occurs, adequate signal
 * is emitted.
 *
 * When polling is set, read() and write() are acting synchronously. Signals are
 * not working in this mode and some functions may not be available. The advantage
 * of polling is that it generates less overhead due to lack of signals emissions
 * and it doesn't start separate thread to monitor events.
 *
 * Generally event driven approach is more capable and friendly, although some
 * applications may need as low overhead as possible and then polling comes.
 *
 * \a mode query mode.
 */
void QextSerialPort::setQueryMode(QueryMode mode)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (mode != d->queryMode) {
		d->queryMode = mode;
	}
}

/*!
    Sets the \a name of the device associated with the object, e.g. "COM1", or "/dev/ttyS0".
*/
void QextSerialPort::setPortName(const QString &name)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);
	d->port = name;
}

/*!
    Returns the name set by setPortName().
*/
QString QextSerialPort::portName() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->port;
}

QextSerialPort::QueryMode QextSerialPort::queryMode() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->queryMode;
}

/*!
    Reads all available data from the device, and returns it as a QByteArray.
    This function has no way of reporting errors; returning an empty QByteArray()
    can mean either that no data was currently available for reading, or that an error occurred.
*/
QByteArray QextSerialPort::readAll()
{
	int avail = this->bytesAvailable();
	return (avail > 0) ? this->read(avail) : QByteArray();
}

/*!
    Returns the baud rate of the serial port.  For a list of possible return values see
    the definition of the enum BaudRateType.
*/
BaudRateType QextSerialPort::baudRate() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->settings.BaudRate;
}

/*!
    Returns the number of data bits used by the port.  For a list of possible values returned by
    this function, see the definition of the enum DataBitsType.
*/
DataBitsType QextSerialPort::dataBits() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->settings.DataBits;
}

/*!
    Returns the type of parity used by the port.  For a list of possible values returned by
    this function, see the definition of the enum ParityType.
*/
ParityType QextSerialPort::parity() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->settings.Parity;
}

/*!
    Returns the number of stop bits used by the port.  For a list of possible return values, see
    the definition of the enum StopBitsType.
*/
StopBitsType QextSerialPort::stopBits() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->settings.StopBits;
}

/*!
    Returns the type of flow control used by the port.  For a list of possible values returned
    by this function, see the definition of the enum FlowType.
*/
FlowType QextSerialPort::flowControl() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->settings.FlowControl;
}

/*!
    \reimp
    Returns true if device is sequential, otherwise returns false. Serial port is sequential device
    so this function always returns true. Check QIODevice::isSequential() documentation for more
    information.
*/
bool QextSerialPort::isSequential() const
{
	return true;
}

/*!
    Return the error number, or 0 if no error occurred.
*/
ulong QextSerialPort::lastError() const
{
	QReadLocker locker(&d_func()->lock);
	return d_func()->lastErr;
}

/*!
    Returns the line status as stored by the port function.  This function will retrieve the states
    of the following lines: DCD, CTS, DSR, and RI.  On POSIX systems, the following additional lines
    can be monitored: DTR, RTS, Secondary TXD, and Secondary RXD.  The value returned is an unsigned
    long with specific bits indicating which lines are high.  The following constants should be used
    to examine the states of individual lines:

    \code
    Mask        Line
    ------      ----
    LS_CTS      CTS
    LS_DSR      DSR
    LS_DCD      DCD
    LS_RI       RI
    LS_RTS      RTS (POSIX only)
    LS_DTR      DTR (POSIX only)
    LS_ST       Secondary TXD (POSIX only)
    LS_SR       Secondary RXD (POSIX only)
    \endcode

    This function will return 0 if the port associated with the class is not currently open.
*/
unsigned long QextSerialPort::lineStatus()
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (isOpen()) {
		return d->lineStatus_sys();
	}

	return 0;
}

/*!
  Returns a human-readable description of the last device error that occurred.
*/
QString QextSerialPort::errorString()
{
	Q_D(QextSerialPort);
	QReadLocker locker(&d->lock);

	switch (d->lastErr) {
		case E_NO_ERROR:
			return tr("No Error has occurred");

		case E_INVALID_FD:
			return tr("Invalid file descriptor (port was not opened correctly)");

		case E_NO_MEMORY:
			return tr("Unable to allocate memory tables (POSIX)");

		case E_CAUGHT_NON_BLOCKED_SIGNAL:
			return tr("Caught a non-blocked signal (POSIX)");

		case E_PORT_TIMEOUT:
			return tr("Operation timed out (POSIX)");

		case E_INVALID_DEVICE:
			return tr("The file opened by the port is not a valid device");

		case E_BREAK_CONDITION:
			return tr("The port detected a break condition");

		case E_FRAMING_ERROR:
			return tr("The port detected a framing error (usually caused by incorrect baud rate settings)");

		case E_IO_ERROR:
			return tr("There was an I/O error while communicating with the port");

		case E_BUFFER_OVERRUN:
			return tr("Character buffer overrun");

		case E_RECEIVE_OVERFLOW:
			return tr("Receive buffer overflow");

		case E_RECEIVE_PARITY_ERROR:
			return tr("The port detected a parity error in the received data");

		case E_TRANSMIT_OVERFLOW:
			return tr("Transmit buffer overflow");

		case E_READ_FAILED:
			return tr("General read operation failure");

		case E_WRITE_FAILED:
			return tr("General write operation failure");

		case E_FILE_NOT_FOUND:
			return tr("The %1 file doesn't exists").arg(this->portName());

		case E_PERMISSION_DENIED:
			return tr("Permission denied");

		case E_AGAIN:
			return tr("Device is already locked");

		default:
			return tr("Unknown error: %1").arg(d->lastErr);
	}
}

/*!
   Destructs the QextSerialPort object.
*/
QextSerialPort::~QextSerialPort()
{
	if (isOpen()) {
		close();
	}

	delete d_ptr;
}

/*!
    Sets the flow control used by the port to \a flow.  Possible values of flow are:
    \code
        FLOW_OFF            No flow control
        FLOW_HARDWARE       Hardware (RTS/CTS) flow control
        FLOW_XONXOFF        Software (XON/XOFF) flow control
    \endcode
*/
void QextSerialPort::setFlowControl(FlowType flow)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (d->settings.FlowControl != flow) {
		d->setFlowControl(flow, true);
	}
}

/*!
    Sets the parity associated with the serial port to \a parity.  The possible values of parity are:
    \code
        PAR_SPACE       Space Parity
        PAR_MARK        Mark Parity
        PAR_NONE        No Parity
        PAR_EVEN        Even Parity
        PAR_ODD         Odd Parity
    \endcode
*/
void QextSerialPort::setParity(ParityType parity)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (d->settings.Parity != parity) {
		d->setParity(parity, true);
	}
}

/*!
    Sets the number of data bits used by the serial port to \a dataBits.  Possible values of dataBits are:
    \code
        DATA_5      5 data bits
        DATA_6      6 data bits
        DATA_7      7 data bits
        DATA_8      8 data bits
    \endcode

    \bold note:
    This function is subject to the following restrictions:
    \list
    \o 5 data bits cannot be used with 2 stop bits.
    \o 1.5 stop bits can only be used with 5 data bits.
    \o 8 data bits cannot be used with space parity on POSIX systems.
    \endlist
    */
void QextSerialPort::setDataBits(DataBitsType dataBits)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (d->settings.DataBits != dataBits) {
		d->setDataBits(dataBits, true);
	}
}

/*!
    Sets the number of stop bits used by the serial port to \a stopBits.  Possible values of stopBits are:
    \code
        STOP_1      1 stop bit
        STOP_1_5    1.5 stop bits
        STOP_2      2 stop bits
    \endcode

    \bold note:
    This function is subject to the following restrictions:
    \list
    \o 2 stop bits cannot be used with 5 data bits.
    \o 1.5 stop bits cannot be used with 6 or more data bits.
    \o POSIX does not support 1.5 stop bits.
    \endlist
*/
void QextSerialPort::setStopBits(StopBitsType stopBits)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (d->settings.StopBits != stopBits) {
		d->setStopBits(stopBits, true);
	}
}

/*!
    Sets the baud rate of the serial port to \a baudRate.  Note that not all rates are applicable on
    all platforms.  The following table shows translations of the various baud rate
    constants on Windows(including NT/2000) and POSIX platforms.  Speeds marked with an *
    are speeds that are usable on both Windows and POSIX.
    \code

      RATE          Windows Speed   POSIX Speed
      -----------   -------------   -----------
       BAUD50                   X          50
       BAUD75                   X          75
      *BAUD110                110         110
       BAUD134                  X         134.5
       BAUD150                  X         150
       BAUD200                  X         200
      *BAUD300                300         300
      *BAUD600                600         600
      *BAUD1200              1200        1200
       BAUD1800                 X        1800
      *BAUD2400              2400        2400
      *BAUD4800              4800        4800
      *BAUD9600              9600        9600
       BAUD14400            14400           X
      *BAUD19200            19200       19200
      *BAUD38400            38400       38400
       BAUD56000            56000           X
      *BAUD57600            57600       57600
       BAUD76800                X       76800
      *BAUD115200          115200      115200
       BAUD128000          128000           X
       BAUD230400               X      230400
       BAUD256000          256000           X
       BAUD460800               X      460800
       BAUD500000               X      500000
       BAUD576000               X      576000
       BAUD921600               X      921600
       BAUD1000000              X     1000000
       BAUD1152000              X     1152000
       BAUD1500000              X     1500000
       BAUD2000000              X     2000000
       BAUD2500000              X     2500000
       BAUD3000000              X     3000000
       BAUD3500000              X     3500000
       BAUD4000000              X     4000000
    \endcode
*/

void QextSerialPort::setBaudRate(BaudRateType baudRate)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (d->settings.BaudRate != baudRate) {
		d->setBaudRate(baudRate, true);
	}
}

/*!
    For Unix:

    Sets the read and write timeouts for the port to \a millisec milliseconds.
    Note that this is a per-character timeout, i.e. the port will wait this long for each
    individual character, not for the whole read operation.  This timeout also applies to the
    bytesWaiting() function.

    \bold note:
    POSIX does not support millisecond-level control for I/O timeout values.  Any
    timeout set using this function will be set to the next lowest tenth of a second for
    the purposes of detecting read or write timeouts.  For example a timeout of 550 milliseconds
    will be seen by the class as a timeout of 500 milliseconds for the purposes of reading and
    writing the port.  However millisecond-level control is allowed by the select() system call,
    so for example a 550-millisecond timeout will be seen as 550 milliseconds on POSIX systems for
    the purpose of detecting available bytes in the read buffer.

    For Windows:

    Sets the read and write timeouts for the port to \a millisec milliseconds.
    Setting 0 indicates that timeouts are not used for read nor write operations;
    however read() and write() functions will still block. Set -1 to provide
    non-blocking behaviour (read() and write() will return immediately).

    \bold note: this function does nothing in event driven mode.
*/
void QextSerialPort::setTimeout(long millisec)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (d->settings.Timeout_Millisec != millisec) {
		d->setTimeout(millisec, true);
	}
}

/*!
    Sets DTR line to the requested state (\a set default to high).  This function will have no effect if
    the port associated with the class is not currently open.
*/
void QextSerialPort::setDtr(bool set)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (isOpen()) {
		d->setDtr_sys(set);
	}
}

/*!
    Sets RTS line to the requested state \a set (high by default).
    This function will have no effect if
    the port associated with the class is not currently open.
*/
void QextSerialPort::setRts(bool set)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);

	if (isOpen()) {
		d->setRts_sys(set);
	}
}

/*! \reimp
    Reads a block of data from the serial port.  This function will read at most maxlen bytes from
    the serial port and place them in the buffer pointed to by data.  Return value is the number of
    bytes actually read, or -1 on error.

    \warning before calling this function ensure that serial port associated with this class
    is currently open (use isOpen() function to check if port is open).
*/
qint64 QextSerialPort::readData(char *data, qint64 maxSize)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);
	qint64 bytesFromBuffer = 0;

	if (!d->readBuffer.isEmpty()) {
		bytesFromBuffer = d->readBuffer.read(data, maxSize);

		if (bytesFromBuffer == maxSize) {
			return bytesFromBuffer;
		}
	}

	qint64 bytesFromDevice = d->readData_sys(data + bytesFromBuffer, maxSize - bytesFromBuffer);

	if (bytesFromDevice < 0) {
		return -1;
	}

	return bytesFromBuffer + bytesFromDevice;
}

/*! \reimp
    Writes a block of data to the serial port.  This function will write len bytes
    from the buffer pointed to by data to the serial port.  Return value is the number
    of bytes actually written, or -1 on error.

    \warning before calling this function ensure that serial port associated with this class
    is currently open (use isOpen() function to check if port is open).
*/
qint64 QextSerialPort::writeData(const char *data, qint64 maxSize)
{
	Q_D(QextSerialPort);
	QWriteLocker locker(&d->lock);
	return d->writeData_sys(data, maxSize);
}

#include "moc_qextserialport.cpp"
