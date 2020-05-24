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

#include "dialoggui_p.h"

#include <QtGui/QFileIconProvider>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPixmap>

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QSet>

// QFileDialog on X11 does not provide an image preview. Display icons.
#ifdef Q_WS_X11
#  define IMAGE_PREVIEW
#endif

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// Icon provider that reads out the known image formats
class IconProvider : public QFileIconProvider {
    Q_DISABLE_COPY(IconProvider)

public:
    IconProvider();
    virtual QIcon icon (const QFileInfo &info) const;

    inline bool loadCheck(const QFileInfo &info) const;
    QImage loadImage(const QString &fileName) const;

private:
    QSet<QString> m_imageFormats;
};

IconProvider::IconProvider()
{
    // Determine a list of readable extensions (upper and lower case)
    typedef QList<QByteArray> ByteArrayList;
    const ByteArrayList fmts = QImageReader::supportedImageFormats();
    const ByteArrayList::const_iterator cend = fmts.constEnd();
    for (ByteArrayList::const_iterator it = fmts.constBegin(); it != cend; ++it) {
        const QString suffix = QString::fromUtf8(it->constData());
        m_imageFormats.insert(suffix.toLower());
        m_imageFormats.insert(suffix.toUpper());

    }
}

// Check by extension and type if this appears to be a loadable image
bool IconProvider::loadCheck(const QFileInfo &info) const
{
    if (info.isFile() && info.isReadable()) {
        const QString suffix = info.suffix();
        if (!suffix.isEmpty())
            return m_imageFormats.contains(suffix);
    }
    return false;
}

QImage IconProvider::loadImage(const QString &fileName) const
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QImageReader imgReader(&file);
        if (imgReader.canRead()) {
            QImage image;
            if (imgReader.read(&image))
                return image;
        }
    }
    return QImage();
}

QIcon IconProvider::icon (const QFileInfo &info) const
{
    // Don't get stuck on large images.
    const qint64 maxSize = 131072;
    if (loadCheck(info) && info.size() < maxSize) {
        const QImage image = loadImage(info.absoluteFilePath());
        if (!image.isNull())
            return QIcon(QPixmap::fromImage(image, Qt::ThresholdDither|Qt::AutoColor));
    }
    return QFileIconProvider::icon(info);
}

// ---------------- DialogGui
DialogGui::DialogGui() :
    m_iconProvider(0)
{
}

DialogGui::~DialogGui()
{
    delete m_iconProvider;
}

QFileIconProvider *DialogGui::ensureIconProvider()
{
    if (!m_iconProvider)
        m_iconProvider = new IconProvider;
    return m_iconProvider;
}

QMessageBox::StandardButton
        DialogGui::message(QWidget *parent, Message /*context*/, QMessageBox::Icon icon,
                           const QString &title, const QString &text, QMessageBox::StandardButtons buttons,
                           QMessageBox::StandardButton defaultButton)
{
    QMessageBox::StandardButton rc = QMessageBox::NoButton;
    switch (icon) {
    case QMessageBox::Information:
        rc = QMessageBox::information(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::Warning:
        rc = QMessageBox::warning(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::Critical:
        rc = QMessageBox::critical(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::Question:
        rc = QMessageBox::question(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::NoIcon:
        break;
    }
    return rc;
}

QMessageBox::StandardButton
        DialogGui::message(QWidget *parent, Message /*context*/, QMessageBox::Icon icon,
                          const QString &title, const QString &text, const QString &informativeText,
                          QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    QMessageBox msgBox(icon, title, text, buttons, parent);
    msgBox.setDefaultButton(defaultButton);
    msgBox.setInformativeText(informativeText);
    return static_cast<QMessageBox::StandardButton>(msgBox.exec());
}

QMessageBox::StandardButton
        DialogGui::message(QWidget *parent, Message /*context*/, QMessageBox::Icon icon,
                          const QString &title, const QString &text, const QString &informativeText, const QString &detailedText,
                          QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    QMessageBox msgBox(icon, title, text, buttons, parent);
    msgBox.setDefaultButton(defaultButton);
    msgBox.setInformativeText(informativeText);
    msgBox.setDetailedText(detailedText);
    return static_cast<QMessageBox::StandardButton>(msgBox.exec());
}

QString DialogGui::getExistingDirectory(QWidget *parent, const QString &caption, const QString &dir, QFileDialog::Options options)
{
    return QFileDialog::getExistingDirectory(parent, caption, dir, options);
}

QString DialogGui::getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
}

QStringList DialogGui::getOpenFileNames(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    return QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
}

QString DialogGui::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    return QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options);
}

void DialogGui::initializeImageFileDialog(QFileDialog &fileDialog, QFileDialog::Options options, QFileDialog::FileMode fm)
{
    fileDialog.setConfirmOverwrite( !(options & QFileDialog::DontConfirmOverwrite) );
    fileDialog.setResolveSymlinks( !(options & QFileDialog::DontResolveSymlinks) );
    fileDialog.setIconProvider(ensureIconProvider());
    fileDialog.setFileMode(fm);
}

QString DialogGui::getOpenImageFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options )
{

#ifdef IMAGE_PREVIEW
    QFileDialog fileDialog(parent, caption, dir, filter);
    initializeImageFileDialog(fileDialog, options, QFileDialog::ExistingFile);
    if (fileDialog.exec() != QDialog::Accepted)
        return QString();

    const QStringList selectedFiles = fileDialog.selectedFiles();
    if (selectedFiles.empty())
        return QString();

    if (selectedFilter)
        *selectedFilter =  fileDialog.selectedFilter();

    return selectedFiles.front();
#else
    return getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
#endif
}

QStringList DialogGui::getOpenImageFileNames(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options )
{
#ifdef IMAGE_PREVIEW
    QFileDialog fileDialog(parent, caption, dir, filter);
    initializeImageFileDialog(fileDialog, options, QFileDialog::ExistingFiles);
    if (fileDialog.exec() != QDialog::Accepted)
        return QStringList();

    const QStringList selectedFiles = fileDialog.selectedFiles();
    if (!selectedFiles.empty() && selectedFilter)
        *selectedFilter =  fileDialog.selectedFilter();

    return selectedFiles;
#else
      return getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
#endif
}

}

QT_END_NAMESPACE
