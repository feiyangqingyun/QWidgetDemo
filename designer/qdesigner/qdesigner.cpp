/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

// designer
#include "qdesigner.h"
#include "qdesigner_actions.h"
#include "qdesigner_server.h"
#include "qdesigner_settings.h"
#include "qdesigner_workbench.h"
#include "mainwindow.h"

#include <qdesigner_propertysheet_p.h>

#include <QtGui/QFileOpenEvent>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QIcon>
#include <QtGui/QErrorMessage>
#include <QtCore/QMetaObject>
#include <QtCore/QFile>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtCore/QFileInfo>
#include <QtCore/qdebug.h>

#include <QtDesigner/QDesignerComponents>

QT_BEGIN_NAMESPACE

static const char *designerApplicationName = "Designer";
static const char *designerWarningPrefix = "Designer: ";

static void designerMessageHandler(QtMsgType type, const char *msg)
{
    // Only Designer warnings are displayed as box
    QDesigner *designerApp = qDesigner;
    if (type != QtWarningMsg || !designerApp || qstrncmp(designerWarningPrefix, msg, qstrlen(designerWarningPrefix))) {
        qInstallMsgHandler(0);
        qt_message_output(type, msg);
        qInstallMsgHandler (designerMessageHandler);
        return;
    }
    designerApp->showErrorMessage(msg);
}

QDesigner::QDesigner(int &argc, char **argv)
    : QApplication(argc, argv),
      m_server(0),
      m_client(0),
      m_workbench(0), m_suppressNewFormShow(false)
{
    setOrganizationName(QLatin1String("Trolltech"));
    setApplicationName(QLatin1String(designerApplicationName));
    QDesignerComponents::initializeResources();

    initialize();
}

QDesigner::~QDesigner()
{
    if (m_workbench) {
        delete m_workbench;
    }
    if (m_server) {
        delete m_server;
    }
    if (m_client) {
        delete m_client;
    }
}

void QDesigner::showErrorMessage(const char *message)
{
    // strip the prefix
    const QString qMessage = QString::fromUtf8(message + qstrlen(designerWarningPrefix));
    // If there is no main window yet, just store the message.
    // The QErrorMessage would otherwise be hidden by the main window.
    if (m_mainWindow) {
        showErrorMessageBox(qMessage);
    } else {
        qInstallMsgHandler(0);
        qt_message_output(QtWarningMsg, message); // just in case we crash
        qInstallMsgHandler (designerMessageHandler);
        m_initializationErrors += qMessage;
        m_initializationErrors += QLatin1Char('\n');
    }
}

void QDesigner::showErrorMessageBox(const QString &msg)
{
    // Manually suppress consecutive messages.
    // This happens if for example sth is wrong with custom widget creation.
    // The same warning will be displayed by Widget box D&D and form Drop
    // while trying to create instance.
    if (m_errorMessageDialog && m_lastErrorMessage == msg) {
        return;
    }

    if (!m_errorMessageDialog) {
        m_lastErrorMessage.clear();
        m_errorMessageDialog = new QErrorMessage(m_mainWindow);
        const QString title = QCoreApplication::translate("QDesigner", "%1 - warning").arg(QLatin1String(designerApplicationName));
        m_errorMessageDialog->setWindowTitle(title);
        m_errorMessageDialog->setMinimumSize(QSize(600, 250));
        m_errorMessageDialog->setWindowFlags(m_errorMessageDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }
    m_errorMessageDialog->showMessage(msg);
    m_lastErrorMessage = msg;
}

QDesignerWorkbench *QDesigner::workbench() const
{
    return m_workbench;
}

QDesignerServer *QDesigner::server() const
{
    return m_server;
}

bool QDesigner::parseCommandLineArgs(QStringList &fileNames, QString &resourceDir)
{
    const QStringList args = arguments();
    const QStringList::const_iterator acend = args.constEnd();
    QStringList::const_iterator it = args.constBegin();
    for (++it; it != acend; ++it) {
        const QString &argument = *it;
        do {
            // Arguments
            if (!argument.startsWith(QLatin1Char('-'))) {
                if (!fileNames.contains(argument)) {
                    fileNames.append(argument);
                }
                break;
            }
            // Options
            if (argument == QLatin1String("-server")) {
                m_server = new QDesignerServer();
                printf("%d\n", m_server->serverPort());
                fflush(stdout);
                break;
            }
            if (argument == QLatin1String("-client")) {
                bool ok = true;
                if (++it == acend) {
                    qWarning("** WARNING The option -client requires an argument");
                    return false;
                }
                const quint16 port = it->toUShort(&ok);
                if (ok) {
                    m_client = new QDesignerClient(port, this);
                } else {
                    qWarning("** WARNING Non-numeric argument specified for -client");
                    return false;
                }
                break;
            }
            if (argument == QLatin1String("-resourcedir")) {
                if (++it == acend) {
                    qWarning("** WARNING The option -resourcedir requires an argument");
                    return false;
                }
                resourceDir = QFile::decodeName(it->toLocal8Bit());
                break;
            }
            if (argument == QLatin1String("-enableinternaldynamicproperties")) {
                QDesignerPropertySheet::setInternalDynamicPropertiesEnabled(true);
                break;
            }
            const QString msg = QString::fromUtf8("** WARNING Unknown option %1").arg(argument);
            qWarning("%s", qPrintable(msg));
        } while (false);
    }
    return true;
}

void QDesigner::initialize()
{
    // initialize the sub components
    QStringList files;
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    parseCommandLineArgs(files, resourceDir);

    QTranslator *translator = new QTranslator(this);
    translator->load(":/trolltech/designer/images/designer_zh_CN.qm");
    this->installTranslator(translator);

    if (QLibraryInfo::licensedProducts() == QLatin1String("Console")) {
        QMessageBox::information(0, tr("Qt Designer"),
                                 tr("This application cannot be used for the Console edition of Qt"));
        QMetaObject::invokeMethod(this, "quit", Qt::QueuedConnection);
        return;
    }

    m_workbench = new QDesignerWorkbench();

    emit initialized();
    qInstallMsgHandler(designerMessageHandler); // Warn when loading faulty forms

    m_suppressNewFormShow = m_workbench->readInBackup();

    if (!files.empty()) {
        const QStringList::const_iterator cend = files.constEnd();
        for (QStringList::const_iterator it = files.constBegin(); it != cend; ++it) {
            // Ensure absolute paths for recent file list to be unique
            QString fileName = *it;
            const QFileInfo fi(fileName);
            if (fi.exists() && fi.isRelative()) {
                fileName = fi.absoluteFilePath();
            }
            m_workbench->readInForm(fileName);
        }
    }
    if ( m_workbench->formWindowCount()) {
        m_suppressNewFormShow = true;
    }

    // Show up error box with parent now if something went wrong
    if (m_initializationErrors.isEmpty()) {
        if (!m_suppressNewFormShow && QDesignerSettings(m_workbench->core()).showNewFormOnStartup()) {
            QTimer::singleShot(100, this, SLOT(callCreateForm()));    // won't show anything if suppressed
        }
    } else {
        showErrorMessageBox(m_initializationErrors);
        m_initializationErrors.clear();
    }
}

bool QDesigner::event(QEvent *ev)
{
    bool eaten;
    switch (ev->type()) {
    case QEvent::FileOpen:
        // Set it true first since, if it's a Qt 3 form, the messagebox from convert will fire the timer.
        m_suppressNewFormShow = true;
        if (!m_workbench->readInForm(static_cast<QFileOpenEvent *>(ev)->file())) {
            m_suppressNewFormShow = false;
        }
        eaten = true;
        break;
    case QEvent::Close: {
        QCloseEvent *closeEvent = static_cast<QCloseEvent *>(ev);
        closeEvent->setAccepted(m_workbench->handleClose());
        if (closeEvent->isAccepted()) {
            // We're going down, make sure that we don't get our settings saved twice.
            if (m_mainWindow) {
                m_mainWindow->setCloseEventPolicy(MainWindowBase::AcceptCloseEvents);
            }
            eaten = QApplication::event(ev);
        }
        eaten = true;
        break;
    }
    default:
        eaten = QApplication::event(ev);
        break;
    }
    return eaten;
}

void QDesigner::setMainWindow(MainWindowBase *tw)
{
    m_mainWindow = tw;
}

MainWindowBase *QDesigner::mainWindow() const
{
    return m_mainWindow;
}

void QDesigner::callCreateForm()
{
    if (!m_suppressNewFormShow) {
        QFile file("quc.ui");
        if (file.exists()) {
            m_workbench->actionManager()->readInForm("quc.ui");
        } else {
            m_workbench->actionManager()->createForm();
        }
    }
}

QT_END_NAMESPACE
