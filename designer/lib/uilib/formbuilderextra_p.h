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

#ifndef ABSTRACTFORMBUILDERPRIVATE_H
#define ABSTRACTFORMBUILDERPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "uilib_global.h"

#ifndef QT_FORMBUILDER_NO_SCRIPT
#    include "formscriptrunner_p.h"
#endif

#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

class QObject;
class QVariant;
class QWidget;
class QObject;
class QLabel;
class QButtonGroup;

class QBoxLayout;
class QGridLayout;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal
{
#endif

class DomButtonGroups;
class DomButtonGroup;
class DomCustomWidget;

class QAbstractFormBuilder;
class QResourceBuilder;
class QTextBuilder;

class QDESIGNER_UILIB_EXPORT QFormBuilderExtra
{
    QFormBuilderExtra();
    ~QFormBuilderExtra();
public:
    struct CustomWidgetData {
        CustomWidgetData();
        explicit CustomWidgetData(const DomCustomWidget *dc);

        QString addPageMethod;
        QString script;
        QString baseClass;
        bool isContainer;
    };

    void clear();

    bool applyPropertyInternally(QObject *o, const QString &propertyName, const QVariant &value);

    enum BuddyMode { BuddyApplyAll, BuddyApplyVisibleOnly };

    void applyInternalProperties() const;
    static bool applyBuddy(const QString &buddyName, BuddyMode applyMode, QLabel *label);

    const QPointer<QWidget> &parentWidget() const;
    bool parentWidgetIsSet() const;
    void setParentWidget(const QPointer<QWidget> &w);

#ifndef QT_FORMBUILDER_NO_SCRIPT
    QFormScriptRunner &formScriptRunner();
    QString customWidgetScript(const QString &className) const;
#endif

    void setProcessingLayoutWidget(bool processing);
    bool processingLayoutWidget() const;

    void setResourceBuilder(QResourceBuilder *builder);
    QResourceBuilder *resourceBuilder() const;

    void setTextBuilder(QTextBuilder *builder);
    QTextBuilder *textBuilder() const;

    static QFormBuilderExtra *instance(const QAbstractFormBuilder *afb);
    static void removeInstance(const QAbstractFormBuilder *afb);

    void storeCustomWidgetData(const QString &className, const DomCustomWidget *d);
    QString customWidgetAddPageMethod(const QString &className) const;
    QString customWidgetBaseClass(const QString &className) const;
    bool isCustomWidgetContainer(const QString &className) const;

    // --- Hash used in creating button groups on demand. Store a map of name and pair of dom group and real group
    void registerButtonGroups(const DomButtonGroups *groups);

    typedef QPair<DomButtonGroup *, QButtonGroup*> ButtonGroupEntry;
    typedef QHash<QString, ButtonGroupEntry> ButtonGroupHash;
    const ButtonGroupHash &buttonGroups() const { return m_buttonGroups; }
    ButtonGroupHash &buttonGroups()  { return m_buttonGroups; }

    // return stretch as a comma-separated list
    static QString boxLayoutStretch(const QBoxLayout*);
    // apply stretch
    static bool setBoxLayoutStretch(const QString &, QBoxLayout*);
    static void clearBoxLayoutStretch(QBoxLayout*);

    static QString gridLayoutRowStretch(const QGridLayout *);
    static bool setGridLayoutRowStretch(const QString &, QGridLayout *);
    static void clearGridLayoutRowStretch(QGridLayout *);

    static QString gridLayoutColumnStretch(const QGridLayout *);
    static bool setGridLayoutColumnStretch(const QString &, QGridLayout *);
    static void clearGridLayoutColumnStretch(QGridLayout *);

    // return the row/column sizes as  comma-separated lists
    static QString gridLayoutRowMinimumHeight(const QGridLayout *);
    static bool setGridLayoutRowMinimumHeight(const QString &, QGridLayout *);
    static void clearGridLayoutRowMinimumHeight(QGridLayout *);

    static QString gridLayoutColumnMinimumWidth(const QGridLayout *);
    static bool setGridLayoutColumnMinimumWidth(const QString &, QGridLayout *);
    static void clearGridLayoutColumnMinimumWidth(QGridLayout *);

private:
    void clearResourceBuilder();
    void clearTextBuilder();

    typedef QHash<QLabel*, QString> BuddyHash;
    BuddyHash m_buddies;

#ifndef QT_FORMBUILDER_NO_SCRIPT
    QFormScriptRunner m_FormScriptRunner;
#endif

    QHash<QString, CustomWidgetData> m_customWidgetDataHash;

    ButtonGroupHash m_buttonGroups;

    bool m_layoutWidget;
    QResourceBuilder *m_resourceBuilder;
    QTextBuilder *m_textBuilder;

    QPointer<QWidget> m_parentWidget;
    bool m_parentWidgetIsSet;
};

void uiLibWarning(const QString &message);

// Struct with static accessor that provides most strings used in the form builder.
struct QDESIGNER_UILIB_EXPORT QFormBuilderStrings {
    QFormBuilderStrings();

    static const QFormBuilderStrings &instance();

    const QString buddyProperty;
    const QString cursorProperty;
    const QString objectNameProperty;
    const QString trueValue;
    const QString falseValue;
    const QString horizontalPostFix;
    const QString separator;
    const QString defaultTitle;
    const QString titleAttribute;
    const QString labelAttribute;
    const QString toolTipAttribute;
    const QString whatsThisAttribute;
    const QString flagsAttribute;
    const QString iconAttribute;
    const QString pixmapAttribute;
    const QString textAttribute;
    const QString currentIndexProperty;
    const QString toolBarAreaAttribute;
    const QString toolBarBreakAttribute;
    const QString dockWidgetAreaAttribute;
    const QString marginProperty;
    const QString spacingProperty;
    const QString leftMarginProperty;
    const QString topMarginProperty;
    const QString rightMarginProperty;
    const QString bottomMarginProperty;
    const QString horizontalSpacingProperty;
    const QString verticalSpacingProperty;
    const QString sizeHintProperty;
    const QString sizeTypeProperty;
    const QString orientationProperty;
    const QString styleSheetProperty;
    const QString qtHorizontal;
    const QString qtVertical;
    const QString currentRowProperty;
    const QString tabSpacingProperty;
    const QString qWidgetClass;
    const QString lineClass;
    const QString geometryProperty;
    const QString scriptWidgetVariable;
    const QString scriptChildWidgetsVariable;

    typedef QPair<Qt::ItemDataRole, QString> RoleNName;
    QList<RoleNName> itemRoles;
    QHash<QString, Qt::ItemDataRole> treeItemRoleHash;

    // first.first is primary role, first.second is shadow role.
    // Shadow is used for either the translation source or the designer
    // representation of the string value.
    typedef QPair<QPair<Qt::ItemDataRole, Qt::ItemDataRole>, QString> TextRoleNName;
    QList<TextRoleNName> itemTextRoles;
    QHash<QString, QPair<Qt::ItemDataRole, Qt::ItemDataRole> > treeItemTextRoleHash;
};
#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

QT_END_NAMESPACE

#endif // ABSTRACTFORMBUILDERPRIVATE_H
