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

#ifndef ASSISTANTCLIENT_H
#define ASSISTANTCLIENT_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QProcess;
class QString;

class AssistantClient
{
    AssistantClient(const AssistantClient &);
    AssistantClient &operator=(const AssistantClient &);

public:
    AssistantClient();
    ~AssistantClient();

    bool showPage(const QString &path, QString *errorMessage);
    bool activateIdentifier(const QString &identifier, QString *errorMessage);
    bool activateKeyword(const QString &keyword, QString *errorMessage);

    bool isRunning() const;

    static QString documentUrl(const QString &prefix, int qtVersion = 0);
    // Root of the Qt Designer documentation
    static QString designerManualUrl(int qtVersion = 0);
    // Root of the Qt Reference documentation
    static QString qtReferenceManualUrl(int qtVersion = 0);

private:
    static QString binary();
    bool sendCommand(const QString &cmd, QString *errorMessage);
    bool ensureRunning(QString *errorMessage);

    QProcess *m_process;
};

QT_END_NAMESPACE

#endif // ASSISTANTCLIENT_H
