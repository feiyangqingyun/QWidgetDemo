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

#include "appfontdialog.h"

#include <iconloader_p.h>
#include <abstractsettings_p.h>

#include <QtGui/QTreeView>
#include <QtGui/QToolButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QFileDialog>
#include <QtGui/QStandardItemModel>
#include <QtGui/QMessageBox>
#include <QtGui/QFontDatabase>
#include <QtGui/QDialogButtonBox>

#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QtAlgorithms>
#include <QtCore/QVector>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

enum {FileNameRole = Qt::UserRole + 1, IdRole =  Qt::UserRole + 2 };
enum { debugAppFontWidget = 0 };

static const char fontFileKeyC[] = "fontFiles";

// AppFontManager: Singleton that maintains the mapping of loaded application font
// ids to the file names (which are not stored in QFontDatabase)
// and provides API for loading/unloading fonts as well for saving/restoring settings.

class AppFontManager
{
    Q_DISABLE_COPY(AppFontManager)
    AppFontManager();
public:
    static AppFontManager &instance();

    void save(QDesignerSettingsInterface *s, const QString &prefix) const;
    void restore(const QDesignerSettingsInterface *s, const QString &prefix);

    // Return id or -1
    int add(const QString &fontFile, QString *errorMessage);

    bool remove(int id, QString *errorMessage);
    bool remove(const QString &fontFile, QString *errorMessage);
    bool removeAt(int index, QString *errorMessage);

    // Store loaded fonts as pair of file name and Id
    typedef QPair<QString,int> FileNameFontIdPair;
    typedef QList<FileNameFontIdPair> FileNameFontIdPairs;
    const FileNameFontIdPairs &fonts() const;

private:
    FileNameFontIdPairs m_fonts;
};

AppFontManager::AppFontManager()
{
}

AppFontManager &AppFontManager::instance()
{
    static AppFontManager rc;
    return rc;
}

void AppFontManager::save(QDesignerSettingsInterface *s, const QString &prefix) const
{
    // Store as list of file names
    QStringList fontFiles;
    const FileNameFontIdPairs::const_iterator cend = m_fonts.constEnd();
    for (FileNameFontIdPairs::const_iterator it = m_fonts.constBegin(); it != cend; ++it)
        fontFiles.push_back(it->first);

    s->beginGroup(prefix);
    s->setValue(QLatin1String(fontFileKeyC),  fontFiles);
    s->endGroup();

    if (debugAppFontWidget)
        qDebug() << "AppFontManager::saved" << fontFiles.size() << "fonts under " << prefix;
}

void AppFontManager::restore(const QDesignerSettingsInterface *s, const QString &prefix)
{
    QString key = prefix;
    key += QLatin1Char('/');
    key += QLatin1String(fontFileKeyC);
    const QStringList fontFiles = s->value(key, QStringList()).toStringList();

    if (debugAppFontWidget)
        qDebug() << "AppFontManager::restoring" << fontFiles.size() << "fonts from " << prefix;
    if (!fontFiles.empty()) {
        QString errorMessage;
        const QStringList::const_iterator cend = fontFiles.constEnd();
        for (QStringList::const_iterator it = fontFiles.constBegin(); it != cend; ++it)
            if (add(*it, &errorMessage) == -1)
                qWarning("%s", qPrintable(errorMessage));
    }
}

int AppFontManager::add(const QString &fontFile, QString *errorMessage)
{
    const QFileInfo inf(fontFile);
    if (!inf.isFile()) {
        *errorMessage = QCoreApplication::translate("AppFontManager", "'%1' is not a file.").arg(fontFile);
        return -1;
    }
    if (!inf.isReadable()) {
        *errorMessage = QCoreApplication::translate("AppFontManager", "The font file '%1' does not have read permissions.").arg(fontFile);
        return -1;
    }
    const QString fullPath = inf.absoluteFilePath();
    // Check if already loaded
    const FileNameFontIdPairs::const_iterator cend = m_fonts.constEnd();
    for (FileNameFontIdPairs::const_iterator it = m_fonts.constBegin(); it != cend; ++it) {
        if (it->first == fullPath) {
            *errorMessage = QCoreApplication::translate("AppFontManager", "The font file '%1' is already loaded.").arg(fontFile);
            return -1;
        }
    }

    const int id = QFontDatabase::addApplicationFont(fullPath);
    if (id == -1) {
        *errorMessage = QCoreApplication::translate("AppFontManager", "The font file '%1' could not be loaded.").arg(fontFile);
        return -1;
    }

    if (debugAppFontWidget)
        qDebug() << "AppFontManager::add" << fontFile << id;
    m_fonts.push_back(FileNameFontIdPair(fullPath, id));
    return id;
}

bool AppFontManager::remove(int id, QString *errorMessage)
{
    const int count = m_fonts.size();
    for (int i = 0; i < count; i++)
        if (m_fonts[i].second == id)
            return removeAt(i, errorMessage);

    *errorMessage = QCoreApplication::translate("AppFontManager", "'%1' is not a valid font id.").arg(id);
    return false;
}

bool AppFontManager::remove(const QString &fontFile, QString *errorMessage)
{
    const int count = m_fonts.size();
    for (int i = 0; i < count; i++)
        if (m_fonts[i].first == fontFile)
            return removeAt(i, errorMessage);

    *errorMessage = QCoreApplication::translate("AppFontManager", "There is no loaded font matching the id '%1'.").arg(fontFile);
    return false;
}

bool AppFontManager::removeAt(int index, QString *errorMessage)
{
    Q_ASSERT(index >= 0 && index < m_fonts.size());

    const QString fontFile = m_fonts[index].first;
    const int id = m_fonts[index].second;

    if (debugAppFontWidget)
        qDebug() << "AppFontManager::removeAt" << index << '(' <<  fontFile << id << ')';

    if (!QFontDatabase::removeApplicationFont(id)) {
        *errorMessage = QCoreApplication::translate("AppFontManager", "The font '%1' (%2) could not be unloaded.").arg(fontFile).arg(id);
        return false;
    }
    m_fonts.removeAt(index);
    return true;
}

const AppFontManager::FileNameFontIdPairs &AppFontManager::fonts() const
{
    return  m_fonts;
}

// ------------- AppFontModel
class AppFontModel : public QStandardItemModel {
    Q_DISABLE_COPY(AppFontModel)
public:
    AppFontModel(QObject *parent = 0);

    void init(const AppFontManager &mgr);
    void add(const QString &fontFile, int id);
    int idAt(const QModelIndex &idx) const;
};

AppFontModel::AppFontModel(QObject * parent) :
    QStandardItemModel(parent)
{
    setHorizontalHeaderLabels(QStringList(AppFontWidget::tr("Fonts")));
}

void AppFontModel::init(const AppFontManager &mgr)
{
    typedef AppFontManager::FileNameFontIdPairs FileNameFontIdPairs;

    const FileNameFontIdPairs &fonts = mgr.fonts();
    const FileNameFontIdPairs::const_iterator cend = fonts.constEnd();
    for (FileNameFontIdPairs::const_iterator it = fonts.constBegin(); it != cend; ++it)
        add(it->first, it->second);
}

void AppFontModel::add(const QString &fontFile, int id)
{
    const QFileInfo inf(fontFile);
    // Root item with base name
    QStandardItem *fileItem = new QStandardItem(inf.completeBaseName());
    const QString fullPath = inf.absoluteFilePath();
    fileItem->setData(fullPath, FileNameRole);
    fileItem->setToolTip(fullPath);
    fileItem->setData(id, IdRole);
    fileItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    appendRow(fileItem);
    const QStringList families = QFontDatabase::applicationFontFamilies(id);
    const QStringList::const_iterator cend = families.constEnd();
    for (QStringList::const_iterator it = families.constBegin(); it != cend; ++it) {
        QStandardItem *familyItem = new QStandardItem(*it);
        familyItem->setToolTip(fullPath);
        familyItem->setFont(QFont(*it));
        familyItem->setFlags(Qt::ItemIsEnabled);
        fileItem->appendRow(familyItem);
    }
}

int AppFontModel::idAt(const QModelIndex &idx) const
{
    if (const QStandardItem *item = itemFromIndex(idx))
        return item->data(IdRole).toInt();
    return -1;
}

// ------------- AppFontWidget
AppFontWidget::AppFontWidget(QWidget *parent) :
    QGroupBox(parent),
    m_view(new QTreeView),
    m_addButton(new QToolButton),
    m_removeButton(new QToolButton),
    m_removeAllButton(new QToolButton),
    m_model(new AppFontModel(this))
{
    m_model->init(AppFontManager::instance());
    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->expandAll();
    connect(m_view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));

    m_addButton->setToolTip(tr("Add font files"));
    m_addButton->setIcon(qdesigner_internal::createIconSet(QString::fromUtf8("plus.png")));
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(addFiles()));

    m_removeButton->setEnabled(false);
    m_removeButton->setToolTip(tr("Remove current font file"));
    m_removeButton->setIcon(qdesigner_internal::createIconSet(QString::fromUtf8("minus.png")));
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveFiles()));

    m_removeAllButton->setToolTip(tr("Remove all font files"));
    m_removeAllButton->setIcon(qdesigner_internal::createIconSet(QString::fromUtf8("editdelete.png")));
    connect(m_removeAllButton, SIGNAL(clicked()), this, SLOT(slotRemoveAll()));

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_addButton);
    hLayout->addWidget(m_removeButton);
    hLayout->addWidget(m_removeAllButton);
    hLayout->addItem(new QSpacerItem(0, 0,QSizePolicy::MinimumExpanding));

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(m_view);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
}

void AppFontWidget::addFiles()
{
    const QStringList files =
        QFileDialog::getOpenFileNames(this, tr("Add Font Files"), QString(),
                                      tr("Font files (*.ttf)"));
    if (files.empty())
        return;

    QString errorMessage;

    AppFontManager &fmgr = AppFontManager::instance();
    const QStringList::const_iterator cend = files.constEnd();
    for (QStringList::const_iterator it = files.constBegin(); it != cend; ++it) {
        const int id = fmgr.add(*it, &errorMessage);
        if (id != -1) {
            m_model->add(*it, id);
        } else {
            QMessageBox::critical(this, tr("Error Adding Fonts"), errorMessage);
        }
    }
    m_view->expandAll();
}

static void removeFonts(const QModelIndexList &selectedIndexes, AppFontModel *model, QWidget *dialogParent)
{
    if (selectedIndexes.empty())
        return;

    // Reverse sort top level rows and remove
    AppFontManager &fmgr = AppFontManager::instance();
    QVector<int> rows;
    rows.reserve(selectedIndexes.size());

    QString errorMessage;
    const QModelIndexList::const_iterator cend = selectedIndexes.constEnd();
    for (QModelIndexList::const_iterator it = selectedIndexes.constBegin(); it != cend; ++it) {
        const int id = model->idAt(*it);
        if (id != -1) {
            if (fmgr.remove(id, &errorMessage)) {
                rows.push_back(it->row());
            } else {
                QMessageBox::critical(dialogParent, AppFontWidget::tr("Error Removing Fonts"), errorMessage);
            }
        }
    }

    qStableSort(rows.begin(), rows.end());
    for (int i = rows.size() - 1; i >= 0; i--)
        model->removeRow(rows[i]);
}

void AppFontWidget::slotRemoveFiles()
{
    removeFonts(m_view->selectionModel()->selectedIndexes(), m_model, this);
}

void AppFontWidget::slotRemoveAll()
{
    const int count = m_model->rowCount();
    if (!count)
        return;

    const QMessageBox::StandardButton answer =
        QMessageBox::question(this, tr("Remove Fonts"), tr("Would you like to remove all fonts?"),
                              QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::No)
        return;

    QModelIndexList topLevels;
    for (int i = 0; i < count; i++)
        topLevels.push_back(m_model->index(i, 0));
    removeFonts(topLevels, m_model, this);
}

void AppFontWidget::selectionChanged(const QItemSelection &selected, const QItemSelection & /*deselected*/)
{
     m_removeButton->setEnabled(!selected.indexes().empty());
}

void AppFontWidget::save(QDesignerSettingsInterface *s, const QString &prefix)
{
    AppFontManager::instance().save(s, prefix);
}

void AppFontWidget::restore(const QDesignerSettingsInterface *s, const QString &prefix)
{
    AppFontManager::instance().restore(s, prefix);
}

// ------------ AppFontDialog
AppFontDialog::AppFontDialog(QWidget *parent) :
    QDialog(parent),
    m_appFontWidget(new AppFontWidget)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Additional Fonts"));
    setModal(false);
    QVBoxLayout *vl = new  QVBoxLayout;
    vl->addWidget(m_appFontWidget);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close);
    QDialog::connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
    vl->addWidget(bb);
    setLayout(vl);
}

QT_END_NAMESPACE
