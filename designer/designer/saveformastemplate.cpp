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

#include "saveformastemplate.h"
#include "qdesigner_settings.h"

#include <QtCore/QFile>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

#include <QtDesigner/abstractformeditor.h>
#include <QtDesigner/abstractformwindow.h>

QT_BEGIN_NAMESPACE

SaveFormAsTemplate::SaveFormAsTemplate(QDesignerFormEditorInterface *core,
                                       QDesignerFormWindowInterface *formWindow,
                                       QWidget *parent)
    : QDialog(parent, Qt::Sheet),
      m_core(core),
      m_formWindow(formWindow)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.templateNameEdit->setText(formWindow->mainContainer()->objectName());
    ui.templateNameEdit->selectAll();

    ui.templateNameEdit->setFocus();

    QStringList paths = QDesignerSettings(m_core).formTemplatePaths();
    ui.categoryCombo->addItems(paths);
    ui.categoryCombo->addItem(tr("Add path..."));
    m_addPathIndex = ui.categoryCombo->count() - 1;
    connect(ui.templateNameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateOKButton(QString)));
    connect(ui.categoryCombo, SIGNAL(activated(int)), this, SLOT(checkToAddPath(int)));
}

SaveFormAsTemplate::~SaveFormAsTemplate()
{
}

void SaveFormAsTemplate::accept()
{
    QString templateFileName = ui.categoryCombo->currentText();
    templateFileName += QLatin1Char('/');
    const QString name = ui.templateNameEdit->text();
    templateFileName +=  name;
    const QString extension = QLatin1String(".ui");
    if (!templateFileName.endsWith(extension))
        templateFileName.append(extension);
    QFile file(templateFileName);

    if (file.exists()) {
        QMessageBox msgBox(QMessageBox::Information, tr("Template Exists"),
                        tr("A template with the name %1 already exists.\n"
                           "Do you want overwrite the template?").arg(name), QMessageBox::Cancel, m_formWindow);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        QPushButton *overwriteButton = msgBox.addButton(tr("Overwrite Template"), QMessageBox::AcceptRole);
        msgBox.exec();
        if (msgBox.clickedButton() != overwriteButton)
            return;
    }

    while (!file.open(QFile::WriteOnly)) {
        if (QMessageBox::information(m_formWindow, tr("Open Error"),
            tr("There was an error opening template %1 for writing. Reason: %2").arg(name).arg(file.errorString()),
            QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel) {
            return;
        }
    }

    const QString origName = m_formWindow->fileName();
    // ensure m_formWindow->contents() will convert properly resource paths to relative paths
    // (relative to template location, not to the current form location)
    m_formWindow->setFileName(templateFileName);
    QByteArray ba = m_formWindow->contents().toUtf8();
    m_formWindow->setFileName(origName);
    while (file.write(ba) != ba.size()) {
        if (QMessageBox::information(m_formWindow, tr("Write Error"),
            tr("There was an error writing the template %1 to disk. Reason: %2").arg(name).arg(file.errorString()),
            QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel) {
                file.close();
                file.remove();
                return;
            }
            file.reset();
    }
    // update the list of places too...
    QStringList sl;
    for (int i = 0; i < m_addPathIndex; ++i)
        sl << ui.categoryCombo->itemText(i);

    QDesignerSettings(m_core).setFormTemplatePaths(sl);

    QDialog::accept();
}

void SaveFormAsTemplate::updateOKButton(const QString &str)
{
    QPushButton *okButton = ui.buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(!str.isEmpty());
}

QString  SaveFormAsTemplate::chooseTemplatePath(QWidget *parent)
{
    QString rc = QFileDialog::getExistingDirectory(parent,
                                                   tr("Pick a directory to save templates in"));
    if (rc.isEmpty())
        return rc;

    if (rc.endsWith(QDir::separator()))
        rc.remove(rc.size() - 1, 1);
    return rc;
}

void SaveFormAsTemplate::checkToAddPath(int itemIndex)
{
    if (itemIndex != m_addPathIndex)
        return;

    const QString dir = chooseTemplatePath(this);
    if (dir.isEmpty()) {
        ui.categoryCombo->setCurrentIndex(0);
        return;
    }

    ui.categoryCombo->insertItem(m_addPathIndex, dir);
    ui.categoryCombo->setCurrentIndex(m_addPathIndex);
    ++m_addPathIndex;
}

QT_END_NAMESPACE
