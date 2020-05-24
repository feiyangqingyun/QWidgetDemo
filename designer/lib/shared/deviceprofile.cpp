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

#include "deviceprofile_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <widgetfactory_p.h>
#include <qdesigner_utils_p.h>

#include <QtGui/QApplication>
#include <QtGui/QFont>
#include <QtGui/QDesktopWidget>
#include <QtGui/QStyle>
#include <QtGui/QStyleFactory>
#include <QtGui/QApplication>

#include <QtCore/QSharedData>
#include <QtCore/QTextStream>

#include <QtCore/QXmlStreamWriter>
#include <QtCore/QXmlStreamReader>


static const char *dpiXPropertyC = "_q_customDpiX";
static const char *dpiYPropertyC = "_q_customDpiY";

// XML serialization
static const char *xmlVersionC="1.0";
static const char *rootElementC="deviceprofile";
static const char *nameElementC = "name";
static const char *fontFamilyElementC = "fontfamily";
static const char *fontPointSizeElementC = "fontpointsize";
static const char *dPIXElementC = "dpix";
static const char *dPIYElementC = "dpiy";
static const char *styleElementC = "style";

/* DeviceProfile:
 * For preview purposes (preview, widget box, new form dialog), the
 * QDesignerFormBuilder applies the settings to the form main container
 * (Point being here that the DPI must be set directly for size calculations
 * to be correct).
 * For editing purposes, FormWindow applies the settings to the form container
 * as not to interfere with the font settings of the form main container.
 * In addition, the widgetfactory maintains the system settings style
 * and applies it when creating widgets. */

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// ---------------- DeviceProfileData
class DeviceProfileData : public QSharedData {
public:
    DeviceProfileData();
    void fromSystem();
    void clear();

    QString m_fontFamily;
    int m_fontPointSize;
    QString m_style;
    int m_dpiX;
    int m_dpiY;
    QString m_name;
};

DeviceProfileData::DeviceProfileData()  :
   m_fontPointSize(-1),
   m_dpiX(-1),
   m_dpiY(-1)
{
}

void DeviceProfileData::clear()
{
    m_fontPointSize = -1;
    m_dpiX = 0;
    m_dpiY = 0;
    m_name.clear();
    m_style.clear();
}

void DeviceProfileData::fromSystem()
{
    const QFont appFont = QApplication::font();
    m_fontFamily = appFont.family();
    m_fontPointSize = appFont.pointSize();
    DeviceProfile::systemResolution(&m_dpiX, &m_dpiY);
    m_style.clear();
}

// ---------------- DeviceProfile
DeviceProfile::DeviceProfile() :
   m_d(new DeviceProfileData)
{
}

DeviceProfile::DeviceProfile(const DeviceProfile &o) :
    m_d(o.m_d)

{
}

DeviceProfile& DeviceProfile::operator=(const DeviceProfile &o)
{
    m_d.operator=(o.m_d);
    return *this;
}

DeviceProfile::~DeviceProfile()
{
}

void DeviceProfile::clear()
{
    m_d->clear();
}

bool DeviceProfile::isEmpty() const
{
    return m_d->m_name.isEmpty();
}

QString DeviceProfile::fontFamily() const
{
     return m_d->m_fontFamily;
}

void DeviceProfile::setFontFamily(const QString &f)
{
     m_d->m_fontFamily = f;
}

int DeviceProfile::fontPointSize() const
{
     return m_d->m_fontPointSize;
}

void DeviceProfile::setFontPointSize(int p)
{
     m_d->m_fontPointSize = p;
}

QString DeviceProfile::style() const
{
    return m_d->m_style;
}

void DeviceProfile::setStyle(const QString &s)
{
    m_d->m_style = s;
}

int DeviceProfile::dpiX() const
{
     return m_d->m_dpiX;
}

void DeviceProfile::setDpiX(int d)
{
     m_d->m_dpiX = d;
}

int DeviceProfile::dpiY() const
{
     return m_d->m_dpiY;
}

void DeviceProfile::setDpiY(int d)
{
     m_d->m_dpiY = d;
}

void DeviceProfile::fromSystem()
{
    m_d->fromSystem();
}

QString DeviceProfile::name() const
{
    return m_d->m_name;
}

void DeviceProfile::setName(const QString &n)
{
    m_d->m_name = n;
}

void DeviceProfile::systemResolution(int *dpiX, int *dpiY)
{
    const QDesktopWidget *dw = qApp->desktop();
    *dpiX = dw->logicalDpiX();
    *dpiY = dw->logicalDpiY();
}

class FriendlyWidget : public QWidget {
    friend class DeviceProfile;
};

void DeviceProfile::widgetResolution(const QWidget *w, int *dpiX, int *dpiY)
{
    const FriendlyWidget *fw = static_cast<const FriendlyWidget*>(w);
    *dpiX = fw->metric(QPaintDevice::PdmDpiX);
    *dpiY = fw->metric(QPaintDevice::PdmDpiY);
}

QString DeviceProfile::toString() const
{
    const DeviceProfileData &d = *m_d;
    QString rc;
    QTextStream(&rc) << "DeviceProfile:name=" << d.m_name << " Font=" << d.m_fontFamily << ' '
        << d.m_fontPointSize << " Style=" << d.m_style << " DPI=" << d.m_dpiX << ',' << d.m_dpiY;
    return rc;
}

// Apply font to widget
static void applyFont(const QString &family, int size, DeviceProfile::ApplyMode am, QWidget *widget)
{
    QFont currentFont = widget->font();
    if (currentFont.pointSize() == size && currentFont.family() == family)
        return;
    switch (am) {
    case DeviceProfile::ApplyFormParent:
        // Invisible form parent: Apply all
        widget->setFont(QFont(family, size));
        break;
    case DeviceProfile::ApplyPreview: {
        // Preview: Apply only subproperties that have not been changed by designer properties
        bool apply = false;
        const uint resolve = currentFont.resolve();
        if (!(resolve & QFont::FamilyResolved)) {
            currentFont.setFamily(family);
            apply = true;
        }
        if (!(resolve & QFont::SizeResolved)) {
            currentFont.setPointSize(size);
            apply = true;
        }
        if (apply)
            widget->setFont(currentFont);
    }
        break;
    }
}

void DeviceProfile::applyDPI(int dpiX, int dpiY, QWidget *widget)
{
    int sysDPIX, sysDPIY; // Set dynamic variables in case values are different from system DPI
    systemResolution(&sysDPIX, &sysDPIY);
    if (dpiX != sysDPIX && dpiY != sysDPIY) {
        widget->setProperty(dpiXPropertyC, QVariant(dpiX));
        widget->setProperty(dpiYPropertyC, QVariant(dpiY));
    }
}

void DeviceProfile::apply(const QDesignerFormEditorInterface *core, QWidget *widget, ApplyMode am) const
{
    if (isEmpty())
        return;

    const DeviceProfileData &d = *m_d;

    if (!d.m_fontFamily.isEmpty())
        applyFont(d.m_fontFamily, d.m_fontPointSize, am, widget);

    applyDPI(d.m_dpiX, d.m_dpiY, widget);

    if (!d.m_style.isEmpty()) {
        if (WidgetFactory *wf = qobject_cast<qdesigner_internal::WidgetFactory *>(core->widgetFactory()))
            wf->applyStyleTopLevel(d.m_style, widget);
    }
}

bool DeviceProfile::equals(const DeviceProfile& rhs) const
{
    const DeviceProfileData &d = *m_d;
    const DeviceProfileData &rhs_d = *rhs.m_d;
    return d.m_fontPointSize == rhs_d.m_fontPointSize &&
           d.m_dpiX == rhs_d.m_dpiX && d.m_dpiY == rhs_d.m_dpiY && d.m_fontFamily == rhs_d.m_fontFamily &&
           d.m_style == rhs_d.m_style && d.m_name == rhs_d.m_name;
}

static inline void writeElement(QXmlStreamWriter &writer, const QString &element, const QString &cdata)
{
    writer.writeStartElement(element);
    writer.writeCharacters(cdata);
    writer.writeEndElement();
}

QString DeviceProfile::toXml() const
{
    const DeviceProfileData &d = *m_d;
    QString rc;
    QXmlStreamWriter writer(&rc);
    writer.writeStartDocument(QLatin1String(xmlVersionC));
    writer.writeStartElement(QLatin1String(rootElementC));
    writeElement(writer, QLatin1String(nameElementC), d.m_name);

    if (!d.m_fontFamily.isEmpty())
        writeElement(writer, QLatin1String(fontFamilyElementC), d.m_fontFamily);
    if (d.m_fontPointSize >= 0)
        writeElement(writer, QLatin1String(fontPointSizeElementC), QString::number(d.m_fontPointSize));
    if (d.m_dpiX > 0)
        writeElement(writer, QLatin1String(dPIXElementC), QString::number(d.m_dpiX));
    if (d.m_dpiY > 0)
        writeElement(writer, QLatin1String(dPIYElementC), QString::number(d.m_dpiY));
    if (!d.m_style.isEmpty())
        writeElement(writer, QLatin1String(styleElementC), d.m_style);

    writer.writeEndElement();
    writer.writeEndDocument();
    return rc;
}

/* Switch stages when encountering a start element (state table) */
enum ParseStage { ParseBeginning, ParseWithinRoot,
                  ParseName, ParseFontFamily, ParseFontPointSize, ParseDPIX,  ParseDPIY,  ParseStyle,
                  ParseError };

static ParseStage nextStage(ParseStage currentStage, const QStringRef &startElement)
{
    switch (currentStage) {
    case ParseBeginning:
        if (startElement == QLatin1String(rootElementC))
            return ParseWithinRoot;
        break;
    case ParseWithinRoot:
    case ParseName:
    case ParseFontFamily:
    case ParseFontPointSize:
    case ParseDPIX:
    case ParseDPIY:
    case ParseStyle:
        if (startElement == QLatin1String(nameElementC))
            return ParseName;
        if (startElement == QLatin1String(fontFamilyElementC))
            return ParseFontFamily;
        if (startElement == QLatin1String(fontPointSizeElementC))
            return ParseFontPointSize;
        if (startElement == QLatin1String(dPIXElementC))
            return ParseDPIX;
        if (startElement == QLatin1String(dPIYElementC))
            return ParseDPIY;
        if (startElement == QLatin1String(styleElementC))
            return ParseStyle;
        break;
    case ParseError:
        break;
    }
    return ParseError;
}

static bool readIntegerElement(QXmlStreamReader &reader, int *v)
{
    const QString e = reader.readElementText();
    bool ok;
    *v = e.toInt(&ok);
    //: Reading a number for an embedded device profile
    if (!ok)
        reader.raiseError(QApplication::translate("DeviceProfile", "'%1' is not a number.").arg(e));
    return ok;
}

bool DeviceProfile::fromXml(const QString &xml, QString *errorMessage)
{
    DeviceProfileData &d = *m_d;
    d.fromSystem();

    QXmlStreamReader reader(xml);

    ParseStage ps = ParseBeginning;
    QXmlStreamReader::TokenType tt = QXmlStreamReader::NoToken;
    int iv = 0;
    do {
        tt = reader.readNext();
        if (tt == QXmlStreamReader::StartElement) {
            ps = nextStage(ps, reader.name());
            switch (ps) {
            case ParseBeginning:
            case ParseWithinRoot:
                break;
            case ParseError:
                reader.raiseError(QApplication::translate("DeviceProfile", "An invalid tag <%1> was encountered.").arg(reader.name().toString()));
                tt = QXmlStreamReader::Invalid;
                break;
            case ParseName:
                d.m_name = reader.readElementText();
                break;
            case ParseFontFamily:
                d.m_fontFamily = reader.readElementText();
                break;
            case ParseFontPointSize:
                if (readIntegerElement(reader, &iv)) {
                    d.m_fontPointSize = iv;
                } else {
                    tt = QXmlStreamReader::Invalid;
                }
                break;
            case ParseDPIX:
                if (readIntegerElement(reader, &iv)) {
                    d.m_dpiX = iv;
                } else {
                    tt = QXmlStreamReader::Invalid;
                }
                break;
            case ParseDPIY:
                if (readIntegerElement(reader, &iv)) {
                    d.m_dpiY = iv;
                } else {
                    tt = QXmlStreamReader::Invalid;
                }
                break;
            case ParseStyle:
                d.m_style = reader.readElementText();
                break;
            }
        }
    } while (tt != QXmlStreamReader::Invalid && tt != QXmlStreamReader::EndDocument);

    if (reader.hasError()) {
        *errorMessage = reader.errorString();
        return false;
    }

    return true;
}
}

QT_END_NAMESPACE

