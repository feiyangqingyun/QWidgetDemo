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

#include "qscripthighlighter_p.h"

#include <QtCore/QSet>

QT_BEGIN_NAMESPACE

static const QSet<QString> &qscriptKeywords() {
    static QSet<QString> keywords;
    if (keywords.empty()) {
        keywords.insert(QLatin1String("Infinity"));
        keywords.insert(QLatin1String("NaN"));
        keywords.insert(QLatin1String("abstract"));
        keywords.insert(QLatin1String("boolean"));
        keywords.insert(QLatin1String("break"));
        keywords.insert(QLatin1String("byte"));
        keywords.insert(QLatin1String("case"));
        keywords.insert(QLatin1String("catch"));
        keywords.insert(QLatin1String("char"));
        keywords.insert(QLatin1String("class"));
        keywords.insert(QLatin1String("const"));
        keywords.insert(QLatin1String("constructor"));
        keywords.insert(QLatin1String("continue"));
        keywords.insert(QLatin1String("debugger"));
        keywords.insert(QLatin1String("default"));
        keywords.insert(QLatin1String("delete"));
        keywords.insert(QLatin1String("do"));
        keywords.insert(QLatin1String("double"));
        keywords.insert(QLatin1String("else"));
        keywords.insert(QLatin1String("enum"));
        keywords.insert(QLatin1String("export"));
        keywords.insert(QLatin1String("extends"));
        keywords.insert(QLatin1String("false"));
        keywords.insert(QLatin1String("final"));
        keywords.insert(QLatin1String("finally"));
        keywords.insert(QLatin1String("float"));
        keywords.insert(QLatin1String("for"));
        keywords.insert(QLatin1String("function"));
        keywords.insert(QLatin1String("goto"));
        keywords.insert(QLatin1String("if"));
        keywords.insert(QLatin1String("implements"));
        keywords.insert(QLatin1String("import"));
        keywords.insert(QLatin1String("in"));
        keywords.insert(QLatin1String("instanceof"));
        keywords.insert(QLatin1String("int"));
        keywords.insert(QLatin1String("interface"));
        keywords.insert(QLatin1String("long"));
        keywords.insert(QLatin1String("native"));
        keywords.insert(QLatin1String("new"));
        keywords.insert(QLatin1String("package"));
        keywords.insert(QLatin1String("private"));
        keywords.insert(QLatin1String("protected"));
        keywords.insert(QLatin1String("public"));
        keywords.insert(QLatin1String("return"));
        keywords.insert(QLatin1String("short"));
        keywords.insert(QLatin1String("static"));
        keywords.insert(QLatin1String("super"));
        keywords.insert(QLatin1String("switch"));
        keywords.insert(QLatin1String("synchronized"));
        keywords.insert(QLatin1String("this"));
        keywords.insert(QLatin1String("throw"));
        keywords.insert(QLatin1String("throws"));
        keywords.insert(QLatin1String("transient"));
        keywords.insert(QLatin1String("true"));
        keywords.insert(QLatin1String("try"));
        keywords.insert(QLatin1String("typeof"));
        keywords.insert(QLatin1String("undefined"));
        keywords.insert(QLatin1String("var"));
        keywords.insert(QLatin1String("void"));
        keywords.insert(QLatin1String("volatile"));
        keywords.insert(QLatin1String("while"));
        keywords.insert(QLatin1String("with"));    // end
    }
    return keywords;
}

static QSet<QChar> alphaChars() {
    QSet<QChar> rc;
    const QString alpha = QLatin1String("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    foreach (QChar chr, alpha)
        rc.insert(chr);
    return rc;
}

namespace qdesigner_internal {

QScriptHighlighter::QScriptHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    m_numberFormat.setForeground(Qt::blue);
    m_stringFormat.setForeground(Qt::darkGreen);
    m_typeFormat.setForeground(Qt::darkMagenta);
    m_keywordFormat.setForeground(Qt::darkYellow);
    m_labelFormat.setForeground(Qt::darkRed);
    m_commentFormat.setForeground(Qt::red);
    //m_commentFormat.setFontFamily("times");
    m_commentFormat.setFontItalic(true);
    m_preProcessorFormat.setForeground(Qt::darkBlue);
}

void QScriptHighlighter::highlightBlock(const QString &text)
{
    // states
    enum {
        StateStandard,
        StateCommentStart1,
        StateCCommentStart2,
        StateCppCommentStart2,
        StateCComment,
        StateCppComment,
        StateCCommentEnd1,
        StateCCommentEnd2,
        StateStringStart,
        StateString,
        StateStringEnd,
        StateString2Start,
        StateString2,
        StateString2End,
        StateNumber,
        StatePreProcessor,
        NumStates
    };
    // tokens
    enum {
        InputAlpha,
        InputNumber,
        InputAsterix,
        InputSlash,
        InputParen,
        InputSpace,
        InputHash,
        InputQuotation,
        InputApostrophe,
        InputSep,
        NumInputs
    };

    static const uchar table[NumStates][NumInputs] = {
        { StateStandard,      StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateStandard
        { StateStandard,      StateNumber,   StateCCommentStart2, StateCppCommentStart2, StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateCommentStart1
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCComment,         StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCCommentStart2
        { StateCppComment,    StateCppComment, StateCppComment,     StateCppComment,       StateCppComment, StateCppComment, StateCppComment,   StateCppComment,  StateCppComment,   StateCppComment }, // CppCommentStart2
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCComment,         StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCComment
        { StateCppComment,    StateCppComment, StateCppComment,     StateCppComment,       StateCppComment, StateCppComment, StateCppComment,   StateCppComment,  StateCppComment,   StateCppComment }, // StateCppComment
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCCommentEnd2,     StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCCommentEnd1
        { StateStandard,      StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateCCommentEnd2
        { StateString,        StateString,     StateString,         StateString,           StateString,     StateString,     StateString,       StateStringEnd,   StateString,       StateString }, // StateStringStart
        { StateString,        StateString,     StateString,         StateString,           StateString,     StateString,     StateString,       StateStringEnd,   StateString,       StateString }, // StateString
        { StateStandard,      StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateStringEnd
        { StateString2,       StateString2,    StateString2,        StateString2,          StateString2,    StateString2,    StateString2,      StateString2,     StateString2End,   StateString2 }, // StateString2Start
        { StateString2,       StateString2,    StateString2,        StateString2,          StateString2,    StateString2,    StateString2,      StateString2,     StateString2End,   StateString2 }, // StateString2
        { StateStandard,      StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateString2End
        { StateNumber,        StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateNumber
        { StatePreProcessor,  StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard } // StatePreProcessor
    };

    QString buffer;
    buffer.reserve(text.length());
    QTextCharFormat emptyFormat;

    int state = StateStandard;
    const int previousState = previousBlockState();
    if (previousState != -1)
        state = previousState;

    if (text.isEmpty()) {
        setCurrentBlockState(previousState);
        return;
    }

    int input = -1;
    int i = 0;
    bool lastWasBackSlash = false;
    bool makeLastStandard = false;

    static const QSet<QChar> alphabeth = alphaChars();
    static const QString mathChars = QString::fromLatin1("xXeE");
    static const QString numbers = QString::fromLatin1("0123456789");
    bool questionMark = false;
    QChar lastChar;
    QString firstWord;
    forever {
        const QChar c = text.at(i);

        if (lastWasBackSlash) {
            input = InputSep;
        } else {
            switch (c.toLatin1()) {
                case '*':
                    input = InputAsterix;
                    break;
                case '/':
                    input = InputSlash;
                    break;
                case '(': case '[': case '{':
                    input = InputParen;
                    if (state == StateStandard
                        || state == StateNumber
                        || state == StatePreProcessor
                        || state == StateCCommentEnd2
                        || state == StateCCommentEnd1
                        || state == StateString2End
                        || state == StateStringEnd
                       )
                        //blockData->parentheses << Parenthesis(Parenthesis::Open, c, i);
                    break;
                case ')': case ']': case '}':
                    input = InputParen;
                    if (state == StateStandard
                        || state == StateNumber
                        || state == StatePreProcessor
                        || state == StateCCommentEnd2
                        || state == StateCCommentEnd1
                        || state == StateString2End
                        || state == StateStringEnd
                       ) {
                        //blockData->parentheses << Parenthesis(Parenthesis::Closed, c, i);
                    }
                    break;
                case '#':
                    input = InputHash;
                    break;
                case '"':
                    input = InputQuotation;
                    break;
                case '\'':
                    input = InputApostrophe;
                    break;
                case ' ':
                    input = InputSpace;
                    break;
                case '1': case '2': case '3': case '4': case '5':
                case '6': case '7': case '8': case '9': case '0':
                    if (alphabeth.contains(lastChar)
                        && (!mathChars.contains(lastChar) || !numbers.contains(text.at(i - 1)))) {
                        input = InputAlpha;
                    } else {
                        if (input == InputAlpha && numbers.contains(lastChar))
                            input = InputAlpha;
                        else
                            input = InputNumber;
                    }
                    break;
                case ':': {
                              input = InputAlpha;
                              QChar nextChar = QLatin1Char(' ');
                              if (i < text.length() - 1)
                                  nextChar = text.at(i + 1);
                              if (state == StateStandard && !questionMark &&
                                  lastChar != QLatin1Char(':') && nextChar != QLatin1Char(':')) {
                                  for (int j = 0; j < i; ++j) {
                                      if (format(j) == emptyFormat)
                                          setFormat(j, 1, m_labelFormat);
                                  }
                              }
                              break;
                          }
                default: {
                             if (!questionMark && c == QLatin1Char('?'))
                                 questionMark = true;
                             if (c.isLetter() || c == QLatin1Char('_'))
                                 input = InputAlpha;
                             else
                                 input = InputSep;
                         } break;
            }
        }

        lastWasBackSlash = !lastWasBackSlash && c == QLatin1Char('\\');

        if (input == InputAlpha)
            buffer += c;

        state = table[state][input];

        switch (state) {
            case StateStandard: {
                                    setFormat(i, 1, emptyFormat);
                                    if (makeLastStandard)
                                        setFormat(i - 1, 1, emptyFormat);
                                    makeLastStandard = false;
                                    if (!buffer.isEmpty() && input != InputAlpha ) {
                                        highlightKeyword(i, buffer);
                                        buffer.clear();
                                    }
                                } break;
            case StateCommentStart1:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = true;
                                buffer.resize(0);
                                break;
            case StateCCommentStart2:
                                setFormat(i - 1, 2, m_commentFormat);
                                makeLastStandard = false;
                                buffer.resize(0);
                                break;
            case StateCppCommentStart2:
                                setFormat(i - 1, 2, m_commentFormat);
                                makeLastStandard = false;
                                buffer.resize(0);
                                break;
            case StateCComment:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateCppComment:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateCCommentEnd1:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateCCommentEnd2:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateStringStart:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateString:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_stringFormat);
                                buffer.resize(0);
                                break;
            case StateStringEnd:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateString2Start:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateString2:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_stringFormat);
                                buffer.resize(0);
                                break;
            case StateString2End:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateNumber:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat( i, 1, m_numberFormat);
                                buffer.resize(0);
                                break;
            case StatePreProcessor:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_preProcessorFormat);
                                buffer.resize(0);
                                break;
        }

        lastChar = c;
        i++;
        if (i >= text.length())
            break;
    }

    highlightKeyword(text.length(), buffer);

    if (state == StateCComment
        || state == StateCCommentEnd1
        || state == StateCCommentStart2
       ) {
        state = StateCComment;
    } else if (state == StateString) {
        state = StateString;
    } else if (state == StateString2) {
        state =  StateString2;
    } else {
        state = StateStandard;
    }

    setCurrentBlockState(state);
}

void QScriptHighlighter::highlightKeyword(int currentPos, const QString &buffer)
{
    if (buffer.isEmpty())
        return;

    if (buffer.at(0) == QLatin1Char('Q')) {
        setFormat(currentPos - buffer.length(), buffer.length(), m_typeFormat);
    } else {
        if (qscriptKeywords().contains(buffer)) {
            setFormat(currentPos - buffer.length(), buffer.length(), m_keywordFormat);
        }
    }
}
}

QT_END_NAMESPACE
