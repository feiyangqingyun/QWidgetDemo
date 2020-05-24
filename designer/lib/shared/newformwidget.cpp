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

#include "newformwidget_p.h"
#include "ui_newformwidget.h"
#include "qdesigner_formbuilder_p.h"
#include "sheet_delegate_p.h"
#include "widgetdatabase_p.h"
#include "shared_settings_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

enum { profileComboIndexOffset = 1 };
enum { debugNewFormWidget = 0 };

enum NewForm_CustomRole {
    // File name (templates from resources, paths)
    TemplateNameRole = Qt::UserRole + 100,
    // Class name (widgets from Widget data base)
    ClassNameRole = Qt::UserRole + 101
};

static const char *newFormObjectNameC = "Form";

// Create a form name for an arbitrary class. If it is Qt, qtify it,
//  else return "Form".
static QString formName(const QString &className)
{
    if (!className.startsWith(QLatin1Char('Q')))
        return QLatin1String(newFormObjectNameC);
    QString rc = className;
    rc.remove(0, 1);
    return rc;
}

namespace qdesigner_internal {

struct TemplateSize {
    const char *name;
    int width;
    int height;
};

static const struct TemplateSize templateSizes[] =
{
    { QT_TRANSLATE_NOOP("qdesigner_internal::NewFormWidget", "Default size"), 0, 0 },
    { QT_TRANSLATE_NOOP("qdesigner_internal::NewFormWidget", "QVGA portrait (240x320)"), 240, 320  },
    { QT_TRANSLATE_NOOP("qdesigner_internal::NewFormWidget", "QVGA landscape (320x240)"), 320, 240 },
    { QT_TRANSLATE_NOOP("qdesigner_internal::NewFormWidget", "VGA portrait (480x640)"), 480, 640 },
    { QT_TRANSLATE_NOOP("qdesigner_internal::NewFormWidget", "VGA landscape (640x480)"), 640, 480 }
};

/* -------------- NewForm dialog.
 * Designer takes new form templates from:
 * 1) Files located in directories specified in resources
 * 2) Files located in directories specified as user templates
 * 3) XML from container widgets deemed usable for form templates by the widget
 *    database
 * 4) XML from custom container widgets deemed usable for form templates by the
 *    widget database
 *
 * The widget database provides helper functions to obtain lists of names
 * and xml for 3,4.
 *
 * Fixed-size forms for embedded platforms are obtained as follows:
 * 1) If the origin is a file:
 *    - Check if the file exists in the subdirectory "/<width>x<height>/" of
 *      the path (currently the case for the dialog box because the button box
 *      needs to be positioned)
 *    - Scale the form using the QWidgetDatabase::scaleFormTemplate routine.
 * 2) If the origin is XML:
 *     - Scale the form using the QWidgetDatabase::scaleFormTemplate routine.
 *
 * The tree widget item roles indicate which type of entry it is
 * (TemplateNameRole = file name 1,2, ClassNameRole = class name 3,4)
 */

NewFormWidget::NewFormWidget(QDesignerFormEditorInterface *core, QWidget *parentWidget) :
    QDesignerNewFormWidgetInterface(parentWidget),
    m_core(core),
    m_ui(new Ui::NewFormWidget),
    m_currentItem(0),
    m_acceptedItem(0)
{
    typedef QList<qdesigner_internal::DeviceProfile> DeviceProfileList;

    m_ui->setupUi(this);
    m_ui->treeWidget->setItemDelegate(new qdesigner_internal::SheetDelegate(m_ui->treeWidget, this));
    m_ui->treeWidget->header()->hide();
    m_ui->treeWidget->header()->setStretchLastSection(true);
    m_ui->lblPreview->setBackgroundRole(QPalette::Base);
    QDesignerSharedSettings settings(m_core);

    QString uiExtension = QLatin1String("ui");
    QString templatePath = QLatin1String(":/trolltech/designer/templates/forms");

    QDesignerLanguageExtension *lang = qt_extension<QDesignerLanguageExtension *>(core->extensionManager(), core);
    if (lang) {
        templatePath = QLatin1String(":/templates/forms");
        uiExtension = lang->uiExtension();
    }

    // Resource templates
    const QString formTemplate = settings.formTemplate();
    QTreeWidgetItem *selectedItem = 0;
    loadFrom(templatePath, true, uiExtension, formTemplate, selectedItem);
    // Additional template paths
    const QStringList formTemplatePaths = settings.formTemplatePaths();
    const QStringList::const_iterator ftcend = formTemplatePaths.constEnd();
    for (QStringList::const_iterator it = formTemplatePaths.constBegin(); it != ftcend; ++it)
        loadFrom(*it, false, uiExtension, formTemplate, selectedItem);

    // Widgets/custom widgets
    if (!lang) {
        //: New Form Dialog Categories
        loadFrom(tr("Widgets"), qdesigner_internal::WidgetDataBase::formWidgetClasses(core), formTemplate, selectedItem);
        loadFrom(tr("Custom Widgets"), qdesigner_internal::WidgetDataBase::customFormWidgetClasses(core), formTemplate, selectedItem);
    }

    // Still no selection - default to first item
    if (selectedItem == 0 && m_ui->treeWidget->topLevelItemCount() != 0) {
        QTreeWidgetItem *firstTopLevel = m_ui->treeWidget->topLevelItem(0);
        if (firstTopLevel->childCount() > 0)
            selectedItem = firstTopLevel->child(0);
    }

    // Open parent, select and make visible
    if (selectedItem) {
        m_ui->treeWidget->setCurrentItem(selectedItem);
        m_ui->treeWidget->setItemSelected(selectedItem, true);
        m_ui->treeWidget->scrollToItem(selectedItem->parent());
    }
    // Fill profile combo
    m_deviceProfiles = settings.deviceProfiles();
    m_ui->profileComboBox->addItem(tr("None"));
    connect(m_ui->profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDeviceProfileIndexChanged(int)));
    if (m_deviceProfiles.empty()) {
        m_ui->profileComboBox->setEnabled(false);
    } else {
        const DeviceProfileList::const_iterator dcend = m_deviceProfiles.constEnd();
        for (DeviceProfileList::const_iterator it = m_deviceProfiles.constBegin(); it != dcend; ++it)
            m_ui->profileComboBox->addItem(it->name());
        const int ci = settings.currentDeviceProfileIndex();
        if (ci >= 0)
            m_ui->profileComboBox->setCurrentIndex(ci + profileComboIndexOffset);
    }
    // Fill size combo
    const int sizeCount =  sizeof(templateSizes)/ sizeof(TemplateSize);
    for (int i = 0; i < sizeCount; i++) {
        const QSize size = QSize(templateSizes[i].width, templateSizes[i].height);
        m_ui->sizeComboBox->addItem(tr(templateSizes[i].name), size);
    }

    setTemplateSize(settings.newFormSize());

    if (debugNewFormWidget)
        qDebug() << Q_FUNC_INFO << "Leaving";
}

NewFormWidget::~NewFormWidget()
{
    QDesignerSharedSettings settings (m_core);
    settings.setNewFormSize(templateSize());
    // Do not change previously stored item if dialog was rejected
    if (m_acceptedItem)
        settings.setFormTemplate(m_acceptedItem->text(0));
    delete m_ui;
}

void NewFormWidget::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (debugNewFormWidget)
        qDebug() << Q_FUNC_INFO << current;
    if (!current)
        return;

    if (!current->parent()) { // Top level item: Ensure expanded when browsing down
        return;
    }

    m_currentItem = current;

    emit currentTemplateChanged(showCurrentItemPixmap());
}

bool NewFormWidget::showCurrentItemPixmap()
{
    bool rc = false;
    if (m_currentItem) {
        const QPixmap pixmap = formPreviewPixmap(m_currentItem);
        if (pixmap.isNull()) {
            m_ui->lblPreview->setText(tr("Error loading form"));
        } else {
            m_ui->lblPreview->setPixmap(pixmap);
            rc = true;
        }
    }
    return rc;
}

void NewFormWidget::on_treeWidget_itemActivated(QTreeWidgetItem *item)
{
    if (debugNewFormWidget)
        qDebug() << Q_FUNC_INFO << item;

    if (item->data(0, TemplateNameRole).isValid() || item->data(0, ClassNameRole).isValid())
        emit templateActivated();
}

QPixmap  NewFormWidget::formPreviewPixmap(const QTreeWidgetItem *item)
{
    // Cache pixmaps per item/device profile
    const ItemPixmapCacheKey cacheKey(item, profileComboIndex());
    ItemPixmapCache::iterator it = m_itemPixmapCache.find(cacheKey);
    if (it == m_itemPixmapCache.end()) {
        // file or string?
        const QVariant fileName = item->data(0, TemplateNameRole);
        QPixmap rc;
        if (fileName.type() == QVariant::String) {
            rc = formPreviewPixmap(fileName.toString());
        } else {
            const QVariant classNameV = item->data(0, ClassNameRole);
            Q_ASSERT(classNameV.type() == QVariant::String);
            const QString className = classNameV.toString();
            QByteArray data =  qdesigner_internal::WidgetDataBase::formTemplate(m_core, className, formName(className)).toUtf8();
            QBuffer buffer(&data);
            buffer.open(QIODevice::ReadOnly);
            rc = formPreviewPixmap(buffer);
        }
        if (rc.isNull()) // Retry invalid ones
            return rc;
        it = m_itemPixmapCache.insert(cacheKey, rc);
    }
    return it.value();
}

QPixmap NewFormWidget::formPreviewPixmap(const QString &fileName) const
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QFileInfo fi(fileName);
        const QPixmap rc = formPreviewPixmap(f, fi.absolutePath());
        f.close();
        return rc;
    }
    qWarning() << "The file " << fileName << " could not be opened: " << f.errorString();
    return QPixmap();
}

QImage NewFormWidget::grabForm(QDesignerFormEditorInterface *core,
                         QIODevice &file,
                         const QString &workingDir,
                         const qdesigner_internal::DeviceProfile &dp)
{
    qdesigner_internal::NewFormWidgetFormBuilder
        formBuilder(core, qdesigner_internal::QDesignerFormBuilder::DisableScripts, dp);
    if (!workingDir.isEmpty())
        formBuilder.setWorkingDirectory(workingDir);

    QWidget *widget = formBuilder.load(&file, 0);
    if (!widget)
        return QImage();

    const QPixmap pixmap = QPixmap::grabWidget(widget);
    widget->deleteLater();
    return pixmap.toImage();
}

QPixmap NewFormWidget::formPreviewPixmap(QIODevice &file, const QString &workingDir) const
{
    const int margin = 7;
    const int shadow = 7;
    const int previewSize = 256;

    const QImage wimage = grabForm(m_core, file, workingDir,  currentDeviceProfile());
    if (wimage.isNull())
        return QPixmap();
    const QImage image =  wimage.scaled(previewSize - margin * 2, previewSize - margin * 2,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);

    QImage dest(previewSize, previewSize, QImage::Format_ARGB32_Premultiplied);
    dest.fill(0);

    QPainter p(&dest);
    p.drawImage(margin, margin, image);

    p.setPen(QPen(palette().brush(QPalette::WindowText), 0));

    p.drawRect(margin-1, margin-1, image.width() + 1, image.height() + 1);

    const QColor dark(Qt::darkGray);
    const QColor light(Qt::transparent);

    // right shadow
    {
        const QRect rect(margin + image.width() + 1, margin + shadow, shadow, image.height() - shadow + 1);
        QLinearGradient lg(rect.topLeft(), rect.topRight());
        lg.setColorAt(0, dark);
        lg.setColorAt(1, light);
        p.fillRect(rect, lg);
    }

    // bottom shadow
    {
        const QRect rect(margin + shadow, margin + image.height() + 1, image.width() - shadow + 1, shadow);
        QLinearGradient lg(rect.topLeft(), rect.bottomLeft());
        lg.setColorAt(0, dark);
        lg.setColorAt(1, light);
        p.fillRect(rect, lg);
    }

    // bottom/right corner shadow
    {
        const QRect rect(margin + image.width() + 1, margin + image.height() + 1, shadow, shadow);
        QRadialGradient g(rect.topLeft(), shadow);
        g.setColorAt(0, dark);
        g.setColorAt(1, light);
        p.fillRect(rect, g);
    }

    // top/right corner
    {
        const QRect rect(margin + image.width() + 1, margin, shadow, shadow);
        QRadialGradient g(rect.bottomLeft(), shadow);
        g.setColorAt(0, dark);
        g.setColorAt(1, light);
        p.fillRect(rect, g);
    }

    // bottom/left corner
    {
        const QRect rect(margin, margin + image.height() + 1, shadow, shadow);
        QRadialGradient g(rect.topRight(), shadow);
        g.setColorAt(0, dark);
        g.setColorAt(1, light);
        p.fillRect(rect, g);
    }

    p.end();

    return QPixmap::fromImage(dest);
}

void NewFormWidget::loadFrom(const QString &path, bool resourceFile, const QString &uiExtension,
                       const QString &selectedItem, QTreeWidgetItem *&selectedItemFound)
{
    const QDir dir(path);

    if (!dir.exists())
        return;

    // Iterate through the directory and add the templates
    const QFileInfoList list = dir.entryInfoList(QStringList(QLatin1String("*.") + uiExtension),
                                                 QDir::Files);

    if (list.isEmpty())
        return;

    const QChar separator = resourceFile ? QChar(QLatin1Char('/'))
                                         : QDir::separator();
    QTreeWidgetItem *root = new QTreeWidgetItem(m_ui->treeWidget);
    root->setFlags(root->flags() & ~Qt::ItemIsSelectable);
    // Try to get something that is easy to read.
    QString visiblePath = path;
    int index = visiblePath.lastIndexOf(separator);
    if (index != -1) {
        // try to find a second slash, just to be a bit better.
        const int index2 = visiblePath.lastIndexOf(separator, index - 1);
        if (index2 != -1)
            index = index2;
        visiblePath = visiblePath.mid(index + 1);
        visiblePath = QDir::toNativeSeparators(visiblePath);
    }

    const QChar underscore = QLatin1Char('_');
    const QChar blank = QLatin1Char(' ');
    root->setText(0, visiblePath.replace(underscore, blank));
    root->setToolTip(0, path);

    const QFileInfoList::const_iterator lcend = list.constEnd();
    for (QFileInfoList::const_iterator it = list.constBegin(); it != lcend; ++it) {
        if (!it->isFile())
            continue;

        QTreeWidgetItem *item = new QTreeWidgetItem(root);
        const QString text = it->baseName().replace(underscore, blank);
        if (selectedItemFound == 0 && text == selectedItem)
            selectedItemFound = item;
        item->setText(0, text);
        item->setData(0, TemplateNameRole, it->absoluteFilePath());
    }
}

void NewFormWidget::loadFrom(const QString &title, const QStringList &nameList,
                       const QString &selectedItem, QTreeWidgetItem *&selectedItemFound)
{
    if (nameList.empty())
        return;
    QTreeWidgetItem *root = new QTreeWidgetItem(m_ui->treeWidget);
    root->setFlags(root->flags() & ~Qt::ItemIsSelectable);
    root->setText(0, title);
    const QStringList::const_iterator cend = nameList.constEnd();
    for (QStringList::const_iterator it = nameList.constBegin(); it != cend; ++it) {
        const QString text = *it;
        QTreeWidgetItem *item = new QTreeWidgetItem(root);
        item->setText(0, text);
        if (selectedItemFound == 0 && text == selectedItem)
            selectedItemFound = item;
        item->setData(0, ClassNameRole, *it);
    }
}

void NewFormWidget::on_treeWidget_itemPressed(QTreeWidgetItem *item)
{
    if (item && !item->parent())
        m_ui->treeWidget->setItemExpanded(item, !m_ui->treeWidget->isItemExpanded(item));
}

QSize NewFormWidget::templateSize() const
{
    return m_ui->sizeComboBox->itemData(m_ui->sizeComboBox->currentIndex()).toSize();
}

void NewFormWidget::setTemplateSize(const QSize &s)
{
    const int index = s.isNull() ? 0 : m_ui->sizeComboBox->findData(s);
    if (index != -1)
        m_ui->sizeComboBox->setCurrentIndex(index);
}

static QString readAll(const QString &fileName, QString *errorMessage)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        *errorMessage = NewFormWidget::tr("Unable to open the form template file '%1': %2").arg(fileName, file.errorString());
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

QString NewFormWidget::itemToTemplate(const QTreeWidgetItem *item, QString *errorMessage) const
{
    const QSize size = templateSize();
    // file name or string contents?
    const QVariant templateFileName = item->data(0, TemplateNameRole);
    if (templateFileName.type() == QVariant::String) {
        const QString fileName = templateFileName.toString();
        // No fixed size: just open.
        if (size.isNull())
            return readAll(fileName, errorMessage);
        // try to find a file matching the size, like "../640x480/xx.ui"
        const QFileInfo fiBase(fileName);
        QString sizeFileName;
        QTextStream(&sizeFileName) << fiBase.path() << QDir::separator()
                                   << size.width() << QLatin1Char('x') << size.height() << QDir::separator()
                                   << fiBase.fileName();
        if (QFileInfo(sizeFileName).isFile())
            return readAll(sizeFileName, errorMessage);
        // Nothing found, scale via DOM/temporary file
        QString contents = readAll(fileName, errorMessage);
        if (!contents.isEmpty())
            contents = qdesigner_internal::WidgetDataBase::scaleFormTemplate(contents, size, false);
        return contents;
    }
    // Content.
    const QString className = item->data(0, ClassNameRole).toString();
    QString contents = qdesigner_internal::WidgetDataBase::formTemplate(m_core, className, formName(className));
    if (!size.isNull())
        contents = qdesigner_internal::WidgetDataBase::scaleFormTemplate(contents, size, false);
    return contents;
}

void NewFormWidget::slotDeviceProfileIndexChanged(int idx)
{
    // Store index for form windows to take effect and refresh pixmap
    QDesignerSharedSettings settings(m_core);
    settings.setCurrentDeviceProfileIndex(idx - profileComboIndexOffset);
    showCurrentItemPixmap();
}

int NewFormWidget::profileComboIndex() const
{
    return m_ui->profileComboBox->currentIndex();
}

qdesigner_internal::DeviceProfile NewFormWidget::currentDeviceProfile() const
{
    const int ci = profileComboIndex();
    if (ci > 0)
        return m_deviceProfiles.at(ci - profileComboIndexOffset);
    return qdesigner_internal::DeviceProfile();
}

bool NewFormWidget::hasCurrentTemplate() const
{
    return m_currentItem != 0;
}

QString NewFormWidget::currentTemplateI(QString *ptrToErrorMessage)
{
    if (m_currentItem == 0) {
        *ptrToErrorMessage = tr("Internal error: No template selected.");
        return QString();
    }
    const QString contents = itemToTemplate(m_currentItem, ptrToErrorMessage);
    if (contents.isEmpty())
        return contents;

    m_acceptedItem = m_currentItem;
    return contents;
}

QString NewFormWidget::currentTemplate(QString *ptrToErrorMessage)
{
    if (ptrToErrorMessage)
        return currentTemplateI(ptrToErrorMessage);
    // Do not loose the error
    QString errorMessage;
    const QString contents = currentTemplateI(&errorMessage);
    if (!errorMessage.isEmpty())
        qWarning("%s", errorMessage.toUtf8().constData());
    return contents;
}

}

QT_END_NAMESPACE
