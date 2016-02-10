/*
  This file is part of the Ofi Labs X2 project.

  Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "glsledit.h"

#include <QtWidgets>

class GLSLBlockData: public QTextBlockUserData
{
public:
    QList<int> bracketPositions;
};

class GLSLHighlighter : public QSyntaxHighlighter
{
public:
    GLSLHighlighter(QTextDocument *parent = 0);
    void setColor(GLSLEdit::ColorComponent component, const QColor &color);
    void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);

protected:
    void highlightBlock(const QString &text);

private:
    QSet<QString> m_keywords;
    QSet<QString> m_types;
    QSet<QString> m_builtins;
    QHash<GLSLEdit::ColorComponent, QColor> m_colors;
    QString m_markString;
    Qt::CaseSensitivity m_markCaseSensitivity;
};

GLSLHighlighter::GLSLHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
    , m_markCaseSensitivity(Qt::CaseInsensitive)
{
    // default color scheme
    m_colors[GLSLEdit::Normal]     = QColor("#000000");
    m_colors[GLSLEdit::Comment]    = QColor("#808080");
    m_colors[GLSLEdit::Number]     = QColor("#008000");
    m_colors[GLSLEdit::String]     = QColor("#800000");
    m_colors[GLSLEdit::Operator]   = QColor("#808000");
    m_colors[GLSLEdit::Identifier] = QColor("#000020");
    m_colors[GLSLEdit::Keyword]    = QColor("#000080");
    m_colors[GLSLEdit::BuiltIn]    = QColor("#008080");
    m_colors[GLSLEdit::Marker]     = QColor("#ffff00");

    m_keywords << "attribute";
    m_keywords << "const";
    m_keywords << "uniform";
    m_keywords << "varying";
    m_keywords << "layout";
    m_keywords << "centroid";
    m_keywords << "flat";
    m_keywords << "smooth";
    m_keywords << "noperspective";
    m_keywords << "patch";
    m_keywords << "sample";
    m_keywords << "break";
    m_keywords << "continue";
    m_keywords << "do";
    m_keywords << "for";
    m_keywords << "while";
    m_keywords << "switch";
    m_keywords << "case";
    m_keywords << "default";
    m_keywords << "if";
    m_keywords << "else";
    m_keywords << "subroutine";
    m_keywords << "in";
    m_keywords << "out";
    m_keywords << "inout";
    m_keywords << "true";
    m_keywords << "false";
    m_keywords << "invariant";
    m_keywords << "discard";
    m_keywords << "return";
    m_keywords << "lowp";
    m_keywords << "mediump";
    m_keywords << "highp";
    m_keywords << "precision";
    m_keywords << "struct";

    m_types << "float";
    m_types << "double";
    m_types << "int";
    m_types << "void";
    m_types << "bool";
    m_types << "mat2";
    m_types << "mat3";
    m_types << "mat4";
    m_types << "dmat2";
    m_types << "dmat3";
    m_types << "dmat4";
    m_types << "mat2x2";
    m_types << "mat2x3";
    m_types << "mat2x4";
    m_types << "dmat2x2";
    m_types << "dmat2x3";
    m_types << "dmat2x4";
    m_types << "mat3x2";
    m_types << "mat3x3";
    m_types << "mat3x4";
    m_types << "dmat3x2";
    m_types << "dmat3x3";
    m_types << "dmat3x4";
    m_types << "mat4x2";
    m_types << "mat4x3";
    m_types << "mat4x4";
    m_types << "dmat4x2";
    m_types << "dmat4x3";
    m_types << "dmat4x4";
    m_types << "vec2";
    m_types << "vec3";
    m_types << "vec4";
    m_types << "ivec2";
    m_types << "ivec3";
    m_types << "ivec4";
    m_types << "bvec2";
    m_types << "bvec3";
    m_types << "bvec4";
    m_types << "dvec2";
    m_types << "dvec3";
    m_types << "dvec4";
    m_types << "uint";
    m_types << "uvec2";
    m_types << "uvec3";
    m_types << "uvec4";
    m_types << "sampler1D";
    m_types << "sampler2D";
    m_types << "sampler3D";
    m_types << "samplerCube";
    m_types << "sampler1DShadow";
    m_types << "sampler2DShadow";
    m_types << "samplerCubeShadow";
    m_types << "sampler1DArray";
    m_types << "sampler2DArray";
    m_types << "sampler1DArrayShadow";
    m_types << "sampler2DArrayShadow";
    m_types << "isampler1D";
    m_types << "isampler2D";
    m_types << "isampler3D";
    m_types << "isamplerCube";
    m_types << "isampler1DArray";
    m_types << "isampler2DArray";
    m_types << "usampler1D";
    m_types << "usampler2D";
    m_types << "usampler3D";
    m_types << "usamplerCube";
    m_types << "usampler1DArray";
    m_types << "usampler2DArray";
    m_types << "sampler2DRect";
    m_types << "sampler2DRectShadow";
    m_types << "isampler2DRect";
    m_types << "usampler2DRect";
    m_types << "samplerBuffer";
    m_types << "isamplerBuffer";
    m_types << "usamplerBuffer";
    m_types << "sampler2DMS";
    m_types << "isampler2DMS";
    m_types << "usampler2DMS";
    m_types << "sampler2DMSArray";
    m_types << "isampler2DMSArray";
    m_types << "usampler2DMSArray";
    m_types << "samplerCubeArray";
    m_types << "samplerCubeArrayShadow";
    m_types << "isamplerCubeArray";
    m_types << "usamplerCubeArray";
}

void GLSLHighlighter::setColor(GLSLEdit::ColorComponent component, const QColor &color)
{
    m_colors[component] = color;
    rehighlight();
}

void GLSLHighlighter::highlightBlock(const QString &text)
{
    // parsing state
    enum {
        Start = 0,
        Number = 1,
        Identifier = 2,
        String = 3,
        Comment = 4,
        Regex = 5
    };

    QList<int> bracketPositions;

    int blockState = previousBlockState();
    int bracketLevel = blockState >> 4;
    int state = blockState & 15;
    if (blockState < 0) {
        bracketLevel = 0;
        state = Start;
    }

    int start = 0;
    int i = 0;
    while (i <= text.length()) {
        QChar ch = (i < text.length()) ? text.at(i) : QChar();
        QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();

        switch (state) {

        case Start:
            start = i;
            if (ch.isSpace()) {
                ++i;
            } else if (ch.isDigit()) {
                ++i;
                state = Number;
            } else if (ch.isLetter() || ch == '_') {
                ++i;
                state = Identifier;
            } else if (ch == '\'' || ch == '\"') {
                ++i;
                state = String;
            } else if (ch == '/' && next == '*') {
                ++i;
                ++i;
                state = Comment;
            } else if (ch == '/' && next == '/') {
                i = text.length();
                setFormat(start, text.length(), m_colors[GLSLEdit::Comment]);
            } else if (ch == '/' && next != '*') {
                ++i;
                state = Regex;
            } else {
                if (!QString("(){}[]").contains(ch))
                    setFormat(start, 1, m_colors[GLSLEdit::Operator]);
                if (ch =='{' || ch == '}') {
                    bracketPositions += i;
                    if (ch == '{')
                        bracketLevel++;
                    else
                        bracketLevel--;
                }
                ++i;
                state = Start;
            }
            break;

        case Number:
            if (ch.isSpace() || !ch.isDigit()) {
                setFormat(start, i - start, m_colors[GLSLEdit::Number]);
                state = Start;
            } else {
                ++i;
            }
            break;

        case Identifier:
            if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_')) {
                QString token = text.mid(start, i - start).trimmed();
                if (m_keywords.contains(token))
                    setFormat(start, i - start, m_colors[GLSLEdit::Keyword]);
                else if (m_types.contains(token) || m_builtins.contains(token))
                    setFormat(start, i - start, m_colors[GLSLEdit::BuiltIn]);
                state = Start;
            } else {
                ++i;
            }
            break;

        case String:
            if (ch == text.at(start)) {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (prev != '\\') {
                    ++i;
                    setFormat(start, i - start, m_colors[GLSLEdit::String]);
                    state = Start;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }
            break;

        case Comment:
            if (ch == '*' && next == '/') {
                ++i;
                ++i;
                setFormat(start, i - start, m_colors[GLSLEdit::Comment]);
                state = Start;
            } else {
                ++i;
            }
            break;

        case Regex:
            if (ch == '/') {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (prev != '\\') {
                    ++i;
                    setFormat(start, i - start, m_colors[GLSLEdit::String]);
                    state = Start;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }
            break;

        default:
            state = Start;
            break;
        }
    }

    if (state == Comment)
        setFormat(start, text.length(), m_colors[GLSLEdit::Comment]);
    else
        state = Start;

    if (!m_markString.isEmpty()) {
        int pos = 0;
        int len = m_markString.length();
        QTextCharFormat markerFormat;
        markerFormat.setBackground(m_colors[GLSLEdit::Marker]);
        markerFormat.setForeground(m_colors[GLSLEdit::Normal]);
        for (;;) {
            pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
            if (pos < 0)
                break;
            setFormat(pos, len, markerFormat);
            ++pos;
        }
    }

    if (!bracketPositions.isEmpty()) {
        GLSLBlockData *blockData = reinterpret_cast<GLSLBlockData*>(currentBlock().userData());
        if (!blockData) {
            blockData = new GLSLBlockData;
            currentBlock().setUserData(blockData);
        }
        blockData->bracketPositions = bracketPositions;
    }

    blockState = (state & 15) | (bracketLevel << 4);
    setCurrentBlockState(blockState);
}

void GLSLHighlighter::mark(const QString &str, Qt::CaseSensitivity caseSensitivity)
{
    m_markString = str;
    m_markCaseSensitivity = caseSensitivity;
    rehighlight();
}

struct BlockInfo {
    int position;
    int number;
    bool foldable: 1;
    bool folded : 1;
};

Q_DECLARE_TYPEINFO(BlockInfo, Q_PRIMITIVE_TYPE);

class SidebarWidget : public QWidget
{
public:
    SidebarWidget(GLSLEdit *editor);
    QVector<BlockInfo> lineNumbers;
    QColor backgroundColor;
    QColor lineNumberColor;
    QColor indicatorColor;
    QColor foldIndicatorColor;
    QFont font;
    int foldIndicatorWidth;
    QPixmap rightArrowIcon;
    QPixmap downArrowIcon;
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
};

SidebarWidget::SidebarWidget(GLSLEdit *editor)
    : QWidget(editor)
    , foldIndicatorWidth(0)
{
    backgroundColor = Qt::lightGray;
    lineNumberColor = Qt::black;
    indicatorColor = Qt::white;
    foldIndicatorColor = Qt::lightGray;
}

void SidebarWidget::mousePressEvent(QMouseEvent *event)
{
    if (foldIndicatorWidth > 0) {
        int xofs = width() - foldIndicatorWidth;
        int lineNo = -1;
        int fh = fontMetrics().lineSpacing();
        int ys = event->pos().y();
        if (event->pos().x() > xofs) {
            foreach (BlockInfo ln, lineNumbers)
                if (ln.position < ys && (ln.position + fh) > ys) {
                    if (ln.foldable)
                        lineNo = ln.number;
                    break;
                }
        }
        if (lineNo >= 0) {
            GLSLEdit *editor = qobject_cast<GLSLEdit*>(parent());
            if (editor)
                editor->toggleFold(lineNo);
        }
    }
}

void SidebarWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(event->rect(), backgroundColor);
    p.setPen(lineNumberColor);
    p.setFont(font);
    int fh = QFontMetrics(font).height();
    foreach (BlockInfo ln, lineNumbers)
        p.drawText(0, ln.position, width() - 4 - foldIndicatorWidth, fh, Qt::AlignRight, QString::number(ln.number));

    if (foldIndicatorWidth > 0) {
        int xofs = width() - foldIndicatorWidth;
        p.fillRect(xofs, 0, foldIndicatorWidth, height(), indicatorColor);

        // initialize (or recreate) the arrow icons whenever necessary
        if (foldIndicatorWidth != rightArrowIcon.width()) {
            QPainter iconPainter;
            QPolygonF polygon;

            int dim = foldIndicatorWidth;
            rightArrowIcon = QPixmap(dim, dim);
            rightArrowIcon.fill(Qt::transparent);
            downArrowIcon = rightArrowIcon;

            polygon << QPointF(dim * 0.4, dim * 0.25);
            polygon << QPointF(dim * 0.4, dim * 0.75);
            polygon << QPointF(dim * 0.8, dim * 0.5);
            iconPainter.begin(&rightArrowIcon);
            iconPainter.setRenderHint(QPainter::Antialiasing);
            iconPainter.setPen(Qt::NoPen);
            iconPainter.setBrush(foldIndicatorColor);
            iconPainter.drawPolygon(polygon);
            iconPainter.end();

            polygon.clear();
            polygon << QPointF(dim * 0.25, dim * 0.4);
            polygon << QPointF(dim * 0.75, dim * 0.4);
            polygon << QPointF(dim * 0.5, dim * 0.8);
            iconPainter.begin(&downArrowIcon);
            iconPainter.setRenderHint(QPainter::Antialiasing);
            iconPainter.setPen(Qt::NoPen);
            iconPainter.setBrush(foldIndicatorColor);
            iconPainter.drawPolygon(polygon);
            iconPainter.end();
        }

        foreach (BlockInfo ln, lineNumbers)
            if (ln.foldable) {
                if (ln.folded)
                    p.drawPixmap(xofs, ln.position, rightArrowIcon);
                else
                    p.drawPixmap(xofs, ln.position, downArrowIcon);
            }
    }
}

static int findClosingMatch(const QTextDocument *doc, int cursorPosition)
{
    QTextBlock block = doc->findBlock(cursorPosition);
    GLSLBlockData *blockData = reinterpret_cast<GLSLBlockData*>(block.userData());
    if (!blockData->bracketPositions.isEmpty()) {
        int depth = 1;
        while (block.isValid()) {
            blockData = reinterpret_cast<GLSLBlockData*>(block.userData());
            if (blockData && !blockData->bracketPositions.isEmpty()) {
                for (int c = 0; c < blockData->bracketPositions.count(); ++c) {
                    int absPos = block.position() + blockData->bracketPositions.at(c);
                    if (absPos <= cursorPosition)
                        continue;
                    if (doc->characterAt(absPos) == '{')
                        depth++;
                    else
                        depth--;
                    if (depth == 0)
                        return absPos;
                }
            }
            block = block.next();
        }
    }
    return -1;
}

static int findOpeningMatch(const QTextDocument *doc, int cursorPosition)
{
    QTextBlock block = doc->findBlock(cursorPosition);
    GLSLBlockData *blockData = reinterpret_cast<GLSLBlockData*>(block.userData());
    if (!blockData->bracketPositions.isEmpty()) {
        int depth = 1;
        while (block.isValid()) {
            blockData = reinterpret_cast<GLSLBlockData*>(block.userData());
            if (blockData && !blockData->bracketPositions.isEmpty()) {
                for (int c = blockData->bracketPositions.count() - 1; c >= 0; --c) {
                    int absPos = block.position() + blockData->bracketPositions.at(c);
                    if (absPos >= cursorPosition - 1)
                        continue;
                    if (doc->characterAt(absPos) == '}')
                        depth++;
                    else
                        depth--;
                    if (depth == 0)
                        return absPos;
                }
            }
            block = block.previous();
        }
    }
    return -1;
}

class GLSLDocLayout: public QPlainTextDocumentLayout
{
public:
    GLSLDocLayout(QTextDocument *doc);
    void forceUpdate();
};

GLSLDocLayout::GLSLDocLayout(QTextDocument *doc)
    : QPlainTextDocumentLayout(doc)
{
}

void GLSLDocLayout::forceUpdate()
{
    emit documentSizeChanged(documentSize());
}

class GLSLEditPrivate
{
public:
    GLSLEdit *editor;
    GLSLDocLayout *layout;
    GLSLHighlighter *highlighter;
    SidebarWidget *sidebar;
    bool showLineNumbers;
    bool textWrap;
    QColor cursorColor;
    bool bracketsMatching;
    QList<int> matchPositions;
    QColor bracketMatchColor;
    QList<int> errorPositions;
    QColor bracketErrorColor;
    bool codeFolding : 1;
};

GLSLEdit::GLSLEdit(QWidget *parent)
    : QPlainTextEdit(parent)
    , d_ptr(new GLSLEditPrivate)
{
    d_ptr->editor = this;
    d_ptr->layout = new GLSLDocLayout(document());
    d_ptr->highlighter = new GLSLHighlighter(document());
    d_ptr->sidebar = new SidebarWidget(this);
    d_ptr->showLineNumbers = true;
    d_ptr->textWrap = true;
    d_ptr->bracketsMatching = true;
    d_ptr->cursorColor = QColor(255, 255, 192);
    d_ptr->bracketMatchColor = QColor(180, 238, 180);
    d_ptr->bracketErrorColor = QColor(224, 128, 128);
    d_ptr->codeFolding = true;

    document()->setDocumentLayout(d_ptr->layout);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursor()));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSidebar()));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateSidebar(QRect, int)));

#if defined(Q_OS_MAC)
    QFont textFont = font();
    textFont.setPointSize(12);
    textFont.setFamily("Monaco");
    setFont(textFont);
#elif defined(Q_OS_UNIX)
    QFont textFont = font();
    textFont.setFamily("Monospace");
    setFont(textFont);
#endif
}

GLSLEdit::~GLSLEdit()
{
    delete d_ptr->layout;
}

void GLSLEdit::setColor(ColorComponent component, const QColor &color)
{
    Q_D(GLSLEdit);

    if (component == Background) {
        QPalette pal = palette();
        pal.setColor(QPalette::Base, color);
        setPalette(pal);
        d->sidebar->indicatorColor = color;
        updateSidebar();
    } else if (component == Normal) {
        QPalette pal = palette();
        pal.setColor(QPalette::Text, color);
        setPalette(pal);
    } else if (component == Sidebar) {
        d->sidebar->backgroundColor = color;
        updateSidebar();
    } else if (component == LineNumber) {
        d->sidebar->lineNumberColor = color;
        updateSidebar();
    } else if (component == Cursor) {
        d->cursorColor = color;
        updateCursor();
    } else if (component == BracketMatch) {
        d->bracketMatchColor = color;
        updateCursor();
    } else if (component == BracketError) {
        d->bracketErrorColor = color;
        updateCursor();
    } else if (component == FoldIndicator) {
        d->sidebar->foldIndicatorColor = color;
        updateSidebar();
    } else {
        d->highlighter->setColor(component, color);
        updateCursor();
    }
}

bool GLSLEdit::isLineNumbersVisible() const
{
    return d_ptr->showLineNumbers;
}

void GLSLEdit::setLineNumbersVisible(bool visible)
{
    d_ptr->showLineNumbers = visible;
    updateSidebar();
}

bool GLSLEdit::isTextWrapEnabled() const
{
    return d_ptr->textWrap;
}

void GLSLEdit::setTextWrapEnabled(bool enable)
{
    d_ptr->textWrap = enable;
    setLineWrapMode(enable ? WidgetWidth : NoWrap);
}

bool GLSLEdit::isBracketsMatchingEnabled() const
{
    return d_ptr->bracketsMatching;
}

void GLSLEdit::setBracketsMatchingEnabled(bool enable)
{
    d_ptr->bracketsMatching = enable;
    updateCursor();
}

bool GLSLEdit::isCodeFoldingEnabled() const
{
    return d_ptr->codeFolding;
}

void GLSLEdit::setCodeFoldingEnabled(bool enable)
{
    d_ptr->codeFolding = enable;
    updateSidebar();
}

static int findClosingConstruct(const QTextBlock &block)
{
    if (!block.isValid())
        return -1;
    GLSLBlockData *blockData = reinterpret_cast<GLSLBlockData*>(block.userData());
    if (!blockData)
        return -1;
    if (blockData->bracketPositions.isEmpty())
        return -1;
    const QTextDocument *doc = block.document();
    int offset = block.position();
    foreach (int pos, blockData->bracketPositions) {
        int absPos = offset + pos;
        if (doc->characterAt(absPos) == '{') {
            int matchPos = findClosingMatch(doc, absPos);
            if (matchPos >= 0)
                return matchPos;
        }
    }
    return -1;
}

bool GLSLEdit::isFoldable(int line) const
{
    int matchPos = findClosingConstruct(document()->findBlockByNumber(line - 1));
    if (matchPos >= 0) {
        QTextBlock matchBlock = document()->findBlock(matchPos);
        if (matchBlock.isValid() && matchBlock.blockNumber() > line)
            return true;
    }
    return false;
}

bool GLSLEdit::isFolded(int line) const
{
    QTextBlock block = document()->findBlockByNumber(line - 1);
    if (!block.isValid())
        return false;
    block = block.next();
    if (!block.isValid())
        return false;
    return !block.isVisible();
}

void GLSLEdit::fold(int line)
{
    QTextBlock startBlock = document()->findBlockByNumber(line - 1);
    int endPos = findClosingConstruct(startBlock);
    if (endPos < 0)
        return;
    QTextBlock endBlock = document()->findBlock(endPos);

    QTextBlock block = startBlock.next();
    while (block.isValid() && block != endBlock) {
        block.setVisible(false);
        block.setLineCount(0);
        block = block.next();
    }

    document()->markContentsDirty(startBlock.position(), endPos - startBlock.position() + 1);
    updateSidebar();
    update();

    GLSLDocLayout *layout = reinterpret_cast<GLSLDocLayout*>(document()->documentLayout());
    layout->forceUpdate();
}

void GLSLEdit::unfold(int line)
{
    QTextBlock startBlock = document()->findBlockByNumber(line - 1);
    int endPos = findClosingConstruct(startBlock);

    QTextBlock block = startBlock.next();
    while (block.isValid() && !block.isVisible()) {
        block.setVisible(true);
        block.setLineCount(block.layout()->lineCount());
        endPos = block.position() + block.length();
        block = block.next();
    }

    document()->markContentsDirty(startBlock.position(), endPos - startBlock.position() + 1);
    updateSidebar();
    update();

    GLSLDocLayout *layout = reinterpret_cast<GLSLDocLayout*>(document()->documentLayout());
    layout->forceUpdate();
}

void GLSLEdit::toggleFold(int line)
{
    if (isFolded(line))
        unfold(line);
    else
        fold(line);
}

void GLSLEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    updateSidebar();
}

void GLSLEdit::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier) {
        int steps = e->delta() / 20;
        steps = qBound(-3, steps, 3);
        QFont textFont = font();
        int pointSize = textFont.pointSize() + steps;
        pointSize = qBound(10, pointSize, 40);
        textFont.setPointSize(pointSize);
        setFont(textFont);
        updateSidebar();
        e->accept();
        return;
    }
    QPlainTextEdit::wheelEvent(e);
}


void GLSLEdit::updateCursor()
{
    Q_D(GLSLEdit);

    if (isReadOnly()) {
        setExtraSelections(QList<QTextEdit::ExtraSelection>());
    } else {

        d->matchPositions.clear();
        d->errorPositions.clear();

        if (d->bracketsMatching && textCursor().block().userData()) {
            QTextCursor cursor = textCursor();
            int cursorPosition = cursor.position();

            if (document()->characterAt(cursorPosition) == '{') {
                int matchPos = findClosingMatch(document(), cursorPosition);
                if (matchPos < 0) {
                    d->errorPositions += cursorPosition;
                } else {
                    d->matchPositions += cursorPosition;
                    d->matchPositions += matchPos;
                }
            }

            if (document()->characterAt(cursorPosition - 1) == '}') {
                int matchPos = findOpeningMatch(document(), cursorPosition);
                if (matchPos < 0) {
                    d->errorPositions += cursorPosition - 1;
                } else {
                    d->matchPositions += cursorPosition - 1;
                    d->matchPositions += matchPos;
                }
            }
        }

        QTextEdit::ExtraSelection highlight;
        highlight.format.setBackground(d->cursorColor);
        highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
        highlight.cursor = textCursor();
        highlight.cursor.clearSelection();

        QList<QTextEdit::ExtraSelection> extraSelections;
        extraSelections.append(highlight);

        for (int i = 0; i < d->matchPositions.count(); ++i) {
            int pos = d->matchPositions.at(i);
            QTextEdit::ExtraSelection matchHighlight;
            matchHighlight.format.setBackground(d->bracketMatchColor);
            matchHighlight.cursor = textCursor();
            matchHighlight.cursor.setPosition(pos);
            matchHighlight.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
            extraSelections.append(matchHighlight);
        }

        for (int i = 0; i < d->errorPositions.count(); ++i) {
            int pos = d->errorPositions.at(i);
            QTextEdit::ExtraSelection errorHighlight;
            errorHighlight.format.setBackground(d->bracketErrorColor);
            errorHighlight.cursor = textCursor();
            errorHighlight.cursor.setPosition(pos);
            errorHighlight.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
            extraSelections.append(errorHighlight);
        }

        setExtraSelections(extraSelections);
    }
}

void GLSLEdit::updateSidebar(const QRect &rect, int d)
{
    Q_UNUSED(rect)
    if (d != 0)
        updateSidebar();
}

void GLSLEdit::updateSidebar()
{
    Q_D(GLSLEdit);

    if (!d->showLineNumbers && !d->codeFolding) {
        d->sidebar->hide();
        setViewportMargins(0, 0, 0, 0);
        d->sidebar->setGeometry(3, 0, 0, height());
        return;
    }

    d->sidebar->foldIndicatorWidth = 0;
    d->sidebar->font = this->font();
    d->sidebar->show();

    int sw = 0;
    if (d->showLineNumbers) {
        int digits = 2;
        int maxLines = blockCount();
        for (int number = 10; number < maxLines; number *= 10)
            ++digits;
        sw += fontMetrics().width('w') * digits;
    }
    if (d->codeFolding) {
        int fh = fontMetrics().lineSpacing();
        int fw = fontMetrics().width('w');
        d->sidebar->foldIndicatorWidth = qMax(fw, fh);
        sw += d->sidebar->foldIndicatorWidth;
    }
    setViewportMargins(sw, 0, 0, 0);

    d->sidebar->setGeometry(0, 0, sw, height());
    QRectF sidebarRect(0, 0, sw, height());

    QTextBlock block = firstVisibleBlock();
    int index = 0;
    while (block.isValid()) {
        if (block.isVisible()) {
            QRectF rect = blockBoundingGeometry(block).translated(contentOffset());
            if (sidebarRect.intersects(rect)) {
                if (d->sidebar->lineNumbers.count() >= index)
                    d->sidebar->lineNumbers.resize(index + 1);
                d->sidebar->lineNumbers[index].position = rect.top();
                d->sidebar->lineNumbers[index].number = block.blockNumber() + 1;
                d->sidebar->lineNumbers[index].foldable = d->codeFolding ? isFoldable(block.blockNumber() + 1) : false;
                d->sidebar->lineNumbers[index].folded = d->codeFolding ? isFolded(block.blockNumber() + 1) : false;
                ++index;
            }
            if (rect.top() > sidebarRect.bottom())
                break;
        }
        block = block.next();
    }
    d->sidebar->lineNumbers.resize(index);
    d->sidebar->update();
}

void GLSLEdit::mark(const QString &str, Qt::CaseSensitivity sens)
{
    d_ptr->highlighter->mark(str, sens);
}

void GLSLEdit::indent()
{
    QTemporaryFile file(QLatin1String("shader.glsl"));
    if (!file.open()) {
        qDebug()<<"Couldn't create temporary file "<<file.fileName();
        return;
    }
    file.write(toPlainText().toUtf8());
    file.flush();

    QString tempFileName =
            QDir::toNativeSeparators(QFileInfo(file).canonicalFilePath());

    QProcess astyle;
    astyle.setStandardInputFile(tempFileName);
    astyle.start("astyle");
    if (!astyle.waitForStarted()) {
        qDebug()<<"Couldn't start the 'astyle' process!";
        QMessageBox::warning(this,
                             tr("QApiTrace"),
                             tr("QApiTrace could not locate the 'astyle'\n"
                                "binary. Make sure 'astyle' is installed\n"
                                "and in the PATH."),
                             QMessageBox::Ok);
        return;
    }

    if (!astyle.waitForFinished()) {
        qDebug()<<"Couldn't finish the 'astyle' process";
        return;
    }

    QByteArray result = astyle.readAll();
    setPlainText(QString::fromUtf8(result));
}

void GLSLEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = createStandardContextMenu();

    menu->addAction(tr("Indent Code"), this, SLOT(indent()));

    menu->exec(e->globalPos());
    delete menu;
}

#include "glsledit.moc"
