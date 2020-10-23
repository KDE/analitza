/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "expressionedit.h"

#include <QMenu>
#include <QKeyEvent>
#include <QHeaderView>
#include <QApplication>
#include <QTimer>
#include <QPropertyAnimation>
#include <QCoreApplication>

#include <analitzagui/operatorsmodel.h>
#include <analitza/explexer.h>
#include <analitza/expressionparser.h>
#include <analitza/analyzer.h>
#include <analitza/variables.h>

using namespace Analitza;

class HelpTip : public QLabel
{
    public:
        HelpTip(QWidget* parent)
            : QLabel(parent, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip | Qt::X11BypassWindowManagerHint)
        {
            setFrameShape(QFrame::Box);
            setFocusPolicy(Qt::NoFocus);
            setAutoFillBackground(false);
            
            QPalette p=palette();
            p.setColor(backgroundRole(), p.color(QPalette::Active, QPalette::ToolTipBase));
            p.setColor(foregroundRole(), p.color(QPalette::Active, QPalette::ToolTipText));
            setPalette(p);
        }
        
        void mousePressEvent(QMouseEvent*) override
        {
            hide();
        }
};

ExpressionEdit::ExpressionEdit(QWidget *parent, AlgebraHighlighter::Mode inimode)
    : QPlainTextEdit(parent), m_histPos(0), a(nullptr), m_correct(true), m_ans(QStringLiteral("ans"))
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setTabChangesFocus(true);
    m_history.append(QString());
    
    m_helptip = new HelpTip(this);
    m_helptip->hide();
    m_hideHelpTip = new QTimer(this);
    m_hideHelpTip->setInterval(500);
    connect(m_hideHelpTip, &QTimer::timeout, m_helptip, &QWidget::hide);
    
    m_highlight= new AlgebraHighlighter(this->document(), a);
    
    m_completer = new QCompleter(this);
    m_completer->setWidget(this);
    m_completer->setCompletionColumn(0);
    m_completer->setCompletionRole(Qt::DisplayRole);
    QTreeView* completionView = new QTreeView;
    m_completer->setPopup(completionView);
    completionView->setRootIsDecorated(false);
    completionView->header()->hide();
//     completionView->resizeColumnToContents(1);
    completionView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    completionView->setMinimumWidth(300);
    m_ops = new OperatorsModel(m_completer);
    m_completer->setModel(m_ops);
    
    updateCompleter();
    
    completionView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    completionView->showColumn(0);
    completionView->showColumn(1);
    completionView->hideColumn(2);
    completionView->hideColumn(3);
    
    connect(this, &ExpressionEdit::returnPressed, this, &ExpressionEdit::returnP);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &ExpressionEdit::cursorMov);
    connect(this, SIGNAL(signalHelper(QString)), this, SLOT(helper(QString)));
    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(completed(QString)));
//     connect(m_completer, SIGNAL(activated(QModelIndex)), this, SLOT(completed(QModelIndex)));
    
    setMode(inimode);
    m_lineHeight = QFontMetrics(currentCharFormat().font()).height();
    setFixedHeight(m_lineHeight+15);
    
    setInputMethodHints(Qt::ImhNoAutoUppercase);
}

ExpressionEdit::~ExpressionEdit()
{
}

void ExpressionEdit::setExpression(const Analitza::Expression& e)
{
    if(!e.isCorrect())
        clear();
    else if(isMathML())
        setText(e.toMathML());
    else
        setText(e.toString());
    
    setCorrect(true);
}

void ExpressionEdit::updateCompleter()
{
    m_ops->updateInformation();
}
 
void ExpressionEdit::completed(const QString& newText)
{
    int l = OperatorsModel::lastWord(textCursor().selectionStart(), text()).length();
    QString toInsert=newText.mid(l);
    if(Analitza::Expression::whatType(newText) == Analitza::Object::oper && !isMathML())
        toInsert += '(';
    insertPlainText(toInsert);
}

bool ExpressionEdit::isMathML() const
{
    switch(m_highlight->mode()) {
        case AlgebraHighlighter::MathML:
            return true;
        case AlgebraHighlighter::Expression:
            return false;
        default:
            return Analitza::Expression::isMathML(this->toPlainText());
    }
}

void ExpressionEdit::setMode(AlgebraHighlighter::Mode en)
{
    bool correct=true;
    if(!text().isEmpty()) {
        if(isMathML() && en==AlgebraHighlighter::Expression) { //We convert it into MathML
            Analitza::Expression e(toPlainText(), true);
            correct=e.isCorrect();
            if(correct) setPlainText(e.toString());
        } else if(!isMathML() && en==AlgebraHighlighter::MathML) {
            Analitza::Expression e(toPlainText(), false);
            correct=e.isCorrect();
            if(correct) setPlainText(e.toMathML());
        }
    }
    if(correct)
        m_highlight->setMode(en);
    
    setCorrect(correct);
}

void ExpressionEdit::returnP()
{
//     removenl();
    if(!this->toPlainText().isEmpty()) {
        m_history.last() = this->toPlainText();
        m_history.append(QString());
        m_histPos=m_history.count()-1;
    }
}

void ExpressionEdit::keyPressEvent(QKeyEvent * e)
{
    bool ch=false;
    QAbstractItemView* completionView = m_completer->popup();
    
    switch(e->key()){
        case Qt::Key_F2:
            simplify();
            break;
        case Qt::Key_Escape:
            if(!completionView->isVisible())
                selectAll();
            
            completionView->hide();
            m_helptip->hide();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if(completionView->isVisible() && !completionView->selectionModel()->selectedRows().isEmpty()) 
                completed(m_completer->currentCompletion());
            else if(returnPress())
                    QPlainTextEdit::keyPressEvent(e);
            completionView->hide();
            break;
        case Qt::Key_Up:
            if(!completionView->isVisible()) {
                m_histPos--;
                ch=true;
            }
            break;
        case Qt::Key_Down:
            if(!completionView->isVisible()) {
                m_histPos++;
                ch=true;
            }
            break;
        case Qt::Key_Left:
        case Qt::Key_Right:
            m_highlight->rehighlight();
            QPlainTextEdit::keyPressEvent(e);
            break;
        case Qt::Key_Plus:
        case Qt::Key_Asterisk:
        case Qt::Key_Slash:
            if(this->toPlainText().length() == (this->textCursor().position()-this->textCursor().anchor())) {
                this->setPlainText(m_ans);
                QTextCursor tc = this->textCursor();
                tc.setPosition(m_ans.length());
                this->setTextCursor(tc);
            }
            QPlainTextEdit::keyPressEvent(e);
            break;
        case Qt::Key_Alt:
            QPlainTextEdit::keyPressEvent(e);
            break;
        default:
            QPlainTextEdit::keyPressEvent(e);
            m_history.last() = this->toPlainText();
            QString last = OperatorsModel::lastWord(textCursor().selectionStart(), text());
            if(!last.isEmpty()) {
                m_completer->setCompletionPrefix(last);
                m_completer->complete();
            } else {
                completionView->hide();
            }
            break;
    }
    
    if(ch) {
        if(m_histPos<0)
            m_histPos=0;
        if(m_histPos>=m_history.count())
            m_histPos=m_history.count()-1;
        this->setPlainText(m_history[m_histPos]);
    }
    
    if(m_completer->completionCount()==1 && m_completer->completionPrefix()==m_completer->currentCompletion()) {
        completionView->hide();
    }
    
    int lineCount=toPlainText().count('\n')+1;
    setFixedHeight(m_lineHeight*lineCount+15);
    setCorrect(m_highlight->isCorrect());
}

void ExpressionEdit::cursorMov()
{
    int pos=this->textCursor().position();
    m_highlight->setPos(pos);
    if(text().isEmpty())
        setCorrect(true);
    
    QString help = helpShow(m_highlight->editingName(),
                            m_highlight->editingParameter(),
                            m_highlight->editingBounds(),
                            a ? a->variables().data() : nullptr);
    
    if(help.isEmpty()) {
        if(isCorrect()) {
            QTimer::singleShot(500, this, &ExpressionEdit::showSimplified);
        }
    } else
        helper(help);
}


void ExpressionEdit::showSimplified()
{
    Analitza::Analyzer a;
    a.setExpression(expression());
    QString help;
    if(a.isCorrect()) {
        a.simplify();
        help=QCoreApplication::tr("Result: %1").arg(a.expression().toString());
    }
    helper(help);
}


QString ExpressionEdit::helpShow(const QString& funcname, int param, bool bounds, const Analitza::Variables* v) const
{
    QString ret;
    QModelIndex idx = m_ops->indexForOperatorName(funcname);
    
    if(idx.isValid())
        ret = m_ops->parameterHelp(idx, param, bounds);
    else if(v && v->contains(funcname)) { //if it is a function defined by the user
        Analitza::Expression val=v->valueExpression(funcname);
        if(val.isLambda())
            ret = m_ops->standardFunctionCallHelp(funcname, param, val.bvarList().size(), false, false);
    }
    return ret;
}

void ExpressionEdit::removenl()
{
    this->setPlainText(this->toPlainText().remove('\n'));
}

void ExpressionEdit::helper(const QString& msg)
{
    QPoint pos = mapToGlobal( QPoint( cursorRect().left(), height() ) );
    
    if(msg.isEmpty()) {
        if(!m_hideHelpTip->isActive())
            m_hideHelpTip->start();
    } else {
        helper(msg, pos-QPoint(0, 50));
        m_hideHelpTip->stop();
    }
}

void ExpressionEdit::helper(const QString& msg, const QPoint& p)
{
    if(isVisible()){
        m_helptip->setText(msg);
        m_helptip->resize(m_helptip->sizeHint());
        if(!m_helptip->isVisible()) {
            m_helptip->move(p);
            
            m_helptip->show();
            m_helptip->raise();
        } else {
            QPropertyAnimation* anim = new QPropertyAnimation(m_helptip, "pos", this);
            anim->setEndValue(p);
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
        setFocus();
    }
}

void ExpressionEdit::setCorrect(bool correct)
{
    QPalette p=qApp->palette();
    QColor c;
    m_correct=correct;
    
    if(m_correct && !isMathML())
        c = p.base().color();
    else if(m_correct)
        c = QColor(255,255,200);
    else //if mathml
        c = QColor(255,222,222);
    
    p.setColor(QPalette::Active, QPalette::Base, c);
    setPalette(p);
}

void ExpressionEdit::focusInEvent ( QFocusEvent * event )
{
    QPlainTextEdit::focusInEvent(event);
    switch(event->reason()) {
        case Qt::OtherFocusReason:
        case Qt::TabFocusReason:
            this->selectAll();
        default:
            break;
    }
}

void ExpressionEdit::focusOutEvent ( QFocusEvent * event)
{
    m_helptip->hide();
    
    QPlainTextEdit::focusOutEvent(event);
}

void ExpressionEdit::simplify()
{
    Analitza::Analyzer a;
    a.setExpression(expression());
    if(a.isCorrect()) {
        a.simplify();
        setExpression(a.expression());
    }
    selectAll();
}

void ExpressionEdit::contextMenuEvent(QContextMenuEvent * e)
{
    QScopedPointer<QMenu> popup(createStandardContextMenu());
    popup->addSeparator();
    if(isMathML())
        popup->addAction(QCoreApplication::tr("To Expression"), this, &ExpressionEdit::toExpression);
    else
        popup->addAction(QCoreApplication::tr("To MathML"), this, &ExpressionEdit::toMathML);
    
    popup->addAction(QCoreApplication::tr("Simplify"), this, &ExpressionEdit::simplify);
    
    QMenu* examples=popup->addMenu(QCoreApplication::tr("Examples"));
    examples->setEnabled(!m_examples.isEmpty());
    foreach(const QString &example, m_examples) {
        QAction* ac=examples->addAction(example);
        ac->setData(example);
    }
    connect(examples, &QMenu::triggered, this, &ExpressionEdit::setActionText);
    
    popup->exec(e->globalPos());

}

void ExpressionEdit::setActionText(QAction* text)
{
    setPlainText(text->data().toString());
}

void ExpressionEdit::setAnalitza(Analitza::Analyzer * in)
{
    m_highlight->setAnalitza(in);
    a=in;
    m_ops->setVariables(a->variables());
    updateCompleter();
}

bool ExpressionEdit::returnPress()
{
    bool haveToPress=false;
    if(isMathML()) {
        emit returnPressed();
    } else {
        bool complete = Analitza::Expression::isCompleteExpression(toPlainText());
        haveToPress = !complete;
        setCorrect(complete);
        if(complete)
            emit returnPressed();
    }
    m_helptip->hide();
    return haveToPress;
}

void ExpressionEdit::insertText(const QString& text)
{
    QTextCursor tc=textCursor();
    tc.insertText(text);
}

Analitza::Expression ExpressionEdit::expression() const
{
    return Analitza::Expression(text(), isMathML());
}

bool ExpressionEdit::isCorrect() const
{
    return m_correct && Analitza::Expression::isCompleteExpression(toPlainText());
}


