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

#include "abstractsettings_p.h"
#include "abstractformeditor.h"
#include "qtresourceeditordialog_p.h"
#include "ui_qtresourceeditordialog.h"
#include "qtresourcemodel_p.h"
#include "iconloader_p.h"

#include <abstractdialoggui_p.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtXml/QDomDocument>
#include <QtGui/QMenu>
#include <QtGui/QHeaderView>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QStandardItemModel>

QT_BEGIN_NAMESPACE

static const char *rccRootTag = "RCC";
static const char *rccTag = "qresource";
static const char *rccFileTag = "file";
static const char *rccAliasAttribute = "alias";
static const char *rccPrefixAttribute = "prefix";
static const char *rccLangAttribute = "lang";
static const char *SplitterPosition = "SplitterPosition";
static const char *Geometry = "Geometry";
static const char *QrcDialogC = "QrcDialog";

static QString msgOverwrite(const QString &fname)
{
    return QApplication::translate("QtResourceEditorDialog", "%1 already exists.\nDo you want to replace it?", 0, QApplication::UnicodeUTF8).arg(fname);
}

static QString msgTagMismatch(const QString &got, const QString &expected)
{
    return QApplication::translate("QtResourceEditorDialog", "The file does not appear to be a resource file; element '%1' was found where '%2' was expected.").arg(got).arg(expected);
}

namespace {

// below 3 data classes should be derived from QSharedData and made implicit shared class
struct QtResourceFileData {
    QString path;
    QString alias;
    bool operator==(const QtResourceFileData &other) const {
        if (path == other.path && alias == other.alias)
            return true;
        return false;
    }
};

struct QtResourcePrefixData {
    QString prefix;
    QString language;
    QList<QtResourceFileData> resourceFileList;
    bool operator==(const QtResourcePrefixData &other) const {
        if (prefix == other.prefix && language == other.language && resourceFileList == other.resourceFileList)
            return true;
        return false;
    }
};

struct QtQrcFileData {
    QString qrcPath;
    QList<QtResourcePrefixData> resourceList;
    bool operator==(const QtQrcFileData &other) const {
        if (qrcPath == other.qrcPath && resourceList == other.resourceList)
            return true;
        return false;
    }
};

bool loadResourceFileData(const QDomElement &fileElem, QtResourceFileData *fileData, QString *errorMessage)
{
    if (!fileData)
        return false;

    if (fileElem.tagName() != QLatin1String(rccFileTag)) {
        *errorMessage = msgTagMismatch(fileElem.tagName(), QLatin1String(rccFileTag));
        return false;
    }

    QtResourceFileData &data = *fileData;

    data.path = fileElem.text();
    data.alias = fileElem.attribute(QLatin1String(rccAliasAttribute));

    return true;
}

static bool loadResourcePrefixData(const QDomElement &prefixElem, QtResourcePrefixData *prefixData, QString *errorMessage)
{
    if (!prefixData)
        return false;

    if (prefixElem.tagName() != QLatin1String(rccTag)) {
        *errorMessage = msgTagMismatch(prefixElem.tagName(), QLatin1String(rccTag));
        return false;
    }

    QtResourcePrefixData &data = *prefixData;

    data.prefix = prefixElem.attribute(QLatin1String(rccPrefixAttribute));
    data.language = prefixElem.attribute(QLatin1String(rccLangAttribute));
    QDomElement fileElem = prefixElem.firstChildElement();
    while (!fileElem.isNull()) {
        QtResourceFileData fileData;
        if (!loadResourceFileData(fileElem, &fileData, errorMessage))
            return false;
        data.resourceFileList.append(fileData);
        fileElem = fileElem.nextSiblingElement();
    }
    return true;
}

static bool loadQrcFileData(const QDomDocument &doc, const QString &path, QtQrcFileData *qrcFileData, QString *errorMessage)
{
    if (!qrcFileData)
        return false;

    QtQrcFileData &data = *qrcFileData;

    QDomElement docElem = doc.documentElement();
    if (docElem.tagName() != QLatin1String(rccRootTag)) {
        *errorMessage = msgTagMismatch(docElem.tagName(), QLatin1String(rccRootTag));
        return false;
    }

    QDomElement prefixElem = docElem.firstChildElement();
    while (!prefixElem.isNull()) {
        QtResourcePrefixData prefixData;
        if (!loadResourcePrefixData(prefixElem, &prefixData, errorMessage))
            return false;
        data.resourceList.append(prefixData);
        prefixElem = prefixElem.nextSiblingElement();
    }

    data.qrcPath = path;

    return true;
}

QDomElement saveResourceFileData(QDomDocument &doc, const QtResourceFileData &fileData)
{
    QDomElement fileElem = doc.createElement(QLatin1String(rccFileTag));
    if (!fileData.alias.isEmpty())
        fileElem.setAttribute(QLatin1String(rccAliasAttribute), fileData.alias);

    QDomText textElem = doc.createTextNode(fileData.path);
    fileElem.appendChild(textElem);

    return fileElem;
}

QDomElement saveResourcePrefixData(QDomDocument &doc, const QtResourcePrefixData &prefixData)
{
    QDomElement prefixElem = doc.createElement(QLatin1String(rccTag));
    if (!prefixData.prefix.isEmpty())
        prefixElem.setAttribute(QLatin1String(rccPrefixAttribute), prefixData.prefix);
    if (!prefixData.language.isEmpty())
        prefixElem.setAttribute(QLatin1String(rccLangAttribute), prefixData.language);

    QListIterator<QtResourceFileData> itFile(prefixData.resourceFileList);
    while (itFile.hasNext()) {
        QDomElement fileElem = saveResourceFileData(doc, itFile.next());
        prefixElem.appendChild(fileElem);
    }

    return prefixElem;
}

QDomDocument saveQrcFileData(const QtQrcFileData &qrcFileData)
{
    QDomDocument doc;
    QDomElement docElem = doc.createElement(QLatin1String(rccRootTag));
    QListIterator<QtResourcePrefixData> itPrefix(qrcFileData.resourceList);
    while (itPrefix.hasNext()) {
        QDomElement prefixElem = saveResourcePrefixData(doc, itPrefix.next());

        docElem.appendChild(prefixElem);
    }
    doc.appendChild(docElem);

    return doc;
}
// --------------- QtResourceFile
class QtResourceFile {
public:
    friend class QtQrcManager;

    QString path() const { return m_path; }
    QString alias() const { return m_alias; }
    QString fullPath() const { return m_fullPath; }
private:
    QtResourceFile() {}

    QString m_path;
    QString m_alias;
    QString m_fullPath;
};

class QtResourcePrefix {
public:
    friend class QtQrcManager;

    QString prefix() const { return m_prefix; }
    QString language() const { return m_language; }
    QList<QtResourceFile *> resourceFiles() const { return m_resourceFiles; }
private:
    QtResourcePrefix() {}

    QString m_prefix;
    QString m_language;
    QList<QtResourceFile *> m_resourceFiles;

};
// ------------------- QtQrcFile
class QtQrcFile {
public:
    friend class QtQrcManager;

    QString path() const { return m_path; }
    QString fileName() const { return m_fileName; }
    QList<QtResourcePrefix *> resourcePrefixList() const { return m_resourcePrefixes; }
    QtQrcFileData initialState() const { return m_initialState; }

private:
    QtQrcFile() { }

    void setPath(const QString &path) {
        m_path = path;
        QFileInfo fi(path);
        m_fileName = fi.fileName();
    }

    QString m_path;
    QString m_fileName;
    QList<QtResourcePrefix *> m_resourcePrefixes;
    QtQrcFileData m_initialState;
};

// ------------------ QtQrcManager
class QtQrcManager : public QObject
{
    Q_OBJECT
public:
    QtQrcManager(QObject *parent = 0);
    ~QtQrcManager();

    QList<QtQrcFile *> qrcFiles() const;

    // helpers
    QtQrcFile *qrcFileOf(const QString &path) const;
    QtQrcFile *qrcFileOf(QtResourcePrefix *resourcePrefix) const;
    QtQrcFile *qrcFileOf(QtResourceFile *resourceFile) const;
    QtResourcePrefix *resourcePrefixOf(QtResourceFile *resourceFile) const;

    QtQrcFile *importQrcFile(const QtQrcFileData &qrcFileData, QtQrcFile *beforeQrcFile = 0);
    void exportQrcFile(QtQrcFile *qrcFile, QtQrcFileData *qrcFileData) const;

    QList<QtResourceFile *> resourceFilesOf(const QString &resourceFullPath) const;
    QIcon icon(const QString &resourceFullPath) const;
    bool exists(const QString &resourceFullPath) const;
    bool exists(QtQrcFile *qrcFile) const;

    QtQrcFile *prevQrcFile(QtQrcFile *qrcFile) const;
    QtQrcFile *nextQrcFile(QtQrcFile *qrcFile) const;
    QtResourcePrefix *prevResourcePrefix(QtResourcePrefix *resourcePrefix) const;
    QtResourcePrefix *nextResourcePrefix(QtResourcePrefix *resourcePrefix) const;
    QtResourceFile *prevResourceFile(QtResourceFile *resourceFile) const;
    QtResourceFile *nextResourceFile(QtResourceFile *resourceFile) const;

    void clear();

public slots:

    QtQrcFile *insertQrcFile(const QString &path, QtQrcFile *beforeQrcFile = 0, bool newFile = false);
    void moveQrcFile(QtQrcFile *qrcFile, QtQrcFile *beforeQrcFile);
    void setInitialState(QtQrcFile *qrcFile, const QtQrcFileData &initialState);
    void removeQrcFile(QtQrcFile *qrcFile);

    QtResourcePrefix *insertResourcePrefix(QtQrcFile *qrcFile, const QString &prefix,
                    const QString &language, QtResourcePrefix *beforeResourcePrefix = 0);
    void moveResourcePrefix(QtResourcePrefix *resourcePrefix, QtResourcePrefix *beforeResourcePrefix); // the same qrc file???
    void changeResourcePrefix(QtResourcePrefix *resourcePrefix, const QString &newPrefix);
    void changeResourceLanguage(QtResourcePrefix *resourcePrefix, const QString &newLanguage);
    void removeResourcePrefix(QtResourcePrefix *resourcePrefix);

    QtResourceFile *insertResourceFile(QtResourcePrefix *resourcePrefix, const QString &path,
                    const QString &alias, QtResourceFile *beforeResourceFile = 0);
    void moveResourceFile(QtResourceFile *resourceFile, QtResourceFile *beforeResourceFile); // the same prefix???
    void changeResourceAlias(QtResourceFile *resourceFile, const QString &newAlias);
    void removeResourceFile(QtResourceFile *resourceFile);

signals:
    void qrcFileInserted(QtQrcFile *qrcFile);
    void qrcFileMoved(QtQrcFile *qrcFile, QtQrcFile *oldBeforeQrcFile);
    void qrcFileRemoved(QtQrcFile *qrcFile);

    void resourcePrefixInserted(QtResourcePrefix *resourcePrefix);
    void resourcePrefixMoved(QtResourcePrefix *resourcePrefix, QtResourcePrefix *oldBeforeResourcePrefix);
    void resourcePrefixChanged(QtResourcePrefix *resourcePrefix, const QString &oldPrefix);
    void resourceLanguageChanged(QtResourcePrefix *resourcePrefix, const QString &oldLanguage);
    void resourcePrefixRemoved(QtResourcePrefix *resourcePrefix);

    void resourceFileInserted(QtResourceFile *resourceFile);
    void resourceFileMoved(QtResourceFile *resourceFile, QtResourceFile *oldBeforeResourceFile);
    void resourceAliasChanged(QtResourceFile *resourceFile, const QString &oldAlias);
    void resourceFileRemoved(QtResourceFile *resourceFile);
private:

    QList<QtQrcFile *> m_qrcFiles;
    QMap<QString, QtQrcFile *> m_pathToQrc;
    QMap<QtQrcFile *, bool> m_qrcFileToExists;
    QMap<QtResourcePrefix *, QtQrcFile *> m_prefixToQrc;
    QMap<QtResourceFile *, QtResourcePrefix *> m_fileToPrefix;
    QMap<QString, QList<QtResourceFile *> > m_fullPathToResourceFiles;
    QMap<QString, QIcon> m_fullPathToIcon;
    QMap<QString, bool> m_fullPathToExists;
};

QtQrcManager::QtQrcManager(QObject *parent)
    : QObject(parent)
{

}

QtQrcManager::~QtQrcManager()
{
    clear();
}

QList<QtQrcFile *> QtQrcManager::qrcFiles() const
{
    return m_qrcFiles;
}

QtQrcFile *QtQrcManager::qrcFileOf(const QString &path) const
{
    return m_pathToQrc.value(path);
}

QtQrcFile *QtQrcManager::qrcFileOf(QtResourcePrefix *resourcePrefix) const
{
    return m_prefixToQrc.value(resourcePrefix);
}

QtQrcFile *QtQrcManager::qrcFileOf(QtResourceFile *resourceFile) const
{
    return qrcFileOf(resourcePrefixOf(resourceFile));
}

QtResourcePrefix *QtQrcManager::resourcePrefixOf(QtResourceFile *resourceFile) const
{
    return m_fileToPrefix.value(resourceFile);
}

QtQrcFile *QtQrcManager::importQrcFile(const QtQrcFileData &qrcFileData, QtQrcFile *beforeQrcFile)
{
    QtQrcFile *qrcFile = insertQrcFile(qrcFileData.qrcPath, beforeQrcFile);
    if (!qrcFile)
        return 0;
    QListIterator<QtResourcePrefixData> itPrefix(qrcFileData.resourceList);
    while (itPrefix.hasNext()) {
        const QtResourcePrefixData &prefixData = itPrefix.next();
        QtResourcePrefix *resourcePrefix = insertResourcePrefix(qrcFile, prefixData.prefix, prefixData.language, 0);
        QListIterator<QtResourceFileData> itFile(prefixData.resourceFileList);
        while (itFile.hasNext()) {
            const QtResourceFileData &fileData = itFile.next();
            insertResourceFile(resourcePrefix, fileData.path, fileData.alias, 0);
        }
    }
    setInitialState(qrcFile, qrcFileData);
    return qrcFile;
}

void QtQrcManager::exportQrcFile(QtQrcFile *qrcFile, QtQrcFileData *qrcFileData) const
{
    if (!qrcFileData)
        return;

    if (!qrcFile)
        return;

    QtQrcFileData &data = *qrcFileData;

    QList<QtResourcePrefixData> resourceList;

    QList<QtResourcePrefix *> resourcePrefixes = qrcFile->resourcePrefixList();
    QListIterator<QtResourcePrefix *> itPrefix(resourcePrefixes);
    while (itPrefix.hasNext()) {
        QList<QtResourceFileData> resourceFileList;

        QtResourcePrefix *prefix = itPrefix.next();

        QList<QtResourceFile *> resourceFiles = prefix->resourceFiles();
        QListIterator<QtResourceFile *> itFile(resourceFiles);
        while (itFile.hasNext()) {
            QtResourceFile *file = itFile.next();
            QtResourceFileData fileData;
            fileData.path = file->path();
            fileData.alias = file->alias();
            resourceFileList << fileData;
        }
        QtResourcePrefixData prefixData;
        prefixData.prefix = prefix->prefix();
        prefixData.language = prefix->language();
        prefixData.resourceFileList = resourceFileList;

        resourceList << prefixData;
    }
    data = QtQrcFileData();
    data.qrcPath = qrcFile->path();
    data.resourceList = resourceList;
}

QList<QtResourceFile *> QtQrcManager::resourceFilesOf(const QString &resourcePath) const
{
    return m_fullPathToResourceFiles.value(resourcePath);
}

QIcon QtQrcManager::icon(const QString &resourceFullPath) const
{
    return m_fullPathToIcon.value(resourceFullPath);
}

bool QtQrcManager::exists(const QString &resourceFullPath) const
{
    return m_fullPathToExists.value(resourceFullPath, false);
}

bool QtQrcManager::exists(QtQrcFile *qrcFile) const
{
    return m_qrcFileToExists.value(qrcFile, false);
}

QtQrcFile *QtQrcManager::prevQrcFile(QtQrcFile *qrcFile) const
{
    if (!qrcFile)
        return 0;
    const int idx = m_qrcFiles.indexOf(qrcFile);
    if (idx <= 0)
        return 0;
    return m_qrcFiles.at(idx - 1);
}

QtQrcFile *QtQrcManager::nextQrcFile(QtQrcFile *qrcFile) const
{
    if (!qrcFile)
        return 0;
    const int idx = m_qrcFiles.indexOf(qrcFile);
    if (idx < 0 || idx == m_qrcFiles.size() - 1)
        return 0;
    return m_qrcFiles.at(idx + 1);
}

QtResourcePrefix *QtQrcManager::prevResourcePrefix(QtResourcePrefix *resourcePrefix) const
{
    if (!resourcePrefix)
        return 0;
    QList<QtResourcePrefix *> prefixes = qrcFileOf(resourcePrefix)->resourcePrefixList();
    const int idx = prefixes.indexOf(resourcePrefix);
    if (idx <= 0)
        return 0;
    return prefixes.at(idx - 1);
}

QtResourcePrefix *QtQrcManager::nextResourcePrefix(QtResourcePrefix *resourcePrefix) const
{
    if (!resourcePrefix)
        return 0;
    QList<QtResourcePrefix *> prefixes = qrcFileOf(resourcePrefix)->resourcePrefixList();
    const int idx = prefixes.indexOf(resourcePrefix);
    if (idx < 0 || idx == prefixes.size() - 1)
        return 0;
    return prefixes.at(idx + 1);
}

QtResourceFile *QtQrcManager::prevResourceFile(QtResourceFile *resourceFile) const
{
    if (!resourceFile)
        return 0;
    QList<QtResourceFile *> files = resourcePrefixOf(resourceFile)->resourceFiles();
    const int idx = files.indexOf(resourceFile);
    if (idx <= 0)
        return 0;
    return files.at(idx - 1);
}

QtResourceFile *QtQrcManager::nextResourceFile(QtResourceFile *resourceFile) const
{
    if (!resourceFile)
        return 0;
    QList<QtResourceFile *> files = resourcePrefixOf(resourceFile)->resourceFiles();
    const int idx = files.indexOf(resourceFile);
    if (idx < 0 || idx == files.size() - 1)
        return 0;
    return files.at(idx + 1);
}

void QtQrcManager::clear()
{
    QList<QtQrcFile *> oldQrcFiles = qrcFiles();
    QListIterator<QtQrcFile *> it(oldQrcFiles);
    while (it.hasNext())
        removeQrcFile(it.next());
}

QtQrcFile *QtQrcManager::insertQrcFile(const QString &path, QtQrcFile *beforeQrcFile, bool newFile)
{
    if (m_pathToQrc.contains(path))
        return 0;

    int idx = m_qrcFiles.indexOf(beforeQrcFile);
    if (idx < 0)
        idx = m_qrcFiles.size();

    QtQrcFile *qrcFile = new QtQrcFile();
    qrcFile->setPath(path);

    m_qrcFiles.insert(idx, qrcFile);
    m_pathToQrc[path] = qrcFile;

    const QFileInfo fi(path);
    m_qrcFileToExists[qrcFile] = fi.exists() || newFile;

    emit qrcFileInserted(qrcFile);
    return qrcFile;
}

void QtQrcManager::moveQrcFile(QtQrcFile *qrcFile, QtQrcFile *beforeQrcFile)
{
    if (qrcFile == beforeQrcFile)
        return;

    const int idx = m_qrcFiles.indexOf(qrcFile);
    if (idx < 0)
        return;

    int beforeIdx = m_qrcFiles.indexOf(beforeQrcFile);
    if (beforeIdx < 0)
        beforeIdx = m_qrcFiles.size();

    if (idx == beforeIdx - 1) // the same position, nothing changes
        return;

    QtQrcFile *oldBefore = 0;
    if (idx < m_qrcFiles.size() - 1)
        oldBefore = m_qrcFiles.at(idx + 1);

    m_qrcFiles.removeAt(idx);
    if (idx < beforeIdx)
        beforeIdx -= 1;

    m_qrcFiles.insert(beforeIdx, qrcFile);

    emit qrcFileMoved(qrcFile, oldBefore);
}

void QtQrcManager::setInitialState(QtQrcFile *qrcFile, const QtQrcFileData &initialState)
{
    qrcFile->m_initialState = initialState;
}

void QtQrcManager::removeQrcFile(QtQrcFile *qrcFile)
{
    const int idx = m_qrcFiles.indexOf(qrcFile);
    if (idx < 0)
        return;

    QList<QtResourcePrefix *> resourcePrefixes = qrcFile->resourcePrefixList();
    QListIterator<QtResourcePrefix *> it(resourcePrefixes);
    while (it.hasNext())
        removeResourcePrefix(it.next());

    emit qrcFileRemoved(qrcFile);

    m_qrcFiles.removeAt(idx);
    m_pathToQrc.remove(qrcFile->path());
    m_qrcFileToExists.remove(qrcFile);
    delete qrcFile;
}

QtResourcePrefix *QtQrcManager::insertResourcePrefix(QtQrcFile *qrcFile, const QString &prefix,
        const QString &language, QtResourcePrefix *beforeResourcePrefix)
{
    if (!qrcFile)
        return 0;

    int idx = qrcFile->m_resourcePrefixes.indexOf(beforeResourcePrefix);
    if (idx < 0)
        idx = qrcFile->m_resourcePrefixes.size();

    QtResourcePrefix *resourcePrefix = new QtResourcePrefix();
    resourcePrefix->m_prefix = prefix;
    resourcePrefix->m_language = language;

    qrcFile->m_resourcePrefixes.insert(idx, resourcePrefix);
    m_prefixToQrc[resourcePrefix] = qrcFile;

    emit resourcePrefixInserted(resourcePrefix);
    return resourcePrefix;
}

void QtQrcManager::moveResourcePrefix(QtResourcePrefix *resourcePrefix, QtResourcePrefix *beforeResourcePrefix)
{
    if (resourcePrefix == beforeResourcePrefix)
        return;

    QtQrcFile *qrcFile = qrcFileOf(resourcePrefix);
    if (!qrcFile)
        return;

    if (beforeResourcePrefix && qrcFileOf(beforeResourcePrefix) != qrcFile)
        return;

    const int idx = qrcFile->m_resourcePrefixes.indexOf(resourcePrefix);

    int beforeIdx = qrcFile->m_resourcePrefixes.indexOf(beforeResourcePrefix);
    if (beforeIdx < 0)
        beforeIdx = qrcFile->m_resourcePrefixes.size();

    if (idx == beforeIdx - 1) // the same position, nothing changes
        return;

    QtResourcePrefix *oldBefore = 0;
    if (idx < qrcFile->m_resourcePrefixes.size() - 1)
        oldBefore = qrcFile->m_resourcePrefixes.at(idx + 1);

    qrcFile->m_resourcePrefixes.removeAt(idx);
    if (idx < beforeIdx)
        beforeIdx -= 1;

    qrcFile->m_resourcePrefixes.insert(beforeIdx, resourcePrefix);

    emit resourcePrefixMoved(resourcePrefix, oldBefore);
}

void QtQrcManager::changeResourcePrefix(QtResourcePrefix *resourcePrefix, const QString &newPrefix)
{
    if (!resourcePrefix)
        return;

    const QString oldPrefix = resourcePrefix->m_prefix;
    if (oldPrefix == newPrefix)
        return;

    resourcePrefix->m_prefix = newPrefix;

    emit resourcePrefixChanged(resourcePrefix, oldPrefix);
}

void QtQrcManager::changeResourceLanguage(QtResourcePrefix *resourcePrefix, const QString &newLanguage)
{
    if (!resourcePrefix)
        return;

    const QString oldLanguage = resourcePrefix->m_language;
    if (oldLanguage == newLanguage)
        return;

    resourcePrefix->m_language = newLanguage;

    emit resourceLanguageChanged(resourcePrefix, oldLanguage);
}

void QtQrcManager::removeResourcePrefix(QtResourcePrefix *resourcePrefix)
{
    QtQrcFile *qrcFile = qrcFileOf(resourcePrefix);
    if (!qrcFile)
        return;

    const int idx = qrcFile->m_resourcePrefixes.indexOf(resourcePrefix);

    QList<QtResourceFile *> resourceFiles = resourcePrefix->resourceFiles();
    QListIterator<QtResourceFile *> it(resourceFiles);
    while (it.hasNext())
        removeResourceFile(it.next());

    emit resourcePrefixRemoved(resourcePrefix);

    qrcFile->m_resourcePrefixes.removeAt(idx);
    m_prefixToQrc.remove(resourcePrefix);
    delete resourcePrefix;
}

QtResourceFile *QtQrcManager::insertResourceFile(QtResourcePrefix *resourcePrefix, const QString &path,
        const QString &alias, QtResourceFile *beforeResourceFile)
{
    if (!resourcePrefix)
        return 0;

    int idx = resourcePrefix->m_resourceFiles.indexOf(beforeResourceFile);
    if (idx < 0)
        idx = resourcePrefix->m_resourceFiles.size();

    QtResourceFile *resourceFile = new QtResourceFile();
    resourceFile->m_path = path;
    resourceFile->m_alias = alias;
    const QFileInfo fi(qrcFileOf(resourcePrefix)->path());
    const QDir dir(fi.absolutePath());
    const QString fullPath = dir.absoluteFilePath(path);
    resourceFile->m_fullPath = fullPath;

    resourcePrefix->m_resourceFiles.insert(idx, resourceFile);
    m_fileToPrefix[resourceFile] = resourcePrefix;
    m_fullPathToResourceFiles[fullPath].append(resourceFile);
    if (!m_fullPathToIcon.contains(fullPath)) {
        m_fullPathToIcon[fullPath] = QIcon(fullPath);
        const QFileInfo fullInfo(fullPath);
        m_fullPathToExists[fullPath] = fullInfo.exists();
    }

    emit resourceFileInserted(resourceFile);
    return resourceFile;
}

void QtQrcManager::moveResourceFile(QtResourceFile *resourceFile, QtResourceFile *beforeResourceFile)
{
    if (resourceFile == beforeResourceFile)
        return;

    QtResourcePrefix *resourcePrefix = resourcePrefixOf(resourceFile);
    if (!resourcePrefix)
        return;

    if (beforeResourceFile && resourcePrefixOf(beforeResourceFile) != resourcePrefix)
        return;

    const int idx = resourcePrefix->m_resourceFiles.indexOf(resourceFile);

    int beforeIdx = resourcePrefix->m_resourceFiles.indexOf(beforeResourceFile);
    if (beforeIdx < 0)
        beforeIdx = resourcePrefix->m_resourceFiles.size();

    if (idx == beforeIdx - 1) // the same position, nothing changes
        return;

    QtResourceFile *oldBefore = 0;
    if (idx < resourcePrefix->m_resourceFiles.size() - 1)
        oldBefore = resourcePrefix->m_resourceFiles.at(idx + 1);

    resourcePrefix->m_resourceFiles.removeAt(idx);
    if (idx < beforeIdx)
        beforeIdx -= 1;

    resourcePrefix->m_resourceFiles.insert(beforeIdx, resourceFile);

    emit resourceFileMoved(resourceFile, oldBefore);
}

void QtQrcManager::changeResourceAlias(QtResourceFile *resourceFile, const QString &newAlias)
{
    if (!resourceFile)
        return;

    const QString oldAlias = resourceFile->m_alias;
    if (oldAlias == newAlias)
        return;

    resourceFile->m_alias = newAlias;

    emit resourceAliasChanged(resourceFile, oldAlias);
}

void QtQrcManager::removeResourceFile(QtResourceFile *resourceFile)
{
    QtResourcePrefix *resourcePrefix = resourcePrefixOf(resourceFile);
    if (!resourcePrefix)
        return;

    const int idx = resourcePrefix->m_resourceFiles.indexOf(resourceFile);

    emit resourceFileRemoved(resourceFile);

    resourcePrefix->m_resourceFiles.removeAt(idx);
    m_fileToPrefix.remove(resourceFile);
    const QString fullPath = resourceFile->fullPath();
    m_fullPathToResourceFiles[fullPath].removeAll(resourceFile); // optimize me
    if (m_fullPathToResourceFiles[fullPath].isEmpty()) {
        m_fullPathToResourceFiles.remove(fullPath);
        m_fullPathToIcon.remove(fullPath);
        m_fullPathToExists.remove(fullPath);
    }
    delete resourceFile;
}



}

// ----------------- QtResourceEditorDialogPrivate
class QtResourceEditorDialogPrivate
{
    QtResourceEditorDialog *q_ptr;
    Q_DECLARE_PUBLIC(QtResourceEditorDialog)
public:
    QtResourceEditorDialogPrivate();

    void slotQrcFileInserted(QtQrcFile *qrcFile);
    void slotQrcFileMoved(QtQrcFile *qrcFile);
    void slotQrcFileRemoved(QtQrcFile *qrcFile);

    QStandardItem *insertResourcePrefix(QtResourcePrefix *resourcePrefix);

    void slotResourcePrefixInserted(QtResourcePrefix *resourcePrefix) { insertResourcePrefix(resourcePrefix); }
    void slotResourcePrefixMoved(QtResourcePrefix *resourcePrefix);
    void slotResourcePrefixChanged(QtResourcePrefix *resourcePrefix);
    void slotResourceLanguageChanged(QtResourcePrefix *resourcePrefix);
    void slotResourcePrefixRemoved(QtResourcePrefix *resourcePrefix);
    void slotResourceFileInserted(QtResourceFile *resourceFile);
    void slotResourceFileMoved(QtResourceFile *resourceFile);
    void slotResourceAliasChanged(QtResourceFile *resourceFile);
    void slotResourceFileRemoved(QtResourceFile *resourceFile);

    void slotCurrentQrcFileChanged(QListWidgetItem *item);
    void slotCurrentTreeViewItemChanged(const QModelIndex &index);
    void slotListWidgetContextMenuRequested(const QPoint &pos);
    void slotTreeViewContextMenuRequested(const QPoint &pos);
    void slotTreeViewItemChanged(QStandardItem *item);

    void slotNewQrcFile();
    void slotImportQrcFile();
    void slotRemoveQrcFile();
    void slotMoveUpQrcFile();
    void slotMoveDownQrcFile();

    void slotNewPrefix();
    void slotAddFiles();
    void slotChangePrefix();
    void slotChangeLanguage();
    void slotChangeAlias();
    void slotClonePrefix();
    void slotRemove();
    void slotMoveUp();
    void slotMoveDown();

    bool loadQrcFile(const QString &path, QtQrcFileData *qrcFileData, QString *errorMessage);
    bool loadQrcFile(const QString &path, QtQrcFileData *qrcFileData);
    bool saveQrcFile(const QtQrcFileData &qrcFileData);

    QString qrcFileText(QtQrcFile *qrcFile) const;

    QMessageBox::StandardButton warning(const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                        QMessageBox::StandardButton defaultButton = QMessageBox::NoButton) const;

    QString browseForNewLocation(const QString &resourceFile, const QDir &rootDir) const;
    QString copyResourceFile(const QString &resourceFile, const QString &destPath) const;
    QtResourceFile *getCurrentResourceFile() const;
    QtResourcePrefix *getCurrentResourcePrefix() const;
    void selectTreeRow(QStandardItem *item);
    QString getSaveFileNameWithExtension(QWidget *parent,
            const QString &title, QString dir, const QString &filter, const QString &extension) const;
    QString qrcStartDirectory() const;

    Ui::QtResourceEditorDialog m_ui;
    QDesignerFormEditorInterface *m_core;
    QtResourceModel *m_resourceModel;
    QDesignerDialogGuiInterface *m_dlgGui;
    QtQrcManager *m_qrcManager;
    QList<QtQrcFileData> m_initialState;

    QMap<QtQrcFile *, QListWidgetItem *> m_qrcFileToItem;
    QMap<QListWidgetItem *, QtQrcFile *> m_itemToQrcFile;
    QMap<QtResourcePrefix *, QStandardItem *> m_resourcePrefixToPrefixItem;
    QMap<QtResourcePrefix *, QStandardItem *> m_resourcePrefixToLanguageItem;
    QMap<QStandardItem *, QtResourcePrefix *> m_prefixItemToResourcePrefix;
    QMap<QStandardItem *, QtResourcePrefix *> m_languageItemToResourcePrefix;
    QMap<QtResourceFile *, QStandardItem *> m_resourceFileToPathItem;
    QMap<QtResourceFile *, QStandardItem *> m_resourceFileToAliasItem;
    QMap<QStandardItem *, QtResourceFile *> m_pathItemToResourceFile;
    QMap<QStandardItem *, QtResourceFile *> m_aliasItemToResourceFile;

    bool m_ignoreCurrentChanged;
    bool m_firstQrcFileDialog;
    QtQrcFile *m_currentQrcFile;

    QAction *m_newQrcFileAction;
    QAction *m_importQrcFileAction;
    QAction *m_removeQrcFileAction;
    QAction *m_moveUpQrcFileAction;
    QAction *m_moveDownQrcFileAction;

    QAction *m_newPrefixAction;
    QAction *m_addResourceFileAction;
    QAction *m_changePrefixAction;
    QAction *m_changeLanguageAction;
    QAction *m_changeAliasAction;
    QAction *m_clonePrefixAction;
    QAction *m_moveUpAction;
    QAction *m_moveDownAction;
    QAction *m_removeAction;

    QStandardItemModel *m_treeModel;
    QItemSelectionModel *m_treeSelection;
};

QtResourceEditorDialogPrivate::QtResourceEditorDialogPrivate() :
    q_ptr(0),
    m_core(0),
    m_resourceModel(0),
    m_dlgGui(0),
    m_qrcManager(0),
    m_ignoreCurrentChanged(false),
    m_firstQrcFileDialog(true),
    m_currentQrcFile(0),
    m_newQrcFileAction(0),
    m_importQrcFileAction(0),
    m_removeQrcFileAction(0),
    m_moveUpQrcFileAction(0),
    m_moveDownQrcFileAction(0),
    m_newPrefixAction(0),
    m_addResourceFileAction(0),
    m_changePrefixAction(0),
    m_changeLanguageAction(0),
    m_changeAliasAction(0),
    m_clonePrefixAction(0),
    m_moveUpAction(0),
    m_moveDownAction(0),
    m_removeAction(0),
    m_treeModel(0),
    m_treeSelection(0)
{
}

QMessageBox::StandardButton QtResourceEditorDialogPrivate::warning(const QString &title, const QString &text, QMessageBox::StandardButtons buttons,
                                                                   QMessageBox::StandardButton defaultButton) const
{
    return m_dlgGui->message(q_ptr, QDesignerDialogGuiInterface::ResourceEditorMessage, QMessageBox::Warning, title, text, buttons, defaultButton);
}

QString QtResourceEditorDialogPrivate::qrcFileText(QtQrcFile *qrcFile) const
{
    const QString path = qrcFile->path();
    const QString fileName = qrcFile->fileName();
    const QFileInfo fi(path);
    if (fi.exists() && !fi.isWritable())
        return QApplication::translate("QtResourceEditorDialog", "%1 [read-only]").arg(fileName);
    if (!m_qrcManager->exists(qrcFile))
        return QApplication::translate("QtResourceEditorDialog", "%1 [missing]").arg(fileName);
    return fileName;
}

void QtResourceEditorDialogPrivate::slotQrcFileInserted(QtQrcFile *qrcFile)
{
    QListWidgetItem *currentItem = m_ui.qrcFileList->currentItem();
    int idx = m_ui.qrcFileList->count();
    QtQrcFile *nextQrcFile = m_qrcManager->nextQrcFile(qrcFile);
    QListWidgetItem *nextItem = m_qrcFileToItem.value(nextQrcFile);
    if (nextItem) {
        const int row = m_ui.qrcFileList->row(nextItem);
        if (row >= 0)
            idx = row;
    }
    const QString path = qrcFile->path();
    QListWidgetItem *item = new QListWidgetItem(qrcFileText(qrcFile));
    item->setToolTip(path);
    m_ignoreCurrentChanged = true;
    m_ui.qrcFileList->insertItem(idx, item);
    m_ui.qrcFileList->setCurrentItem(currentItem);
    m_ignoreCurrentChanged = false;
    m_qrcFileToItem[qrcFile] = item;
    m_itemToQrcFile[item] = qrcFile;
    if (!m_qrcManager->exists(qrcFile))
        item->setForeground(QBrush(Qt::red));
}

void QtResourceEditorDialogPrivate::slotQrcFileMoved(QtQrcFile *qrcFile)
{
    QListWidgetItem *currentItem = m_ui.qrcFileList->currentItem();
    QListWidgetItem *item = m_qrcFileToItem.value(qrcFile);
    m_ignoreCurrentChanged = true;
    m_ui.qrcFileList->takeItem(m_ui.qrcFileList->row(item));

    int idx = m_ui.qrcFileList->count();
    QtQrcFile *nextQrcFile = m_qrcManager->nextQrcFile(qrcFile);
    QListWidgetItem *nextItem = m_qrcFileToItem.value(nextQrcFile);
    if (nextItem) {
        int row = m_ui.qrcFileList->row(nextItem);
        if (row >= 0)
            idx = row;
    }
    m_ui.qrcFileList->insertItem(idx, item);
    if (currentItem == item)
        m_ui.qrcFileList->setCurrentItem(item);
    m_ignoreCurrentChanged = false;
}

void QtResourceEditorDialogPrivate::slotQrcFileRemoved(QtQrcFile *qrcFile)
{
    QListWidgetItem *item = m_qrcFileToItem.value(qrcFile);
    if (item == m_ui.qrcFileList->currentItem())
        m_ui.qrcFileList->setCurrentItem(0); // this should trigger list view signal currentItemChanged(0), and slot should set m_currentQrcFile to 0
    m_ignoreCurrentChanged = true;
    delete item;
    m_ignoreCurrentChanged = false;
    m_itemToQrcFile.remove(item);
    m_qrcFileToItem.remove(qrcFile);
}

QStandardItem *QtResourceEditorDialogPrivate::insertResourcePrefix(QtResourcePrefix *resourcePrefix)
{
    if (m_qrcManager->qrcFileOf(resourcePrefix) != m_currentQrcFile)
        return 0;

    QtResourcePrefix *prevResourcePrefix = m_qrcManager->prevResourcePrefix(resourcePrefix);
    QStandardItem *prevItem = m_resourcePrefixToPrefixItem.value(prevResourcePrefix);

    int row = 0;
    if (prevItem)
        row = m_treeModel->indexFromItem(prevItem).row() + 1;

    QStandardItem *prefixItem = new QStandardItem();
    QStandardItem *languageItem = new QStandardItem();
    QList<QStandardItem *> items;
    items << prefixItem;
    items << languageItem;
    m_treeModel->insertRow(row, items);
    const QModelIndex newIndex = m_treeModel->indexFromItem(prefixItem);
    m_ui.resourceTreeView->setExpanded(newIndex, true);
    prefixItem->setFlags(prefixItem->flags() | Qt::ItemIsEditable);
    languageItem->setFlags(languageItem->flags() | Qt::ItemIsEditable);
    m_resourcePrefixToPrefixItem[resourcePrefix] = prefixItem;
    m_resourcePrefixToLanguageItem[resourcePrefix] = languageItem;
    m_prefixItemToResourcePrefix[prefixItem] = resourcePrefix;
    m_languageItemToResourcePrefix[languageItem] = resourcePrefix;
    slotResourcePrefixChanged(resourcePrefix);
    slotResourceLanguageChanged(resourcePrefix);
    return prefixItem;
}

void QtResourceEditorDialogPrivate::slotResourcePrefixMoved(QtResourcePrefix *resourcePrefix)
{
    QStandardItem *prefixItem = m_resourcePrefixToPrefixItem.value(resourcePrefix);
    if (!prefixItem)
        return;

    QStandardItem *languageItem = m_resourcePrefixToLanguageItem.value(resourcePrefix);
    if (!languageItem)
        return;

    const QModelIndex index = m_treeModel->indexFromItem(prefixItem);
    const bool expanded = m_ui.resourceTreeView->isExpanded(index);
    m_ignoreCurrentChanged = true;
    const QList<QStandardItem *> items = m_treeModel->takeRow(index.row());

    int row = m_treeModel->rowCount();
    QtResourcePrefix *nextResourcePrefix = m_qrcManager->nextResourcePrefix(resourcePrefix);
    QStandardItem *nextItem = m_resourcePrefixToPrefixItem.value(nextResourcePrefix);
    if (nextItem)
        row = m_treeModel->indexFromItem(nextItem).row();
    m_treeModel->insertRow(row, items);
    m_ignoreCurrentChanged = false;
    m_ui.resourceTreeView->setExpanded(m_treeModel->indexFromItem(items.at(0)), expanded);
}

void QtResourceEditorDialogPrivate::slotResourcePrefixChanged(QtResourcePrefix *resourcePrefix)
{
    QStandardItem *item = m_resourcePrefixToPrefixItem.value(resourcePrefix);
    if (!item)
        return;

    m_ignoreCurrentChanged = true;
    QString prefix = resourcePrefix->prefix();
    if (prefix.isEmpty())
        prefix = QApplication::translate("QtResourceEditorDialog", "<no prefix>", 0, QApplication::UnicodeUTF8);
    item->setText(prefix);
    item->setToolTip(prefix);
    m_ignoreCurrentChanged = false;
}

void QtResourceEditorDialogPrivate::slotResourceLanguageChanged(QtResourcePrefix *resourcePrefix)
{
    QStandardItem *item = m_resourcePrefixToLanguageItem.value(resourcePrefix);
    if (!item)
        return;

    m_ignoreCurrentChanged = true;
    const QString language = resourcePrefix->language();
    item->setText(language);
    item->setToolTip(language);

    m_ignoreCurrentChanged = false;
}

void QtResourceEditorDialogPrivate::slotResourcePrefixRemoved(QtResourcePrefix *resourcePrefix)
{
    QStandardItem *prefixItem = m_resourcePrefixToPrefixItem.value(resourcePrefix);
    if (!prefixItem)
        return;

    QStandardItem *languageItem = m_resourcePrefixToLanguageItem.value(resourcePrefix);
    if (!languageItem)
        return;

    m_ignoreCurrentChanged = true;
    m_treeModel->takeRow(m_treeModel->indexFromItem(prefixItem).row());
    delete prefixItem;
    delete languageItem;
    m_ignoreCurrentChanged = false;
    m_prefixItemToResourcePrefix.remove(prefixItem);
    m_languageItemToResourcePrefix.remove(languageItem);
    m_resourcePrefixToPrefixItem.remove(resourcePrefix);
    m_resourcePrefixToLanguageItem.remove(resourcePrefix);
}

void QtResourceEditorDialogPrivate::slotResourceFileInserted(QtResourceFile *resourceFile)
{
    QtResourcePrefix *resourcePrefix = m_qrcManager->resourcePrefixOf(resourceFile);
    if (m_qrcManager->qrcFileOf(resourcePrefix) != m_currentQrcFile)
        return;

    QtResourceFile *prevResourceFile = m_qrcManager->prevResourceFile(resourceFile);
    QStandardItem *prevItem = m_resourceFileToPathItem.value(prevResourceFile);

    QStandardItem *pathItem = new QStandardItem(resourceFile->path());
    QStandardItem *aliasItem = new QStandardItem();
    QStandardItem *parentItem = m_resourcePrefixToPrefixItem.value(resourcePrefix);
    QList<QStandardItem *> items;
    items << pathItem;
    items << aliasItem;

    int row = 0;
    if (prevItem)
        row = m_treeModel->indexFromItem(prevItem).row() + 1;

    parentItem->insertRow(row, items);

    pathItem->setFlags(pathItem->flags() & ~Qt::ItemIsEditable);
    aliasItem->setFlags(aliasItem->flags() | Qt::ItemIsEditable);
    m_resourceFileToPathItem[resourceFile] = pathItem;
    m_resourceFileToAliasItem[resourceFile] = aliasItem;
    m_pathItemToResourceFile[pathItem] = resourceFile;
    m_aliasItemToResourceFile[aliasItem] = resourceFile;
    pathItem->setToolTip(resourceFile->path());
    pathItem->setIcon(m_qrcManager->icon(resourceFile->fullPath()));
    if (!m_qrcManager->exists(resourceFile->fullPath())) {
        pathItem->setText(QApplication::translate("QtResourceEditorDialog", "%1 [missing]").arg(resourceFile->path()));
        QBrush redBrush(Qt::red);
        pathItem->setForeground(redBrush);
        aliasItem->setForeground(redBrush);
    }
    slotResourceAliasChanged(resourceFile);
}

void QtResourceEditorDialogPrivate::slotResourceFileMoved(QtResourceFile *resourceFile)
{
    QStandardItem *pathItem = m_resourceFileToPathItem.value(resourceFile);
    if (!pathItem)
        return;

    QStandardItem *aliasItem = m_resourceFileToAliasItem.value(resourceFile);
    if (!aliasItem)
        return;

    QStandardItem *parentItem = pathItem->parent();
    m_ignoreCurrentChanged = true;
    const QList<QStandardItem *> items = parentItem->takeRow(m_treeModel->indexFromItem(pathItem).row());

    int row = parentItem->rowCount();
    QtResourceFile *nextResourceFile = m_qrcManager->nextResourceFile(resourceFile);
    QStandardItem *nextItem = m_resourceFileToPathItem.value(nextResourceFile);
    if (nextItem)
        row = m_treeModel->indexFromItem(nextItem).row();
    parentItem->insertRow(row, items);
    m_ignoreCurrentChanged = false;
}

void QtResourceEditorDialogPrivate::slotResourceAliasChanged(QtResourceFile *resourceFile)
{
    QStandardItem *item = m_resourceFileToAliasItem.value(resourceFile);
    if (!item)
        return;

    m_ignoreCurrentChanged = true;
    const QString alias = resourceFile->alias();
    item->setText(alias);
    item->setToolTip(alias);

    m_ignoreCurrentChanged = false;
}

void QtResourceEditorDialogPrivate::slotResourceFileRemoved(QtResourceFile *resourceFile)
{
    QStandardItem *pathItem = m_resourceFileToPathItem.value(resourceFile);
    if (!pathItem)
        return;

    QStandardItem *aliasItem = m_resourceFileToAliasItem.value(resourceFile);
    if (!aliasItem)
        return;

    QStandardItem *parentItem = pathItem->parent();

    m_ignoreCurrentChanged = true;
    parentItem->takeRow(m_treeModel->indexFromItem(pathItem).row());
    delete pathItem;
    delete aliasItem;
    m_ignoreCurrentChanged = false;
    m_pathItemToResourceFile.remove(pathItem);
    m_aliasItemToResourceFile.remove(aliasItem);
    m_resourceFileToPathItem.remove(resourceFile);
    m_resourceFileToAliasItem.remove(resourceFile);
}


void QtResourceEditorDialogPrivate::slotCurrentQrcFileChanged(QListWidgetItem *item)
{
    if (m_ignoreCurrentChanged)
        return;

    QtQrcFile *newCurrentQrcFile = m_itemToQrcFile.value(item);

    if (newCurrentQrcFile == m_currentQrcFile)
        return;

    if (m_currentQrcFile) {
        QMap<QtResourcePrefix *, QStandardItem *> currentPrefixList = m_resourcePrefixToPrefixItem;
        QMapIterator<QtResourcePrefix *, QStandardItem *> itPrefix(currentPrefixList);
        while (itPrefix.hasNext()) {
            QtResourcePrefix *resourcePrefix = itPrefix.next().key();
            QList<QtResourceFile *> currentResourceFiles = resourcePrefix->resourceFiles();
            QListIterator<QtResourceFile *> itFile(currentResourceFiles);
            while (itFile.hasNext())
                slotResourceFileRemoved(itFile.next());
            slotResourcePrefixRemoved(resourcePrefix);
        }
    }

    m_currentQrcFile = newCurrentQrcFile;
    slotCurrentTreeViewItemChanged(QModelIndex());
    QStandardItem *firstPrefix = 0; // select first prefix
    if (m_currentQrcFile) {
        QList<QtResourcePrefix *> newPrefixList = m_currentQrcFile->resourcePrefixList();
        QListIterator<QtResourcePrefix *> itPrefix(newPrefixList);
        while (itPrefix.hasNext()) {
            QtResourcePrefix *resourcePrefix = itPrefix.next();
            if (QStandardItem *newPrefixItem = insertResourcePrefix(resourcePrefix))
                if (!firstPrefix)
                    firstPrefix = newPrefixItem;
            QList<QtResourceFile *> newResourceFiles = resourcePrefix->resourceFiles();
            QListIterator<QtResourceFile *> itFile(newResourceFiles);
            while (itFile.hasNext())
                slotResourceFileInserted(itFile.next());
        }
    }
    m_ui.resourceTreeView->setCurrentIndex(firstPrefix ? m_treeModel->indexFromItem(firstPrefix) : QModelIndex());

    m_removeQrcFileAction->setEnabled(m_currentQrcFile);
    m_moveUpQrcFileAction->setEnabled(m_currentQrcFile && m_qrcManager->prevQrcFile(m_currentQrcFile));
    m_moveDownQrcFileAction->setEnabled(m_currentQrcFile && m_qrcManager->nextQrcFile(m_currentQrcFile));
}

void QtResourceEditorDialogPrivate::slotCurrentTreeViewItemChanged(const QModelIndex &index)
{
    QStandardItem *item = m_treeModel->itemFromIndex(index);
    QtResourceFile *resourceFile = m_pathItemToResourceFile.value(item);
    if (!resourceFile)
        resourceFile = m_aliasItemToResourceFile.value(item);
    QtResourcePrefix *resourcePrefix = m_prefixItemToResourcePrefix.value(item);
    if (!resourcePrefix)
        resourcePrefix = m_languageItemToResourcePrefix.value(item);

    bool moveUpEnabled = false;
    bool moveDownEnabled = false;
    bool currentItem = resourceFile || resourcePrefix;

    if (resourceFile) {
        if (m_qrcManager->prevResourceFile(resourceFile))
            moveUpEnabled = true;
        if (m_qrcManager->nextResourceFile(resourceFile))
            moveDownEnabled = true;
    } else if (resourcePrefix) {
        if (m_qrcManager->prevResourcePrefix(resourcePrefix))
            moveUpEnabled = true;
        if (m_qrcManager->nextResourcePrefix(resourcePrefix))
            moveDownEnabled = true;
    }

    m_newPrefixAction->setEnabled(m_currentQrcFile);
    m_addResourceFileAction->setEnabled(currentItem);
    m_changePrefixAction->setEnabled(currentItem);
    m_changeLanguageAction->setEnabled(currentItem);
    m_changeAliasAction->setEnabled(resourceFile);
    m_removeAction->setEnabled(currentItem);
    m_moveUpAction->setEnabled(moveUpEnabled);
    m_moveDownAction->setEnabled(moveDownEnabled);
    m_clonePrefixAction->setEnabled(currentItem);
}

void QtResourceEditorDialogPrivate::slotListWidgetContextMenuRequested(const QPoint &pos)
{
    QMenu menu(q_ptr);
    menu.addAction(m_newQrcFileAction);
    menu.addAction(m_importQrcFileAction);
    menu.addAction(m_removeQrcFileAction);
    menu.addSeparator();
    menu.addAction(m_moveUpQrcFileAction);
    menu.addAction(m_moveDownQrcFileAction);
    menu.exec(m_ui.qrcFileList->mapToGlobal(pos));
}

void QtResourceEditorDialogPrivate::slotTreeViewContextMenuRequested(const QPoint &pos)
{
    QMenu menu(q_ptr);
    menu.addAction(m_newPrefixAction);
    menu.addAction(m_addResourceFileAction);
    menu.addAction(m_removeAction);
    menu.addSeparator();
    menu.addAction(m_changePrefixAction);
    menu.addAction(m_changeLanguageAction);
    menu.addAction(m_changeAliasAction);
    menu.addSeparator();
    menu.addAction(m_clonePrefixAction);
    menu.addSeparator();
    menu.addAction(m_moveUpAction);
    menu.addAction(m_moveDownAction);
    menu.exec(m_ui.resourceTreeView->mapToGlobal(pos));
}

void QtResourceEditorDialogPrivate::slotTreeViewItemChanged(QStandardItem *item)
{
    if (m_ignoreCurrentChanged)
        return;

    const QString newValue = item->text();
    QtResourceFile *resourceFile = m_aliasItemToResourceFile.value(item);
    if (resourceFile) {
        m_qrcManager->changeResourceAlias(resourceFile, newValue);
        return;
    }

    QtResourcePrefix *resourcePrefix = m_prefixItemToResourcePrefix.value(item);
    if (resourcePrefix) {
        m_qrcManager->changeResourcePrefix(resourcePrefix, newValue);
        return;
    }

    resourcePrefix = m_languageItemToResourcePrefix.value(item);
    if (resourcePrefix) {
        m_qrcManager->changeResourceLanguage(resourcePrefix, newValue);
        return;
    }
}

QString QtResourceEditorDialogPrivate::getSaveFileNameWithExtension(QWidget *parent,
            const QString &title, QString dir, const QString &filter, const QString &extension) const
{
    const QChar dot = QLatin1Char('.');

    QString saveFile;
    while (true) {
        saveFile = m_dlgGui->getSaveFileName(parent, title, dir, filter, 0, QFileDialog::DontConfirmOverwrite);
        if (saveFile.isEmpty())
            return saveFile;

        const QFileInfo fInfo(saveFile);
        if (fInfo.suffix().isEmpty() && !fInfo.fileName().endsWith(dot)) {
            saveFile += dot;
            saveFile += extension;
        }

        const QFileInfo fi(saveFile);
        if (!fi.exists())
            break;

        if (warning(title, msgOverwrite(fi.fileName()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            break;

        dir = saveFile;
    }
    return saveFile;
}

QString QtResourceEditorDialogPrivate::qrcStartDirectory() const
{
    if (!m_currentQrcFile)
        return QString();
    const QDir dir = QFileInfo(m_currentQrcFile->path()).dir();
    return dir.exists() ? dir.absolutePath() : QString();
}

void QtResourceEditorDialogPrivate::slotNewQrcFile()
{
    const QString qrcPath = getSaveFileNameWithExtension(q_ptr,
                QApplication::translate("QtResourceEditorDialog", "New Resource File", 0, QApplication::UnicodeUTF8),
                m_firstQrcFileDialog ? qrcStartDirectory() : QString(),
                QApplication::translate("QtResourceEditorDialog", "Resource files (*.qrc)", 0, QApplication::UnicodeUTF8),
                QLatin1String("qrc"));
    if (qrcPath.isEmpty())
        return;

    m_firstQrcFileDialog = false;
    if (QtQrcFile *sameQrcFile = m_qrcManager->qrcFileOf(qrcPath)) {
        // QMessageBox ???
        QListWidgetItem *item = m_qrcFileToItem.value(sameQrcFile);
        m_ui.qrcFileList->setCurrentItem(item);
        item->setSelected(true);
        return;
    }

    QtQrcFile *nextQrcFile = m_qrcManager->nextQrcFile(m_currentQrcFile);

    QtQrcFile *qrcFile = m_qrcManager->insertQrcFile(qrcPath, nextQrcFile, true);
    m_ui.qrcFileList->setCurrentItem(m_qrcFileToItem.value(qrcFile));
}

void QtResourceEditorDialogPrivate::slotImportQrcFile()
{
    const QString qrcPath = m_dlgGui->getOpenFileName(q_ptr,
                QApplication::translate("QtResourceEditorDialog", "Import Resource File", 0, QApplication::UnicodeUTF8),
                m_firstQrcFileDialog ? qrcStartDirectory() : QString(),
                QApplication::translate("QtResourceEditorDialog", "Resource files (*.qrc)", 0, QApplication::UnicodeUTF8));
    if (qrcPath.isEmpty())
        return;
    m_firstQrcFileDialog = false;
    if (QtQrcFile *sameQrcFile = m_qrcManager->qrcFileOf(qrcPath)) {
        // QMessageBox ???
        QListWidgetItem *item = m_qrcFileToItem.value(sameQrcFile);
        m_ui.qrcFileList->setCurrentItem(item);
        item->setSelected(true);
        return;
    }

    QtQrcFile *nextQrcFile = m_qrcManager->nextQrcFile(m_currentQrcFile);

    QtQrcFileData qrcFileData;
    loadQrcFile(qrcPath, &qrcFileData);
    QtQrcFile *qrcFile = m_qrcManager->importQrcFile(qrcFileData, nextQrcFile);
    m_ui.qrcFileList->setCurrentItem(m_qrcFileToItem.value(qrcFile));
}

void QtResourceEditorDialogPrivate::slotRemoveQrcFile()
{
    if (!m_currentQrcFile)
        return;

    QtQrcFile *currentQrcFile = m_qrcManager->nextQrcFile(m_currentQrcFile);
    if (!currentQrcFile)
        currentQrcFile = m_qrcManager->prevQrcFile(m_currentQrcFile);

    m_qrcManager->removeQrcFile(m_currentQrcFile);
    QListWidgetItem *item = m_qrcFileToItem.value(currentQrcFile);
    if (item) {
        m_ui.qrcFileList->setCurrentItem(item);
        item->setSelected(true);
    }
}

void QtResourceEditorDialogPrivate::slotMoveUpQrcFile()
{
    if (!m_currentQrcFile)
        return;

    QtQrcFile *prevQrcFile = m_qrcManager->prevQrcFile(m_currentQrcFile);
    if (!prevQrcFile)
        return;

    m_qrcManager->moveQrcFile(m_currentQrcFile, prevQrcFile);
}

void QtResourceEditorDialogPrivate::slotMoveDownQrcFile()
{
    if (!m_currentQrcFile)
        return;

    QtQrcFile *nextQrcFile = m_qrcManager->nextQrcFile(m_currentQrcFile);
    if (!nextQrcFile)
        return;
    nextQrcFile = m_qrcManager->nextQrcFile(nextQrcFile);

    m_qrcManager->moveQrcFile(m_currentQrcFile, nextQrcFile);
}

QtResourceFile *QtResourceEditorDialogPrivate::getCurrentResourceFile() const
{
    QStandardItem *currentItem = m_treeModel->itemFromIndex(m_treeSelection->currentIndex());


    QtResourceFile *currentResourceFile = 0;
    if (currentItem) {
        currentResourceFile = m_pathItemToResourceFile.value(currentItem);
        if (!currentResourceFile)
            currentResourceFile = m_aliasItemToResourceFile.value(currentItem);
    }
    return currentResourceFile;
}

QtResourcePrefix *QtResourceEditorDialogPrivate::getCurrentResourcePrefix() const
{
    QStandardItem *currentItem = m_treeModel->itemFromIndex(m_treeSelection->currentIndex());

    QtResourcePrefix *currentResourcePrefix = 0;
    if (currentItem) {
        currentResourcePrefix = m_prefixItemToResourcePrefix.value(currentItem);
        if (!currentResourcePrefix) {
            currentResourcePrefix = m_languageItemToResourcePrefix.value(currentItem);
            if (!currentResourcePrefix) {
                QtResourceFile *currentResourceFile = getCurrentResourceFile();
                if (currentResourceFile)
                    currentResourcePrefix = m_qrcManager->resourcePrefixOf(currentResourceFile);
            }
        }
    }
    return currentResourcePrefix;
}

void QtResourceEditorDialogPrivate::selectTreeRow(QStandardItem *item)
{
    const QModelIndex index = m_treeModel->indexFromItem(item);
    m_treeSelection->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    m_treeSelection->setCurrentIndex(index, QItemSelectionModel::Select);
}

void QtResourceEditorDialogPrivate::slotNewPrefix()
{
    if (!m_currentQrcFile)
        return;

    QtResourcePrefix *currentResourcePrefix = getCurrentResourcePrefix();
    QtResourcePrefix *nextResourcePrefix = m_qrcManager->nextResourcePrefix(currentResourcePrefix);
    QtResourcePrefix *newResourcePrefix = m_qrcManager->insertResourcePrefix(m_currentQrcFile,
                QApplication::translate("QtResourceEditorDialog", "newPrefix", 0, QApplication::UnicodeUTF8),
                QString(), nextResourcePrefix);
    if (!newResourcePrefix)
        return;

    QStandardItem *newItem = m_resourcePrefixToPrefixItem.value(newResourcePrefix);
    if (!newItem)
        return;

    const QModelIndex index = m_treeModel->indexFromItem(newItem);
    selectTreeRow(newItem);
    m_ui.resourceTreeView->edit(index);
}

static inline QString outOfPathWarning(const QString &fname)
{
    return QApplication::translate("QtResourceEditorDialog",
    "<p><b>Warning:</b> The file</p>"
    "<p>%1</p>"
    "<p>is outside of the current resource file's parent directory.</p>").arg(fname);
}

static inline QString outOfPathWarningInfo()
{
    return QApplication::translate("QtResourceEditorDialog",
    "<p>To resolve the issue, press:</p>"
    "<table>"
    "<tr><th align=\"left\">Copy</th><td>to copy the file to the resource file's parent directory.</td></tr>"
    "<tr><th align=\"left\">Copy As...</th><td>to copy the file into a subdirectory of the resource file's parent directory.</td></tr>"
    "<tr><th align=\"left\">Keep</th><td>to use its current location.</td></tr></table>");
}

void QtResourceEditorDialogPrivate::slotAddFiles()
{
    if (!m_currentQrcFile)
        return;

    QtResourcePrefix *currentResourcePrefix = getCurrentResourcePrefix();
    QtResourceFile *currentResourceFile = getCurrentResourceFile();
    if (!currentResourcePrefix)
        return;

    QString initialPath = m_currentQrcFile->path();
    if (currentResourceFile) {
        QFileInfo fi(currentResourceFile->fullPath());
        initialPath = fi.absolutePath();
    }

    const QStringList resourcePaths = m_dlgGui->getOpenImageFileNames(q_ptr,
                QApplication::translate("QtResourceEditorDialog", "Add Files", 0, QApplication::UnicodeUTF8),
                initialPath);
    if (resourcePaths.isEmpty())
        return;

    QtResourceFile *nextResourceFile = m_qrcManager->nextResourceFile(currentResourceFile);
    if (!currentResourceFile) {
        QList<QtResourceFile *> resourceFiles = currentResourcePrefix->resourceFiles();
        if (resourceFiles.count() > 0)
            nextResourceFile = resourceFiles.first();
    }

    const QFileInfo fi(m_currentQrcFile->path());
    const QString destDir = fi.absolutePath();
    const QDir dir(fi.absolutePath());
    QStringListIterator itResourcePath(resourcePaths);
    while (itResourcePath.hasNext()) {
        QString resourcePath = itResourcePath.next();
        QString relativePath = dir.relativeFilePath(resourcePath);
        if (relativePath.startsWith(QLatin1String(".."))) {
            QMessageBox msgBox(QMessageBox::Warning,
                    QApplication::translate("QtResourceEditorDialog", "Incorrect Path", 0, QApplication::UnicodeUTF8),
                    outOfPathWarning(relativePath), QMessageBox::Cancel);
            msgBox.setInformativeText(outOfPathWarningInfo());
            QPushButton *copyButton = msgBox.addButton(QApplication::translate("QtResourceEditorDialog",
                        "Copy", 0, QApplication::UnicodeUTF8), QMessageBox::ActionRole);
            QPushButton *copyAsButton = msgBox.addButton(QApplication::translate("QtResourceEditorDialog",
                        "Copy As...", 0, QApplication::UnicodeUTF8), QMessageBox::ActionRole);
            QPushButton *keepButton = msgBox.addButton(QApplication::translate("QtResourceEditorDialog",
                        "Keep", 0, QApplication::UnicodeUTF8), QMessageBox::ActionRole);
            QPushButton *skipButton = msgBox.addButton(QApplication::translate("QtResourceEditorDialog",
                        "Skip", 0, QApplication::UnicodeUTF8), QMessageBox::ActionRole);
            msgBox.setEscapeButton(QMessageBox::Cancel);
            msgBox.setDefaultButton(copyButton);
            msgBox.exec();
            QString destPath;
            if (msgBox.clickedButton() == keepButton) {
                // nothing
            } else if (msgBox.clickedButton() == copyButton) {
                QFileInfo resInfo(resourcePath);
                QDir dd(destDir);
                destPath = dd.absoluteFilePath(resInfo.fileName());
                if (dd.exists(resInfo.fileName())) {
                    if (warning(QApplication::translate("QtResourceEditorDialog", "Copy", 0, QApplication::UnicodeUTF8),
                                msgOverwrite(resInfo.fileName()),
                                QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel) != QMessageBox::Yes)
                        continue;
                }
                resourcePath = copyResourceFile(resourcePath, destPath); // returns empty string in case copy failed or was canceled
            } else if (msgBox.clickedButton() == copyAsButton) {
                destPath = browseForNewLocation(resourcePath, dir); // returns empty string in case browsing was canceled
                if (destPath.isEmpty())
                    continue;
                resourcePath = copyResourceFile(resourcePath, destPath); // returns empty string in case copy failed or was canceled
            } else if (msgBox.clickedButton() == skipButton) { // skipped
                continue;
            } else { // canceled
                return;
            }
            if (resourcePath.isEmpty())
                continue;
        }
        relativePath = dir.relativeFilePath(resourcePath);
        QtResourceFile *newResourceFile = m_qrcManager->insertResourceFile(currentResourcePrefix, relativePath, QString(), nextResourceFile);

        QStandardItem *newItem = m_resourceFileToPathItem.value(newResourceFile);
        if (newItem)
            selectTreeRow(newItem);
    }
}

void QtResourceEditorDialogPrivate::slotChangePrefix()
{
    QtResourcePrefix *currentResourcePrefix = getCurrentResourcePrefix();
    if (!currentResourcePrefix)
        return;

    QStandardItem *item = m_resourcePrefixToPrefixItem.value(currentResourcePrefix);
    QModelIndex index = m_treeModel->indexFromItem(item);
    selectTreeRow(item);
    m_ui.resourceTreeView->scrollTo(index);
    m_ui.resourceTreeView->edit(index);
}

void QtResourceEditorDialogPrivate::slotChangeLanguage()
{
    QtResourcePrefix *currentResourcePrefix = getCurrentResourcePrefix();
    if (!currentResourcePrefix)
        return;

    QStandardItem *item = m_resourcePrefixToLanguageItem.value(currentResourcePrefix);
    QModelIndex index = m_treeModel->indexFromItem(item);
    selectTreeRow(item);
    m_ui.resourceTreeView->scrollTo(index);
    m_ui.resourceTreeView->edit(index);
}

void QtResourceEditorDialogPrivate::slotChangeAlias()
{
    QtResourceFile *currentResourceFile = getCurrentResourceFile();
    if (!currentResourceFile)
        return;

    QStandardItem *item = m_resourceFileToAliasItem.value(currentResourceFile);
    QModelIndex index = m_treeModel->indexFromItem(item);
    selectTreeRow(item);
    m_ui.resourceTreeView->scrollTo(index);
    m_ui.resourceTreeView->edit(index);
}

void QtResourceEditorDialogPrivate::slotClonePrefix()
{
    QtResourcePrefix *currentResourcePrefix = getCurrentResourcePrefix();
    if (!currentResourcePrefix)
        return;

    bool ok;
    QString suffix = QInputDialog::getText(q_ptr, QApplication::translate("QtResourceEditorDialog", "Clone Prefix", 0, QApplication::UnicodeUTF8),
            QApplication::translate("QtResourceEditorDialog", "Enter the suffix which you want to add to the names of the cloned files.\n"
                "This could for example be a language extension like \"_de\".", 0, QApplication::UnicodeUTF8),
            QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;

    QtResourcePrefix *newResourcePrefix = m_qrcManager->insertResourcePrefix(m_currentQrcFile, currentResourcePrefix->prefix(),
                                    currentResourcePrefix->language(), m_qrcManager->nextResourcePrefix(currentResourcePrefix));
    if (newResourcePrefix) {
        QList<QtResourceFile *> files = currentResourcePrefix->resourceFiles();
        QListIterator<QtResourceFile *> itFile(files);
        while (itFile.hasNext()) {
            QtResourceFile *resourceFile = itFile.next();
            QString path = resourceFile->path();
            QFileInfo fi(path);
            QDir dir(fi.dir());
            QString oldSuffix = fi.completeSuffix();
            if (!oldSuffix.isEmpty())
                oldSuffix = QLatin1Char('.') + oldSuffix;
            const QString newBaseName = fi.baseName() + suffix + oldSuffix;
            const QString newPath = QDir::cleanPath(dir.filePath(newBaseName));
            m_qrcManager->insertResourceFile(newResourcePrefix, newPath,
                    resourceFile->alias());
        }
    }
}

void QtResourceEditorDialogPrivate::slotRemove()
{
    QStandardItem *item = m_treeModel->itemFromIndex(m_treeSelection->currentIndex());
    if (!item)
        return;

    QtResourceFile *resourceFile = m_pathItemToResourceFile.value(item);
    if (!resourceFile)
        resourceFile = m_aliasItemToResourceFile.value(item);
    QtResourcePrefix *resourcePrefix = m_prefixItemToResourcePrefix.value(item);
    if (!resourcePrefix)
        resourcePrefix = m_languageItemToResourcePrefix.value(item);

    QStandardItem *newCurrentItem = 0;

    if (resourceFile) {
        QtResourceFile *nextFile = m_qrcManager->nextResourceFile(resourceFile);
        if (!nextFile)
            nextFile = m_qrcManager->prevResourceFile(resourceFile);
        newCurrentItem = m_resourceFileToPathItem.value(nextFile);
        if (!newCurrentItem)
            newCurrentItem = m_resourcePrefixToPrefixItem.value(m_qrcManager->resourcePrefixOf(resourceFile));
    }
    if (!newCurrentItem) {
        QtResourcePrefix *nextPrefix = m_qrcManager->nextResourcePrefix(resourcePrefix);
        if (!nextPrefix)
            nextPrefix = m_qrcManager->prevResourcePrefix(resourcePrefix);
        newCurrentItem = m_resourcePrefixToPrefixItem.value(nextPrefix);
    }

    selectTreeRow(newCurrentItem);

    if (resourcePrefix)
        m_qrcManager->removeResourcePrefix(resourcePrefix);
    else if (resourceFile)
        m_qrcManager->removeResourceFile(resourceFile);
}

void QtResourceEditorDialogPrivate::slotMoveUp()
{
    if (QtResourceFile *resourceFile = getCurrentResourceFile()) {
        QtResourceFile *prevFile = m_qrcManager->prevResourceFile(resourceFile);

        if (!prevFile)
            return;

        m_qrcManager->moveResourceFile(resourceFile, prevFile);
        selectTreeRow(m_resourceFileToPathItem.value(resourceFile));
    } else if (QtResourcePrefix *resourcePrefix = getCurrentResourcePrefix()) {
        QtResourcePrefix *prevPrefix = m_qrcManager->prevResourcePrefix(resourcePrefix);

        if (!prevPrefix)
            return;

        m_qrcManager->moveResourcePrefix(resourcePrefix, prevPrefix);
        selectTreeRow(m_resourcePrefixToPrefixItem.value(resourcePrefix));
    }
}

void QtResourceEditorDialogPrivate::slotMoveDown()
{
    if (QtResourceFile *resourceFile = getCurrentResourceFile()) {
        QtResourceFile *nextFile = m_qrcManager->nextResourceFile(resourceFile);

        if (!nextFile)
            return;

        m_qrcManager->moveResourceFile(resourceFile, m_qrcManager->nextResourceFile(nextFile));
        selectTreeRow(m_resourceFileToPathItem.value(resourceFile));
    } else if (QtResourcePrefix *resourcePrefix = getCurrentResourcePrefix()) {
        QtResourcePrefix *nextPrefix = m_qrcManager->nextResourcePrefix(resourcePrefix);

        if (!nextPrefix)
            return;

        m_qrcManager->moveResourcePrefix(resourcePrefix, m_qrcManager->nextResourcePrefix(nextPrefix));
        selectTreeRow(m_resourcePrefixToPrefixItem.value(resourcePrefix));
    }
}

QString QtResourceEditorDialogPrivate::browseForNewLocation(const QString &resourceFile, const QDir &rootDir) const
{
    QFileInfo fi(resourceFile);
    const QString initialPath = rootDir.absoluteFilePath(fi.fileName());
    while (1) {
        QString newPath = m_dlgGui->getSaveFileName(q_ptr,
                    QApplication::translate("QtResourceEditorDialog", "Copy As", 0, QApplication::UnicodeUTF8),
                    initialPath);
        QString relativePath = rootDir.relativeFilePath(newPath);
        if (relativePath.startsWith(QLatin1String(".."))) {
            if (warning(QApplication::translate("QtResourceEditorDialog", "Copy As", 0, QApplication::UnicodeUTF8),
                        QApplication::translate("QtResourceEditorDialog", "<p>The selected file:</p>"
                                        "<p>%1</p><p>is outside of the current resource file's directory:</p><p>%2</p>"
                                        "<p>Please select another path within this directory.<p>", 0,
                                        QApplication::UnicodeUTF8).arg(relativePath).arg(rootDir.absolutePath()),
                        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok) != QMessageBox::Ok)
                return QString();
        } else {
            return newPath;
        }
    }

    return QString();
}

QString QtResourceEditorDialogPrivate::copyResourceFile(const QString &resourceFile, const QString &destPath) const
{
    QFileInfo fi(destPath);
    if (fi.exists()) {
        while (fi.exists() && !QFile::remove(destPath)) {
            if (warning(QApplication::translate("QtResourceEditorDialog", "Copy", 0, QApplication::UnicodeUTF8),
                        QApplication::translate("QtResourceEditorDialog", "Could not overwrite %1.", 0, QApplication::UnicodeUTF8).arg(fi.fileName()),
                        QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Cancel) != QMessageBox::Retry)
                return QString();
        }
    }
    while (!QFile::copy(resourceFile, destPath)) {
        if (warning(QApplication::translate("QtResourceEditorDialog", "Copy", 0, QApplication::UnicodeUTF8),
                    QApplication::translate("QtResourceEditorDialog", "Could not copy\n%1\nto\n%2",
                        0, QApplication::UnicodeUTF8).arg(resourceFile).arg(destPath),
                    QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Cancel) != QMessageBox::Retry)
            return QString();
    }
    return destPath;
}
bool QtResourceEditorDialogPrivate::loadQrcFile(const QString &path, QtQrcFileData *qrcFileData)
{
    QString errorMessage;
    const bool rc = loadQrcFile(path, qrcFileData, &errorMessage);
//    if (!rc)
//        warning(QApplication::translate("QtResourceEditorDialog", "Resource File Load Error"), errorMessage);
    return rc;
}
bool QtResourceEditorDialogPrivate::loadQrcFile(const QString &path, QtQrcFileData *qrcFileData, QString *errorMessage)
{
    if (!qrcFileData)
        return false;

    qrcFileData->qrcPath = path;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        // there is sufficient hint while loading a form and after opening the editor (qrc marked marked with red and with [missing] text)
        //*errorMessage =  QApplication::translate("QtResourceEditorDialog", "Unable to open %1 for reading: %2").arg(path).arg(file.errorString());
        return false;
    }

    QByteArray dataArray = file.readAll();
    file.close();

    QDomDocument doc;
    int errLine, errCol;
    if (!doc.setContent(dataArray, errorMessage, &errLine, &errCol))  {
        *errorMessage = QCoreApplication::translate("QtResourceEditorDialog", "A parse error occurred at line %1, column %2 of %3:\n%4").arg(errLine).arg(errCol).arg(path).arg(*errorMessage);
        return false;
    }

    return loadQrcFileData(doc, path, qrcFileData, errorMessage);
}

bool QtResourceEditorDialogPrivate::saveQrcFile(const QtQrcFileData &qrcFileData)
{
    QFile file(qrcFileData.qrcPath);
    while (!file.open(QIODevice::WriteOnly)) {
        QMessageBox msgBox(QMessageBox::Warning,
                QApplication::translate("QtResourceEditorDialog", "Save Resource File", 0, QApplication::UnicodeUTF8),
                QApplication::translate("QtResourceEditorDialog", "Could not write %1: %2", 0, QApplication::UnicodeUTF8).arg(qrcFileData.qrcPath).arg(file.errorString()),
                QMessageBox::Cancel|QMessageBox::Ignore|QMessageBox::Retry);
        msgBox.setEscapeButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ignore);
        switch (msgBox.exec()) {
        case QMessageBox::Retry:
            break; // nothing
        case QMessageBox::Ignore:
            return true;
        default:
            return false;
        }
    }

    QDomDocument doc = saveQrcFileData(qrcFileData);

    QByteArray dataArray = doc.toByteArray(2);
    file.write(dataArray);

    file.close();
    return true;
}

QtResourceEditorDialog::QtResourceEditorDialog(QDesignerFormEditorInterface *core, QDesignerDialogGuiInterface *dlgGui, QWidget *parent)
    : QDialog(parent), d_ptr(new QtResourceEditorDialogPrivate())
{
    d_ptr->q_ptr = this;
    d_ptr->m_ui.setupUi(this);
    d_ptr->m_qrcManager = new QtQrcManager(this);
    d_ptr->m_dlgGui = dlgGui;
    d_ptr->m_core = core;

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Edit Resources"));

    connect(d_ptr->m_qrcManager, SIGNAL(qrcFileInserted(QtQrcFile*)),
                this, SLOT(slotQrcFileInserted(QtQrcFile*)));
    connect(d_ptr->m_qrcManager, SIGNAL(qrcFileMoved(QtQrcFile*,QtQrcFile*)),
                this, SLOT(slotQrcFileMoved(QtQrcFile*)));
    connect(d_ptr->m_qrcManager, SIGNAL(qrcFileRemoved(QtQrcFile*)),
                this, SLOT(slotQrcFileRemoved(QtQrcFile*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourcePrefixInserted(QtResourcePrefix*)),
                this, SLOT(slotResourcePrefixInserted(QtResourcePrefix*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourcePrefixMoved(QtResourcePrefix*,QtResourcePrefix*)),
                this, SLOT(slotResourcePrefixMoved(QtResourcePrefix*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourcePrefixChanged(QtResourcePrefix*,QString)),
                this, SLOT(slotResourcePrefixChanged(QtResourcePrefix*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourceLanguageChanged(QtResourcePrefix*,QString)),
                this, SLOT(slotResourceLanguageChanged(QtResourcePrefix*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourcePrefixRemoved(QtResourcePrefix*)),
                this, SLOT(slotResourcePrefixRemoved(QtResourcePrefix*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourceFileInserted(QtResourceFile*)),
                this, SLOT(slotResourceFileInserted(QtResourceFile*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourceFileMoved(QtResourceFile*,QtResourceFile*)),
                this, SLOT(slotResourceFileMoved(QtResourceFile*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourceAliasChanged(QtResourceFile*,QString)),
                this, SLOT(slotResourceAliasChanged(QtResourceFile*)));
    connect(d_ptr->m_qrcManager, SIGNAL(resourceFileRemoved(QtResourceFile*)),
                this, SLOT(slotResourceFileRemoved(QtResourceFile*)));

    QIcon upIcon = qdesigner_internal::createIconSet(QString::fromUtf8("up.png"));
    QIcon downIcon = qdesigner_internal::createIconSet(QString::fromUtf8("down.png"));
    QIcon minusIcon = qdesigner_internal::createIconSet(QString::fromUtf8("minus-16.png"));
    QIcon newIcon = qdesigner_internal::createIconSet(QString::fromUtf8("filenew-16.png"));
    QIcon openIcon = qdesigner_internal::createIconSet(QString::fromUtf8("fileopen-16.png"));
    QIcon removeIcon = qdesigner_internal::createIconSet(QString::fromUtf8("editdelete-16.png"));
    QIcon addPrefixIcon = qdesigner_internal::createIconSet(QString::fromUtf8("prefix-add.png"));

    d_ptr->m_newQrcFileAction = new QAction(newIcon, tr("New..."), this);
    d_ptr->m_newQrcFileAction->setToolTip(tr("New Resource File"));
    d_ptr->m_importQrcFileAction = new QAction(openIcon, tr("Open..."), this);
    d_ptr->m_importQrcFileAction->setToolTip(tr("Open Resource File"));
    d_ptr->m_removeQrcFileAction = new QAction(removeIcon, tr("Remove"), this);
    d_ptr->m_moveUpQrcFileAction = new QAction(upIcon, tr("Move Up"), this);
    d_ptr->m_moveDownQrcFileAction = new QAction(downIcon, tr("Move Down"), this);

    d_ptr->m_newPrefixAction = new QAction(addPrefixIcon, tr("Add Prefix"), this);
    d_ptr->m_newPrefixAction->setToolTip(tr("Add Prefix"));
    d_ptr->m_addResourceFileAction = new QAction(openIcon, tr("Add Files..."), this);
    d_ptr->m_changePrefixAction = new QAction(tr("Change Prefix"), this);
    d_ptr->m_changeLanguageAction = new QAction(tr("Change Language"), this);
    d_ptr->m_changeAliasAction = new QAction(tr("Change Alias"), this);
    d_ptr->m_clonePrefixAction = new QAction(tr("Clone Prefix..."), this);
    d_ptr->m_removeAction = new QAction(minusIcon, tr("Remove"), this);
    d_ptr->m_moveUpAction = new QAction(upIcon, tr("Move Up"), this);
    d_ptr->m_moveDownAction = new QAction(downIcon, tr("Move Down"), this);

    d_ptr->m_ui.newQrcButton->setDefaultAction(d_ptr->m_newQrcFileAction);
    d_ptr->m_ui.importQrcButton->setDefaultAction(d_ptr->m_importQrcFileAction);
    d_ptr->m_ui.removeQrcButton->setDefaultAction(d_ptr->m_removeQrcFileAction);

    d_ptr->m_ui.newResourceButton->setDefaultAction(d_ptr->m_newPrefixAction);
    d_ptr->m_ui.addResourceButton->setDefaultAction(d_ptr->m_addResourceFileAction);
    d_ptr->m_ui.removeResourceButton->setDefaultAction(d_ptr->m_removeAction);

    connect(d_ptr->m_newQrcFileAction, SIGNAL(triggered()), this, SLOT(slotNewQrcFile()));
    connect(d_ptr->m_importQrcFileAction, SIGNAL(triggered()), this, SLOT(slotImportQrcFile()));
    connect(d_ptr->m_removeQrcFileAction, SIGNAL(triggered()), this, SLOT(slotRemoveQrcFile()));
    connect(d_ptr->m_moveUpQrcFileAction, SIGNAL(triggered()), this, SLOT(slotMoveUpQrcFile()));
    connect(d_ptr->m_moveDownQrcFileAction, SIGNAL(triggered()), this, SLOT(slotMoveDownQrcFile()));

    connect(d_ptr->m_newPrefixAction, SIGNAL(triggered()), this, SLOT(slotNewPrefix()));
    connect(d_ptr->m_addResourceFileAction, SIGNAL(triggered()), this, SLOT(slotAddFiles()));
    connect(d_ptr->m_changePrefixAction, SIGNAL(triggered()), this, SLOT(slotChangePrefix()));
    connect(d_ptr->m_changeLanguageAction, SIGNAL(triggered()), this, SLOT(slotChangeLanguage()));
    connect(d_ptr->m_changeAliasAction, SIGNAL(triggered()), this, SLOT(slotChangeAlias()));
    connect(d_ptr->m_clonePrefixAction, SIGNAL(triggered()), this, SLOT(slotClonePrefix()));
    connect(d_ptr->m_removeAction, SIGNAL(triggered()), this, SLOT(slotRemove()));
    connect(d_ptr->m_moveUpAction, SIGNAL(triggered()), this, SLOT(slotMoveUp()));
    connect(d_ptr->m_moveDownAction, SIGNAL(triggered()), this, SLOT(slotMoveDown()));

    d_ptr->m_ui.qrcFileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d_ptr->m_ui.qrcFileList, SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(slotListWidgetContextMenuRequested(QPoint)));
    connect(d_ptr->m_ui.qrcFileList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                    this, SLOT(slotCurrentQrcFileChanged(QListWidgetItem*)));

    d_ptr->m_treeModel = new QStandardItemModel(this);
    d_ptr->m_treeModel->setColumnCount(2);
    d_ptr->m_treeModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Prefix / Path")));
    d_ptr->m_treeModel->setHorizontalHeaderItem(1, new QStandardItem(tr("Language / Alias")));
    d_ptr->m_ui.resourceTreeView->setModel(d_ptr->m_treeModel);
    d_ptr->m_ui.resourceTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    d_ptr->m_treeSelection = d_ptr->m_ui.resourceTreeView->selectionModel();
    connect(d_ptr->m_ui.resourceTreeView->header(), SIGNAL(sectionDoubleClicked(int)), d_ptr->m_ui.resourceTreeView, SLOT(resizeColumnToContents(int)));
    d_ptr->m_ui.resourceTreeView->setTextElideMode(Qt::ElideLeft);

    connect(d_ptr->m_ui.resourceTreeView, SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(slotTreeViewContextMenuRequested(QPoint)));
    connect(d_ptr->m_treeModel, SIGNAL(itemChanged(QStandardItem*)),
                this, SLOT(slotTreeViewItemChanged(QStandardItem*)));
    connect(d_ptr->m_treeSelection, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    this, SLOT(slotCurrentTreeViewItemChanged(QModelIndex)));

    d_ptr->m_ui.resourceTreeView->setColumnWidth(0, 200);

    d_ptr->slotCurrentTreeViewItemChanged(QModelIndex());
    d_ptr->m_removeQrcFileAction->setEnabled(false);
    d_ptr->m_moveUpQrcFileAction->setEnabled(false);
    d_ptr->m_moveDownQrcFileAction->setEnabled(false);

    QDesignerSettingsInterface *settings = core->settingsManager();
    settings->beginGroup(QLatin1String(QrcDialogC));

    d_ptr->m_ui.splitter->restoreState(settings->value(QLatin1String(SplitterPosition)).toByteArray());
    if (settings->contains(QLatin1String(Geometry)))
        setGeometry(settings->value(QLatin1String(Geometry)).toRect());

    settings->endGroup();
}

QtResourceEditorDialog::~QtResourceEditorDialog()
{
    QDesignerSettingsInterface *settings = d_ptr->m_core->settingsManager();
    settings->beginGroup(QLatin1String(QrcDialogC));

    settings->setValue(QLatin1String(SplitterPosition), d_ptr->m_ui.splitter->saveState());
    settings->setValue(QLatin1String(Geometry), geometry());
    settings->endGroup();
}

QtResourceModel *QtResourceEditorDialog::model() const
{
    return d_ptr->m_resourceModel;
}

void QtResourceEditorDialog::setResourceModel(QtResourceModel *model)
{
    d_ptr->m_resourceModel = model;

    QtResourceSet *resourceSet = d_ptr->m_resourceModel->currentResourceSet();
    if (!resourceSet) {
        // disable everything but cancel button
        return;
    }

    d_ptr->m_initialState.clear();

    // enable qrcBox

    QStringList paths = resourceSet->activeQrcPaths();
    QStringListIterator it(paths);
    while (it.hasNext()) {
        const QString path = it.next();
        QtQrcFileData qrcFileData;
        d_ptr->loadQrcFile(path, &qrcFileData);
        d_ptr->m_initialState << qrcFileData;
        d_ptr->m_qrcManager->importQrcFile(qrcFileData);
    }
    if (d_ptr->m_ui.qrcFileList->count() > 0) {
        d_ptr->m_ui.qrcFileList->item(0)->setSelected(true);
    }
}

QString QtResourceEditorDialog::selectedResource() const
{
    //QtResourcePrefix *currentResourcePrefix = d_ptr->m_qrcManager->resourcePrefixOf(currentResourceFile);
    QtResourcePrefix *currentResourcePrefix = d_ptr->getCurrentResourcePrefix();
    if (!currentResourcePrefix)
        return QString();

    const QChar slash(QLatin1Char('/'));
    QString resource = currentResourcePrefix->prefix();
    if (!resource.startsWith(slash))
        resource.prepend(slash);
    if (!resource.endsWith(slash))
        resource.append(slash);
    resource.prepend(QLatin1Char(':'));

    QtResourceFile *currentResourceFile = d_ptr->getCurrentResourceFile();
    if (!currentResourceFile)
        return resource;

    QString resourceEnding = currentResourceFile->path();
    if (!currentResourceFile->alias().isEmpty())
        resourceEnding = currentResourceFile->alias();

    const QString dotSlash(QLatin1String("./"));
    const QString dotDotSlash(QLatin1String("../"));
    while (1) {
        if (resourceEnding.startsWith(slash))
            resourceEnding = resourceEnding.mid(1);
        else if (resourceEnding.startsWith(dotSlash))
            resourceEnding = resourceEnding.mid(dotSlash.count());
        else if (resourceEnding.startsWith(dotDotSlash))
            resourceEnding = resourceEnding.mid(dotDotSlash.count());
        else
            break;
    }

    resource.append(resourceEnding);

    return resource;
}

void QtResourceEditorDialog::displayResourceFailures(const QString &logOutput, QDesignerDialogGuiInterface *dlgGui, QWidget *parent)
{
    const QString msg = tr("<html><p><b>Warning:</b> There have been problems while reloading the resources:</p><pre>%1</pre></html>").arg(logOutput);
    dlgGui->message(parent, QDesignerDialogGuiInterface::ResourceEditorMessage, QMessageBox::Warning,
                    tr("Resource Warning"), msg);
}

void QtResourceEditorDialog::accept()
{
    QStringList newQrcPaths;
    QList<QtQrcFileData> currentState;

    QList<QtQrcFile *> qrcFiles = d_ptr->m_qrcManager->qrcFiles();
    QListIterator<QtQrcFile *> itQrc(qrcFiles);
    while (itQrc.hasNext()) {
        QtQrcFile *qrcFile = itQrc.next();
        QtQrcFileData qrcFileData;
        d_ptr->m_qrcManager->exportQrcFile(qrcFile, &qrcFileData);
        currentState << qrcFileData;
        if (qrcFileData == qrcFile->initialState()) {
            // nothing
        } else {
            d_ptr->m_resourceModel->setWatcherEnabled(qrcFileData.qrcPath, false);
            bool ok = d_ptr->saveQrcFile(qrcFileData);
            d_ptr->m_resourceModel->setWatcherEnabled(qrcFileData.qrcPath, true);
            if (!ok)
                return;

            d_ptr->m_resourceModel->setModified(qrcFileData.qrcPath);
        }
        newQrcPaths << qrcFileData.qrcPath;
    }

    if (currentState == d_ptr->m_initialState) {
        // nothing
    } else {
        int errorCount;
        QString errorMessages;
        d_ptr->m_resourceModel->currentResourceSet()->activateQrcPaths(newQrcPaths, &errorCount, &errorMessages);
        if (errorCount)
            displayResourceFailures(errorMessages, d_ptr->m_dlgGui, this);
    }
    QDialog::accept();
}

QString QtResourceEditorDialog::editResources(QDesignerFormEditorInterface *core,
                                              QtResourceModel *model,
                                              QDesignerDialogGuiInterface *dlgGui,
                                              QWidget *parent)
{
    QtResourceEditorDialog dialog(core, dlgGui, parent);
    dialog.setResourceModel(model);
    if (dialog.exec() == QDialog::Accepted)
        return dialog.selectedResource();
    return QString();
}

QT_END_NAMESPACE

#include "moc_qtresourceeditordialog_p.cpp"
#include "qtresourceeditordialog.moc"
