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

#include "scriptdialog_p.h"
#include "qscripthighlighter_p.h"

#include <abstractdialoggui_p.h>

#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#ifdef QT_SCRIPT_LIB
#include <QtScript/QScriptEngine>
#endif

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

    // ScriptDialog
    ScriptDialog::ScriptDialog(QDesignerDialogGuiInterface *m_dialogGui, QWidget *parent) :
        QDialog(parent),
        m_dialogGui(m_dialogGui),
        m_textEdit(new QTextEdit)
    {
        setWindowTitle(tr("Edit script"));
        setModal(true);

        QVBoxLayout *vboxLayout = new QVBoxLayout(this);

        const QString textHelp = tr("\
<html>Enter a Qt Script snippet to be executed while loading the form.<br>\
The widget and its children are accessible via the \
variables <i>widget</i> and <i>childWidgets</i>, respectively.");
        m_textEdit->setToolTip(textHelp);
        m_textEdit->setWhatsThis(textHelp);
        m_textEdit->setMinimumSize(QSize(600, 400));
        vboxLayout->addWidget(m_textEdit);
        new QScriptHighlighter(m_textEdit->document());
        // button box
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
        connect(buttonBox , SIGNAL(rejected()), this, SLOT(reject()));
        connect(buttonBox , SIGNAL(accepted()), this, SLOT(slotAccept()));
        vboxLayout->addWidget(buttonBox);
    }

    bool ScriptDialog::editScript(QString &script)
    {
        m_textEdit->setText(script);
        if (exec() != Accepted)
            return false;

        script = trimmedScript();
        return true;
    }

    void ScriptDialog::slotAccept()
    {
        if (checkScript())
            accept();
    }

    QString ScriptDialog::trimmedScript() const
    {
        // Ensure a single newline
        QString rc = m_textEdit->toPlainText().trimmed();
        if (!rc.isEmpty())
            rc += QLatin1Char('\n');
        return rc;
    }

    bool ScriptDialog::checkScript()
    {
        const QString script = trimmedScript();
        if (script.isEmpty())
            return true;
#ifdef QT_SCRIPT_LIB
        QScriptEngine scriptEngine;
        if (scriptEngine.canEvaluate(script))
            return true;
        m_dialogGui->message(this, QDesignerDialogGuiInterface::ScriptDialogMessage, QMessageBox::Warning,
                             windowTitle(), tr("Syntax error"), QMessageBox::Ok);
        return  false;
#else
        return true;
#endif
    }
} // namespace qdesigner_internal

QT_END_NAMESPACE
