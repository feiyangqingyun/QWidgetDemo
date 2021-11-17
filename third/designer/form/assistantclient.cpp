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

#include "assistantclient.h"

#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

enum { debugAssistantClient = 0 };

AssistantClient::AssistantClient() :
    m_process(0)
{
}

AssistantClient::~AssistantClient()
{
    if (isRunning()) {
        m_process->terminate();
        m_process->waitForFinished();
    }
    delete m_process;
}

bool AssistantClient::showPage(const QString &path, QString *errorMessage)
{
    QString cmd = QLatin1String("SetSource ");
    cmd += path;
    return sendCommand(cmd, errorMessage);
}

bool AssistantClient::activateIdentifier(const QString &identifier, QString *errorMessage)
{
    QString cmd = QLatin1String("ActivateIdentifier ");
    cmd += identifier;
    return sendCommand(cmd, errorMessage);
}

bool AssistantClient::activateKeyword(const QString &keyword, QString *errorMessage)
{
    QString cmd = QLatin1String("ActivateKeyword ");
    cmd += keyword;
    return sendCommand(cmd, errorMessage);
}

bool AssistantClient::sendCommand(const QString &cmd, QString *errorMessage)
{
    if (debugAssistantClient)
        qDebug() << "sendCommand " << cmd;
    if (!ensureRunning(errorMessage))
        return false;
    if (!m_process->isWritable() || m_process->bytesToWrite() > 0) {
        *errorMessage = QCoreApplication::translate("AssistantClient", "Unable to send request: Assistant is not responding.");
        return false;
    }
    QTextStream str(m_process);
    str << cmd << QLatin1Char('\n') << endl;
    return true;
}

bool AssistantClient::isRunning() const
{
    return m_process && m_process->state() != QProcess::NotRunning;
}

QString AssistantClient::binary()
{
    QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
    app += QLatin1String("assistant");
#else
    app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");    
#endif

#if defined(Q_OS_WIN)
    app += QLatin1String(".exe");
#endif

    return app;
}

bool AssistantClient::ensureRunning(QString *errorMessage)
{
    if (isRunning())
        return true;

    if (!m_process)
        m_process = new QProcess;

    const QString app = binary();
    if (!QFileInfo(app).isFile()) {
        *errorMessage = QCoreApplication::translate("AssistantClient", "The binary '%1' does not exist.").arg(app);
        return false;
    }
    if (debugAssistantClient)
        qDebug() << "Running " << app;
    // run
    QStringList args(QLatin1String("-enableRemoteControl"));
    m_process->start(app, args);
    if (!m_process->waitForStarted()) {
        *errorMessage = QCoreApplication::translate("AssistantClient", "Unable to launch assistant (%1).").arg(app);
        return false;
    }
    return true;
}

QString AssistantClient::documentUrl(const QString &prefix, int qtVersion)
{
    if (qtVersion == 0)
        qtVersion = QT_VERSION;
    QString rc;
    QTextStream(&rc) << QLatin1String("qthelp://com.trolltech.") << prefix << QLatin1Char('.')
                     << (qtVersion >> 16) << ((qtVersion >> 8) & 0xFF) << (qtVersion & 0xFF)
                     << QLatin1String("/qdoc/");
    return rc;
}

QString AssistantClient::designerManualUrl(int qtVersion)
{
    return documentUrl(QLatin1String("designer"), qtVersion);
}

QString AssistantClient::qtReferenceManualUrl(int qtVersion)
{
    return documentUrl(QLatin1String("qt"), qtVersion);
}

QT_END_NAMESPACE
