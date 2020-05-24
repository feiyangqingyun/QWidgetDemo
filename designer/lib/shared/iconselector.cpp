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

#include "iconselector_p.h"
#include "qdesigner_utils_p.h"
#include "qtresourcemodel_p.h"
#include "qtresourceview_p.h"
#include "iconloader_p.h"
#include "qdesigner_integration_p.h"
#include "formwindowbase_p.h"

#include <abstractdialoggui_p.h>
#include <qdesigner_integration_p.h>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerResourceBrowserInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QToolButton>
#include <QtCore/QSignalMapper>
#include <QtGui/QComboBox>
#include <QtGui/QAction>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QDialog>
#include <QtGui/QMenu>
#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QImageReader>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QVBoxLayout>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// -------------------- LanguageResourceDialogPrivate
class LanguageResourceDialogPrivate {
    LanguageResourceDialog *q_ptr;
    Q_DECLARE_PUBLIC(LanguageResourceDialog)

public:
    LanguageResourceDialogPrivate(QDesignerResourceBrowserInterface *rb);
    void init(LanguageResourceDialog *p);

    void setCurrentPath(const QString &filePath);
    QString currentPath() const;

    void slotAccepted();
    void slotPathChanged(const QString &);

private:
    void setOkButtonEnabled(bool v)         { m_dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(v); }
    static bool checkPath(const QString &p);

    QDesignerResourceBrowserInterface *m_browser;
    QDialogButtonBox *m_dialogButtonBox;
};

LanguageResourceDialogPrivate::LanguageResourceDialogPrivate(QDesignerResourceBrowserInterface *rb) :
    q_ptr(0),
    m_browser(rb),
    m_dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel))
{
     setOkButtonEnabled(false);
}

void LanguageResourceDialogPrivate::init(LanguageResourceDialog *p)
{
    q_ptr = p;
    QLayout *layout = new QVBoxLayout(p);
    layout->addWidget(m_browser);
    layout->addWidget(m_dialogButtonBox);
    QObject::connect(m_dialogButtonBox, SIGNAL(accepted()), p, SLOT(slotAccepted()));
    QObject::connect(m_dialogButtonBox, SIGNAL(rejected()), p, SLOT(reject()));
    QObject::connect(m_browser, SIGNAL(currentPathChanged(QString)), p, SLOT(slotPathChanged(QString)));
    QObject::connect(m_browser, SIGNAL(pathActivated(QString)), p, SLOT(slotAccepted()));
    p->setModal(true);
    p->setWindowTitle(LanguageResourceDialog::tr("Choose Resource"));
    p->setWindowFlags(p->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setOkButtonEnabled(false);
}

void LanguageResourceDialogPrivate::setCurrentPath(const QString &filePath)
{
    m_browser->setCurrentPath(filePath);
    setOkButtonEnabled(checkPath(filePath));
}

QString LanguageResourceDialogPrivate::currentPath() const
{
    return m_browser->currentPath();
}

bool LanguageResourceDialogPrivate::checkPath(const QString &p)
{
    return p.isEmpty() ? false : IconSelector::checkPixmap(p, IconSelector::CheckFast);
}

void LanguageResourceDialogPrivate::slotAccepted()
{
    if (checkPath(currentPath()))
        q_ptr->accept();
}

void LanguageResourceDialogPrivate::slotPathChanged(const QString &p)
{
    setOkButtonEnabled(checkPath(p));
}

// ------------ LanguageResourceDialog
LanguageResourceDialog::LanguageResourceDialog(QDesignerResourceBrowserInterface *rb, QWidget *parent) :
    QDialog(parent),
    d_ptr(new LanguageResourceDialogPrivate(rb))
{
    d_ptr->init( this);
}

LanguageResourceDialog::~LanguageResourceDialog()
{
}

void LanguageResourceDialog::setCurrentPath(const QString &filePath)
{
    d_ptr->setCurrentPath(filePath);
}

QString LanguageResourceDialog::currentPath() const
{
    return d_ptr->currentPath();
}

LanguageResourceDialog* LanguageResourceDialog::create(QDesignerFormEditorInterface *core, QWidget *parent)
{
    if (QDesignerLanguageExtension *lang = qt_extension<QDesignerLanguageExtension *>(core->extensionManager(), core))
        if (QDesignerResourceBrowserInterface *rb = lang->createResourceBrowser(0))
            return new LanguageResourceDialog(rb, parent);
    if (QDesignerIntegration *di = qobject_cast<QDesignerIntegration*>(core->integration()))
        if (QDesignerResourceBrowserInterface *rb = di->createResourceBrowser(0))
            return new LanguageResourceDialog(rb, parent);
    return 0;
}

// ------------ IconSelectorPrivate
class IconSelectorPrivate
{
    IconSelector *q_ptr;
    Q_DECLARE_PUBLIC(IconSelector)
public:
    IconSelectorPrivate();

    void slotStateActivated();
    void slotSetActivated();
    void slotSetResourceActivated();
    void slotSetFileActivated();
    void slotResetActivated();
    void slotResetAllActivated();
    void slotUpdate();

    QList<QPair<QPair<QIcon::Mode, QIcon::State>, QString> > m_stateToName; // could be static map

    QMap<QPair<QIcon::Mode, QIcon::State>, int>  m_stateToIndex;
    QMap<int, QPair<QIcon::Mode, QIcon::State> > m_indexToState;

    QIcon m_emptyIcon;
    QComboBox *m_stateComboBox;
    QToolButton *m_iconButton;
    QAction *m_resetAction;
    QAction *m_resetAllAction;
    PropertySheetIconValue m_icon;
    DesignerIconCache *m_iconCache;
    DesignerPixmapCache *m_pixmapCache;
    QtResourceModel *m_resourceModel;
    QDesignerFormEditorInterface *m_core;
};

IconSelectorPrivate::IconSelectorPrivate() :
    q_ptr(0),
    m_stateComboBox(0),
    m_iconButton(0),
    m_resetAction(0),
    m_resetAllAction(0),
    m_iconCache(0),
    m_pixmapCache(0),
    m_resourceModel(0),
    m_core(0)
{
}
void IconSelectorPrivate::slotUpdate()
{
    QIcon icon;
    if (m_iconCache)
        icon = m_iconCache->icon(m_icon);

    QMap<QPair<QIcon::Mode, QIcon::State>, PropertySheetPixmapValue> paths = m_icon.paths();
    QMapIterator<QPair<QIcon::Mode, QIcon::State>, int> itIndex(m_stateToIndex);
    while (itIndex.hasNext()) {
        const QPair<QIcon::Mode, QIcon::State> state = itIndex.next().key();
        const PropertySheetPixmapValue pixmap = paths.value(state);
        const int index = itIndex.value();

        QIcon pixmapIcon = QIcon(icon.pixmap(16, 16, state.first, state.second));
        if (pixmapIcon.isNull())
            pixmapIcon = m_emptyIcon;
        m_stateComboBox->setItemIcon(index, pixmapIcon);
        QFont font = q_ptr->font();
        if (!pixmap.path().isEmpty())
            font.setBold(true);
        m_stateComboBox->setItemData(index, font, Qt::FontRole);
    }

    QPair<QIcon::Mode, QIcon::State> state = m_indexToState.value(m_stateComboBox->currentIndex());
    PropertySheetPixmapValue currentPixmap = paths.value(state);
    m_resetAction->setEnabled(!currentPixmap.path().isEmpty());
    m_resetAllAction->setEnabled(!paths.isEmpty());
    m_stateComboBox->update();
}

void IconSelectorPrivate::slotStateActivated()
{
    slotUpdate();
}

void IconSelectorPrivate::slotSetActivated()
{
    QPair<QIcon::Mode, QIcon::State> state = m_indexToState.value(m_stateComboBox->currentIndex());
    const PropertySheetPixmapValue pixmap = m_icon.pixmap(state.first, state.second);
    // Default to resource
    const PropertySheetPixmapValue::PixmapSource ps = pixmap.path().isEmpty() ? PropertySheetPixmapValue::ResourcePixmap : pixmap.pixmapSource(m_core);
    switch (ps) {
    case PropertySheetPixmapValue::LanguageResourcePixmap:
    case PropertySheetPixmapValue::ResourcePixmap:
        slotSetResourceActivated();
        break;
    case PropertySheetPixmapValue::FilePixmap:
        slotSetFileActivated();
        break;
    }
}

// Choose a pixmap from resource; use language-dependent resource browser if present
QString IconSelector::choosePixmapResource(QDesignerFormEditorInterface *core, QtResourceModel *resourceModel, const QString &oldPath, QWidget *parent)
{
    Q_UNUSED(resourceModel)
    QString rc;

    if (LanguageResourceDialog* ldlg = LanguageResourceDialog::create(core, parent)) {
        ldlg->setCurrentPath(oldPath);
        if (ldlg->exec() == QDialog::Accepted)
            rc = ldlg->currentPath();
        delete ldlg;
    } else {
        QtResourceViewDialog dlg(core, parent);

        QDesignerIntegration *designerIntegration = qobject_cast<QDesignerIntegration *>(core->integration());
        if (designerIntegration)
            dlg.setResourceEditingEnabled(designerIntegration->isResourceEditingEnabled());

        dlg.selectResource(oldPath);
        if (dlg.exec() == QDialog::Accepted)
            rc = dlg.selectedResource();
    }
    return rc;
}

void IconSelectorPrivate::slotSetResourceActivated()
{
    const QPair<QIcon::Mode, QIcon::State> state = m_indexToState.value(m_stateComboBox->currentIndex());

    PropertySheetPixmapValue pixmap = m_icon.pixmap(state.first, state.second);
    const QString oldPath = pixmap.path();
    const QString newPath = IconSelector::choosePixmapResource(m_core, m_resourceModel, oldPath, q_ptr);
    if (newPath.isEmpty() || newPath == oldPath)
        return;
    const PropertySheetPixmapValue newPixmap = PropertySheetPixmapValue(newPath);
    if (newPixmap != pixmap) {
        m_icon.setPixmap(state.first, state.second, newPixmap);
        slotUpdate();
        emit q_ptr->iconChanged(m_icon);
    }
}

// Helpers for choosing image files: Check for valid image.
bool IconSelector::checkPixmap(const QString &fileName, CheckMode cm, QString *errorMessage)
{
    const QFileInfo fi(fileName);
    if (!fi.exists() || !fi.isFile() || !fi.isReadable()) {
        if (errorMessage)
            *errorMessage = tr("The pixmap file '%1' cannot be read.").arg(fileName);
        return false;
    }
    QImageReader reader(fileName);
    if (!reader.canRead()) {
        if (errorMessage)
            *errorMessage = tr("The file '%1' does not appear to be a valid pixmap file: %2").arg(fileName).arg(reader.errorString());
        return false;
    }
    if (cm == CheckFast)
        return true;

    const QImage image = reader.read();
    if (image.isNull()) {
        if (errorMessage)
            *errorMessage = tr("The file '%1' could not be read: %2").arg(fileName).arg(reader.errorString());
        return false;
    }
    return true;
}

// Helpers for choosing image files: Return an image filter for QFileDialog, courtesy of StyledButton
static QString imageFilter()
{
    QString filter = QApplication::translate("IconSelector", "All Pixmaps (");
    const QList<QByteArray> supportedImageFormats = QImageReader::supportedImageFormats();
    const QString jpeg = QLatin1String("JPEG");
    const int count = supportedImageFormats.count();
    for (int i = 0; i< count; ++i) {
        if (i)
            filter += QLatin1Char(' ');
        filter += QLatin1String("*.");
        const QString outputFormat = QString::fromUtf8(supportedImageFormats.at(i));
        if (outputFormat != jpeg)
            filter += outputFormat.toLower();
        else
            filter += QLatin1String("jpg *.jpeg");
    }
    filter += QLatin1Char(')');
    return filter;
}

// Helpers for choosing image files: Choose a file
QString IconSelector::choosePixmapFile(const QString &directory, QDesignerDialogGuiInterface *dlgGui,QWidget *parent)
{
    QString errorMessage;
    QString newPath;
    do {
        const QString title = tr("Choose a Pixmap");
        static const  QString filter = imageFilter();
        newPath =  dlgGui->getOpenImageFileName(parent, title, directory, filter);
        if (newPath.isEmpty())
            break;
        if (checkPixmap(newPath, CheckFully, &errorMessage))
            break;
        dlgGui->message(parent, QDesignerDialogGuiInterface::ResourceEditorMessage, QMessageBox::Warning, tr("Pixmap Read Error"), errorMessage);
    } while(true);
    return  newPath;
}

void IconSelectorPrivate::slotSetFileActivated()
{
    QPair<QIcon::Mode, QIcon::State> state = m_indexToState.value(m_stateComboBox->currentIndex());

    PropertySheetPixmapValue pixmap = m_icon.pixmap(state.first, state.second);
    const QString newPath = IconSelector::choosePixmapFile(pixmap.path(), m_core->dialogGui(), q_ptr);
    if (!newPath.isEmpty()) {
        const PropertySheetPixmapValue newPixmap = PropertySheetPixmapValue(newPath);
        if (!(newPixmap == pixmap)) {
            m_icon.setPixmap(state.first, state.second, newPixmap);
            slotUpdate();
            emit q_ptr->iconChanged(m_icon);
        }
    }
}

void IconSelectorPrivate::slotResetActivated()
{
    QPair<QIcon::Mode, QIcon::State> state = m_indexToState.value(m_stateComboBox->currentIndex());

    PropertySheetPixmapValue pixmap = m_icon.pixmap(state.first, state.second);
    const PropertySheetPixmapValue newPixmap;
    if (!(newPixmap == pixmap)) {
        m_icon.setPixmap(state.first, state.second, newPixmap);
        slotUpdate();
        emit q_ptr->iconChanged(m_icon);
    }
}

void IconSelectorPrivate::slotResetAllActivated()
{
    const PropertySheetIconValue newIcon;
    if (!(m_icon == newIcon)) {
        m_icon = newIcon;
        slotUpdate();
        emit q_ptr->iconChanged(m_icon);
    }
}

// ------------- IconSelector
IconSelector::IconSelector(QWidget *parent) :
    QWidget(parent), d_ptr(new IconSelectorPrivate())
{
    d_ptr->q_ptr = this;

    d_ptr->m_stateComboBox = new QComboBox(this);

    QHBoxLayout *l = new QHBoxLayout(this);
    d_ptr->m_iconButton = new QToolButton(this);
    d_ptr->m_iconButton->setText(tr("..."));
    d_ptr->m_iconButton->setPopupMode(QToolButton::MenuButtonPopup);
    l->addWidget(d_ptr->m_stateComboBox);
    l->addWidget(d_ptr->m_iconButton);
    l->setMargin(0);

    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Normal,   QIcon::Off), tr("Normal Off")   );
    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Normal,   QIcon::On),  tr("Normal On")    );
    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Disabled, QIcon::Off), tr("Disabled Off") );
    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Disabled, QIcon::On),  tr("Disabled On")  );
    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Active,   QIcon::Off), tr("Active Off")   );
    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Active,   QIcon::On),  tr("Active On")    );
    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Selected, QIcon::Off), tr("Selected Off") );
    d_ptr->m_stateToName << qMakePair(qMakePair(QIcon::Selected, QIcon::On),  tr("Selected On")  );

    QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    d_ptr->m_emptyIcon = QIcon(QPixmap::fromImage(img));

    QMenu *setMenu = new QMenu(this);

    QAction *setResourceAction = new QAction(tr("Choose Resource..."), this);
    QAction *setFileAction = new QAction(tr("Choose File..."), this);
    d_ptr->m_resetAction = new QAction(tr("Reset"), this);
    d_ptr->m_resetAllAction = new QAction(tr("Reset All"), this);
    d_ptr->m_resetAction->setEnabled(false);
    d_ptr->m_resetAllAction->setEnabled(false);
    //d_ptr->m_resetAction->setIcon(createIconSet(QString::fromUtf8("resetproperty.png")));

    setMenu->addAction(setResourceAction);
    setMenu->addAction(setFileAction);
    setMenu->addSeparator();
    setMenu->addAction(d_ptr->m_resetAction);
    setMenu->addAction(d_ptr->m_resetAllAction);

    int index = 0;
    QStringList items;
    QListIterator<QPair<QPair<QIcon::Mode, QIcon::State>, QString> > itName(d_ptr->m_stateToName);
    while (itName.hasNext()) {
        QPair<QPair<QIcon::Mode, QIcon::State>, QString> item = itName.next();
        const QPair<QIcon::Mode, QIcon::State> state = item.first;
        const QString name = item.second;

        items.append(name);
        d_ptr->m_stateToIndex[state] = index;
        d_ptr->m_indexToState[index] = state;
        index++;
    }
    d_ptr->m_stateComboBox->addItems(items);

    d_ptr->m_iconButton->setMenu(setMenu);

    connect(d_ptr->m_stateComboBox, SIGNAL(activated(int)), this, SLOT(slotStateActivated()));
    connect(d_ptr->m_iconButton, SIGNAL(clicked()), this, SLOT(slotSetActivated()));
    connect(setResourceAction, SIGNAL(triggered()), this, SLOT(slotSetResourceActivated()));
    connect(setFileAction, SIGNAL(triggered()), this, SLOT(slotSetFileActivated()));
    connect(d_ptr->m_resetAction, SIGNAL(triggered()), this, SLOT(slotResetActivated()));
    connect(d_ptr->m_resetAllAction, SIGNAL(triggered()), this, SLOT(slotResetAllActivated()));

    d_ptr->slotUpdate();
}

IconSelector::~IconSelector()
{
}

void IconSelector::setIcon(const PropertySheetIconValue &icon)
{
    if (d_ptr->m_icon == icon)
        return;

    d_ptr->m_icon = icon;
    d_ptr->slotUpdate();
}

PropertySheetIconValue IconSelector::icon() const
{
    return d_ptr->m_icon;
}

void IconSelector::setFormEditor(QDesignerFormEditorInterface *core)
{
    d_ptr->m_core = core;
    d_ptr->m_resourceModel = core->resourceModel();
    d_ptr->slotUpdate();
}

void IconSelector::setIconCache(DesignerIconCache *iconCache)
{
    d_ptr->m_iconCache = iconCache;
    connect(iconCache, SIGNAL(reloaded()), this, SLOT(slotUpdate()));
    d_ptr->slotUpdate();
}

void IconSelector::setPixmapCache(DesignerPixmapCache *pixmapCache)
{
    d_ptr->m_pixmapCache = pixmapCache;
    connect(pixmapCache, SIGNAL(reloaded()), this, SLOT(slotUpdate()));
    d_ptr->slotUpdate();
}

} // qdesigner_internal

QT_END_NAMESPACE

#include "moc_iconselector_p.cpp"

