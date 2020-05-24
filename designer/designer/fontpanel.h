/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Qt tools.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef FONTPANEL_H
#define FONTPANEL_H

#include <QtGui/QGroupBox>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>

QT_BEGIN_NAMESPACE

class QComboBox;
class QFontComboBox;
class QTimer;
class QLineEdit;

class FontPanel: public QGroupBox
{
    Q_OBJECT
public:
    FontPanel(QWidget *parentWidget = 0);

    QFont selectedFont() const;
    void setSelectedFont(const QFont &);

    QFontDatabase::WritingSystem writingSystem() const;
    void setWritingSystem(QFontDatabase::WritingSystem ws);

private slots:
    void slotWritingSystemChanged(int);
    void slotFamilyChanged(const QFont &);
    void slotStyleChanged(int);
    void slotPointSizeChanged(int);
    void slotUpdatePreviewFont();

private:
    QString family() const;
    QString styleString() const;
    int pointSize() const;
    int closestPointSizeIndex(int ps) const;

    void updateWritingSystem(QFontDatabase::WritingSystem ws);
    void updateFamily(const QString &family);
    void updatePointSizes(const QString &family, const QString &style);
    void delayedPreviewFontUpdate();

    QFontDatabase m_fontDatabase;
    QLineEdit *m_previewLineEdit;
    QComboBox *m_writingSystemComboBox;
    QFontComboBox* m_familyComboBox;
    QComboBox *m_styleComboBox;
    QComboBox *m_pointSizeComboBox;
    QTimer *m_previewFontUpdateTimer;
};

QT_END_NAMESPACE

#endif // FONTPANEL_H
