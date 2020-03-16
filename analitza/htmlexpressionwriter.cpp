/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "htmlexpressionwriter.h"
#include "value.h"
#include "operator.h"
#include "container.h"
#include "vector.h"
#include <QStringList>
#include <QCoreApplication>

#include "list.h"
#include "variable.h"
#include "stringexpressionwriter.h"
#include "apply.h"
#include "analitzautils.h"
#include "matrix.h"

using namespace Analitza;

const double HtmlExpressionWriter::MIN_PRINTABLE_VALUE = 0.0000000000001; // since the precision we choose for 'G' is 12

//we use the one in string*
QMap<Operator::OperatorType, QString> initOperators();

template <class T>
QStringList HtmlExpressionWriter::allValues(T it, const T& itEnd, AbstractExpressionVisitor* writer)
{
    QStringList elements;
    for(; it!=itEnd; ++it)
        elements += (*it)->accept(writer).toString();
    
    return elements;
}


static const QMap<Operator::OperatorType, QString> s_operators=initOperators();

QString oper(const QString& op) { return QCoreApplication::translate("html representation of an operator", "<span class='op'>%1</span>").arg(op); }
QString oper(const QChar& op) { return QCoreApplication::translate("html representation of an operator", "<span class='op'>%1</span>").arg(op); }
QString keyword(const QString& op) { return QCoreApplication::translate("html representation of an operator", "<span class='keyword'>%1</span>").arg(op); }

HtmlExpressionWriter::HtmlExpressionWriter(const Object* o)
{
    if(o)
        m_result=o->accept(this);
}

QVariant HtmlExpressionWriter::visit(const Vector* vec)
{
    return QString(keyword(QStringLiteral("vector "))+oper(QStringLiteral("{ "))+allValues<Vector::const_iterator>(vec->constBegin(), vec->constEnd(), this).join(QString(oper(QStringLiteral(", "))))+oper(QStringLiteral(" }")));
}

QVariant HtmlExpressionWriter::visit(const Matrix* m)
{
    return QString(keyword(QStringLiteral("matrix "))+oper(QStringLiteral("{ "))+allValues(m->constBegin(), m->constEnd(), this).join(QString(oper(QStringLiteral(", "))))+oper(QStringLiteral(" }")));
}

QVariant HtmlExpressionWriter::visit(const MatrixRow* mr)
{
    return QString(keyword(QStringLiteral("matrixrow "))+oper(QStringLiteral("{ "))+allValues(mr->constBegin(), mr->constEnd(), this).join(QString(oper(QStringLiteral(", "))))+oper(QStringLiteral(" }")));
}

QVariant HtmlExpressionWriter::visit(const List* vec)
{
    if(!vec->isEmpty() && vec->at(0)->type()==Object::value && static_cast<Cn*>(vec->at(0))->format()==Cn::Char) {
        return QVariant::fromValue<QString>(QStringLiteral("<span class='string'>&quot;") + AnalitzaUtils::listToString(vec) + QStringLiteral("&quot;</span>"));
    } else
        return QVariant::fromValue<QString>(keyword(QStringLiteral("list "))+oper(QStringLiteral("{ "))+allValues<List::const_iterator>(vec->constBegin(), vec->constEnd(), this).join(QString(oper(QStringLiteral(", "))))+oper(QStringLiteral(" }")));
}

QVariant HtmlExpressionWriter::visit(const Cn* var)
{
    QString innerhtml;
    if(var->isBoolean())
        innerhtml = var->isTrue() ? QStringLiteral("true") : QStringLiteral("false");
    else if(var->isCharacter())
        innerhtml = QString(var->character());
    else if(var->isComplex()) {
        QString realpart;
        QString imagpart;
        bool realiszero = false;
        const auto complex = var->complexValue();
        if (qAbs(complex.real()) > MIN_PRINTABLE_VALUE)
            realpart = QString::number(complex.real(), 'g', 12);
        else
            realiszero = true;

        if (!qFuzzyCompare(qAbs(complex.imag()), 1)) {
            if (qAbs(complex.imag()) > MIN_PRINTABLE_VALUE) {
                if (!realiszero && complex.imag()>0.)
                    realpart += QLatin1Char('+');
                imagpart = QString::number(complex.imag(), 'g', 12);
                imagpart += QStringLiteral("*i");
            }
        } else {
            if (!realiszero)
                realpart += QLatin1Char('+');
            if (qFuzzyCompare(complex.imag(), 1))
                imagpart = QStringLiteral("i");
            else
                imagpart = QStringLiteral("-i");
        }
        
        innerhtml = realpart+imagpart;
    }
    else
        innerhtml = QString::number(var->value(), 'g', 12);
    
    return QVariant::fromValue<QString>(QStringLiteral("<span class='num'>")+innerhtml+"</span>");
}

QVariant HtmlExpressionWriter::visit(const Analitza::Ci* var)
{
    return var->toHtml();
}

QVariant HtmlExpressionWriter::visit(const Analitza::Operator* o)
{
    return QVariant::fromValue<QString>(QStringLiteral("<span class='func'>") + o->toString() + QStringLiteral("</span>"));
}

QVariant HtmlExpressionWriter::visit ( const Analitza::Apply* a )
{
    Operator op=a->firstOperator();
    QStringList ret;
    QString toret;
    QString bounds;
    
    if(a->ulimit() || a->dlimit()) {
        bounds += oper('=');
        if(a->dlimit())
            bounds += a->dlimit()->accept(this).toString();
        bounds += oper(QStringLiteral(".."));
        if(a->ulimit())
            bounds += a->ulimit()->accept(this).toString();
    }
    else if(a->domain())
        bounds += oper('@')+a->domain()->accept(this).toString();
    
    int i = 0;
    foreach(Object* o, a->m_params) {
        Object::ObjectType type=o->type();
        switch(type) {
            case Object::oper:
                Q_ASSERT(false);
                break;
            case Object::variable:
                ret << static_cast<const Ci*>(o)->accept(this).toString();
                break;
            case Object::apply: {
                Apply *c = (Apply*) o;
                QString s = c->accept(this).toString();
                if(s_operators.contains(op.operatorType())) {
                    Operator child_op = c->firstOperator();
                    
                    if(child_op.operatorType() && 
                            StringExpressionWriter::weight(&op, c->countValues(), -1)>StringExpressionWriter::weight(&child_op, c->countValues(), i))
                        s=oper('(')+s+oper(')');
                }
                ret << s;
            }    break;
            default:
                ret << o->accept(this).toString();
                break;
        }
        ++i;
    }
    
    bool func=op.operatorType()==Operator::function;
    if(func) {
        QString n = ret.takeFirst();
        if(a->m_params.first()->type()!=Object::variable)
            n=oper('(')+n+oper(')');
        
        toret += n+oper('(') + ret.join(oper(QStringLiteral(", "))) + oper(')');
    } else if(op.operatorType()==Operator::selector) {
        if(a->m_params.last()->isApply()) {
            const Apply* a1=static_cast<const Apply*>(a->m_params.last());
            if(s_operators.contains(a1->firstOperator().operatorType()))
                ret.last()=oper('(')+ret.last()+oper(')');
        }
        
        toret += ret.last() + oper('[') + ret.first() + oper(']');
    } else if(ret.count()>1 && s_operators.contains(op.operatorType())) {
        toret += ret.join(oper(s_operators.value(op.operatorType())));
    } else if(ret.count()==1 && op.operatorType()==Operator::minus)
        toret += oper('-')+ret[0];
    else {
        QString bounding;
        QStringList bvars;
        foreach(const Ci* bvar, a->bvarCi())
            bvars += bvar->accept(this).toString();
        
        if(!bounds.isEmpty() || !bvars.isEmpty()) {
            if(bvars.count()!=1) bounding +=oper('(');
            bounding += bvars.join(oper(QStringLiteral(", ")));
            if(bvars.count()!=1) bounding +=oper(')');
            
            bounding = ':'+bounding +bounds;
        }
        
        toret += op.accept(this).toString()+oper('(')+ret.join(oper(QStringLiteral(", ")))+bounding+oper(')');
    }
    
    return toret;
}

QVariant HtmlExpressionWriter::visit(const Container* var)
{
    QStringList ret = allValues(var->constBegin(), var->constEnd(), this);
    
    
    QString toret;
    switch(var->containerType()) {
        case Container::declare:
            toret += ret.join(oper(QStringLiteral(":=")));
            break;
        case Container::lambda: {
            QString last=ret.takeLast();
            QStringList bvars;
            foreach(const Ci* bvar, var->bvarCi())
                bvars += bvar->accept(this).toString();
            
            if(bvars.count()!=1) toret +=oper('(');
            toret += bvars.join(QStringLiteral(", "));
            if(bvars.count()!=1) toret +=oper(')');
            toret += oper(QStringLiteral("->")) + last;
        }    break;
        case Container::math:
            toret += ret.join(oper(QStringLiteral("; ")));
            break;
        case Container::uplimit: //x->(n1..n2) is put at the same time
        case Container::downlimit:
            break;
        case Container::bvar:
            if(ret.count()>1) toret += oper('(');
            toret += ret.join(QStringLiteral(", "));
            if(ret.count()>1) toret += oper(')');
            break;
        case Container::piece:
            toret += ret[1]+oper(QStringLiteral(" ? "))+ret[0];
            break;
        case Container::otherwise:
            toret += oper(QStringLiteral("? "))+ret[0];
            break;
        default:
            toret += var->tagName()+oper(QStringLiteral(" { "))+ret.join(oper(QStringLiteral(", ")))+oper(QStringLiteral(" }"));
            break;
    }
    return toret;
}

QVariant HtmlExpressionWriter::visit(const CustomObject*)
{
    return QStringLiteral("Custom Object");
}

QVariant HtmlExpressionWriter::visit(const None* )
{
    return QString();
}
