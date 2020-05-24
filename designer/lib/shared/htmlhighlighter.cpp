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

#include <QtCore/QTextStream>

#include "htmlhighlighter_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

HtmlHighlighter::HtmlHighlighter(QTextEdit *textEdit)
    : QSyntaxHighlighter(textEdit)
{
    QTextCharFormat entityFormat;
    entityFormat.setForeground(Qt::red);
    setFormatFor(Entity, entityFormat);

    QTextCharFormat tagFormat;
    tagFormat.setForeground(Qt::darkMagenta);
    tagFormat.setFontWeight(QFont::Bold);
    setFormatFor(Tag, tagFormat);

    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);
    setFormatFor(Comment, commentFormat);

    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(Qt::black);
    attributeFormat.setFontWeight(QFont::Bold);
    setFormatFor(Attribute, attributeFormat);

    QTextCharFormat valueFormat;
    valueFormat.setForeground(Qt::blue);
    setFormatFor(Value, valueFormat);
}

void HtmlHighlighter::setFormatFor(Construct construct,
                                   const QTextCharFormat &format)
{
    m_formats[construct] = format;
    rehighlight();
}

void HtmlHighlighter::highlightBlock(const QString &text)
{
    static const QLatin1Char tab = QLatin1Char('\t');
    static const QLatin1Char space = QLatin1Char(' ');
    static const QLatin1Char amp = QLatin1Char('&');
    static const QLatin1Char startTag = QLatin1Char('<');
    static const QLatin1Char endTag = QLatin1Char('>');
    static const QLatin1Char quot = QLatin1Char('"');
    static const QLatin1Char apos = QLatin1Char('\'');
    static const QLatin1Char semicolon = QLatin1Char(';');
    static const QLatin1Char equals = QLatin1Char('=');
    static const QLatin1String startComment = QLatin1String("<!--");
    static const QLatin1String endComment = QLatin1String("-->");
    static const QLatin1String endElement = QLatin1String("/>");

    int state = previousBlockState();
    int len = text.length();
    int start = 0;
    int pos = 0;

    while (pos < len) {
        switch (state) {
        case NormalState:
        default:
            while (pos < len) {
                QChar ch = text.at(pos);
                if (ch == startTag) {
                    if (text.mid(pos, 4) == startComment) {
                        state = InComment;
                    } else {
                        state = InTag;
                        start = pos;
                        while (pos < len && text.at(pos) != space
                               && text.at(pos) != endTag
                               && text.at(pos) != tab
                               && text.mid(pos, 2) != endElement)
                            ++pos;
                        if (text.mid(pos, 2) == endElement)
                            ++pos;
                        setFormat(start, pos - start,
                                  m_formats[Tag]);
                        break;
                    }
                    break;
                } else if (ch == amp) {
                    start = pos;
                    while (pos < len && text.at(pos++) != semicolon)
                        ;
                    setFormat(start, pos - start,
                              m_formats[Entity]);
                } else {
                    // No tag, comment or entity started, continue...
                    ++pos;
                }
            }
            break;
        case InComment:
            start = pos;
            while (pos < len) {
                if (text.mid(pos, 3) == endComment) {
                    pos += 3;
                    state = NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Comment]);
            break;
        case InTag:
            QChar quote = QChar::Null;
            while (pos < len) {
                QChar ch = text.at(pos);
                if (quote.isNull()) {
                    start = pos;
                    if (ch == apos || ch == quot) {
                        quote = ch;
                    } else if (ch == endTag) {
                        ++pos;
                        setFormat(start, pos - start, m_formats[Tag]);
                        state = NormalState;
                        break;
                    } else if (text.mid(pos, 2) == endElement) {
                        pos += 2;
                        setFormat(start, pos - start, m_formats[Tag]);
                        state = NormalState;
                        break;
                    } else if (ch != space && text.at(pos) != tab) {
                        // Tag not ending, not a quote and no whitespace, so
                        // we must be dealing with an attribute.
                        ++pos;
                        while (pos < len && text.at(pos) != space
                               && text.at(pos) != tab
                               && text.at(pos) != equals)
                            ++pos;
                        setFormat(start, pos - start, m_formats[Attribute]);
                        start = pos;
                    }
                } else if (ch == quote) {
                    quote = QChar::Null;

                    // Anything quoted is a value
                    setFormat(start, pos - start, m_formats[Value]);
                }
                ++pos;
            }
            break;
        }
    }
    setCurrentBlockState(state);
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
