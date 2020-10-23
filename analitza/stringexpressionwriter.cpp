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

#include "stringexpressionwriter.h"
#include "value.h"
#include "vector.h"
#include "container.h"
#include <QStringList>
#include "list.h"
#include "variable.h"
#include "apply.h"
#include "analitzautils.h"
#include "matrix.h"

using namespace Analitza;

const double StringExpressionWriter::MIN_PRINTABLE_VALUE = 0.0000000000001; // since the precision we choose for 'G' is 12

template <class T>
QStringList StringExpressionWriter::allValues(T it, const T& itEnd, AbstractExpressionVisitor* writer)
{
    QStringList elements;
    for(; it!=itEnd; ++it)
        elements += (*it)->accept(writer).toString();
    
    return elements;
}


QMap<Operator::OperatorType, QString> initOperators()
{
    QMap<Operator::OperatorType, QString> ret;
    ret.insert(Operator::plus, QStringLiteral("+"));
    ret.insert(Operator::times, QStringLiteral("*"));
    ret.insert(Operator::divide, QStringLiteral("/"));
    ret.insert(Operator::eq, QStringLiteral("="));
    ret.insert(Operator::neq, QStringLiteral("!="));
    ret.insert(Operator::lt, QStringLiteral("<"));
    ret.insert(Operator::leq, QStringLiteral("<="));
    ret.insert(Operator::gt, QStringLiteral(">"));
    ret.insert(Operator::geq, QStringLiteral(">="));
    ret.insert(Operator::power, QStringLiteral("^"));
    ret.insert(Operator::minus, QStringLiteral("-"));
    return ret;
}

const QMap<Operator::OperatorType, QString> StringExpressionWriter::s_operators=initOperators();

StringExpressionWriter::StringExpressionWriter(const Object* o)
{
    if (o)
        m_result=o->accept(this);
}

QVariant StringExpressionWriter::visit(const Ci* var)
{
    return var->name();
}

QVariant StringExpressionWriter::visit(const Operator* op)
{
    return op->name();
}

QVariant StringExpressionWriter::visit(const Vector* vec)
{
    return QStringLiteral("vector { %1 }").arg(allValues<Vector::const_iterator>(vec->constBegin(), vec->constEnd(), this).join(QStringLiteral(", ")));
}

QVariant StringExpressionWriter::visit(const Matrix* m)
{
    return QStringLiteral("matrix { %1 }").arg(allValues(m->constBegin(), m->constEnd(), this).join(QStringLiteral(", ")));
}

QVariant StringExpressionWriter::visit(const MatrixRow* mr)
{
    return QStringLiteral("matrixrow { %1 }").arg(allValues(mr->constBegin(), mr->constEnd(), this).join(QStringLiteral(", ")));
}

QVariant StringExpressionWriter::visit(const List* vec)
{
    if(!vec->isEmpty() && vec->at(0)->type()==Object::value && static_cast<Cn*>(vec->at(0))->format()==Cn::Char)
        return QVariant::fromValue<QString>(QStringLiteral("\"")+AnalitzaUtils::listToString(vec)+'\"');
    else
        return QVariant::fromValue<QString>(QStringLiteral("list { %1 }").arg(allValues<List::const_iterator>(vec->constBegin(), vec->constEnd(), this).join(QStringLiteral(", "))));
}

QVariant StringExpressionWriter::visit(const Cn* var)
{
    if(var->isBoolean())
        return var->isTrue() ? "true" : "false";
    else if(var->isCharacter())
        return QString(var->character());
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

        return QVariant::fromValue<QString>(realpart+imagpart);
    }
    else
        return QString::number(var->value(), 'g', 12);
}

int StringExpressionWriter::weight(const Operator* op, int size, int pos)
{
    switch(op->operatorType()) {
        case Operator::lt:
        case Operator::gt:
        case Operator::eq:
        case Operator::neq:
        case Operator::leq:
        case Operator::geq:
            return 1;
        case Operator::plus:
            return 2;
        case Operator::minus:
            return size==1 ? 8 : 3;
        case Operator::times:
            return 4;
        case Operator::divide:
            return 5 + (pos>0 ? 0 : 1);
        case Operator::_and:
        case Operator::_or:
        case Operator::_xor:
            return 6;
        case Operator::power:
            return 7 + (pos>0 ? 0 : 1);
        default:
            return 1000;
    }
}

QVariant StringExpressionWriter::visit(const Analitza::Apply* a)
{
    Operator op=a->firstOperator();
    QStringList ret;
    QString toret;
    QString bounds;
    QStringList bvars=a->bvarStrings();
    
    if(a->ulimit() || a->dlimit()) {
        bounds += '=';
        if(a->dlimit())
            bounds += a->dlimit()->accept(this).toString();
        bounds += QLatin1String("..");
        if(a->ulimit())
            bounds += a->ulimit()->accept(this).toString();
    }
    else if(a->domain())
        bounds += '@'+a->domain()->accept(this).toString();
    
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
                const Apply *c = (const Apply*) o;
                QString s = c->accept(this).toString();
                if(s_operators.contains(op.operatorType()) && !c->isUnary()) {
                    Operator child_op = c->firstOperator();
                    
                    if(child_op.operatorType() && weight(&op, c->countValues(), -1)>weight(&child_op, c->countValues(), i))
                        s=QStringLiteral("(%1)").arg(s);
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
            n='('+n+')';
        
        toret += QStringLiteral("%1(%2)").arg(n, ret.join(QStringLiteral(", ")));
    } else if(op.operatorType()==Operator::selector) {
        if(a->m_params.last()->isApply()) {
            const Apply* a1=static_cast<const Apply*>(a->m_params.last());
            if(s_operators.contains(a1->firstOperator().operatorType()))
                ret.last()='('+ret.last()+')';
        }
        
        toret += QStringLiteral("%1[%2]").arg(ret.last(), ret.first());
    } else if(ret.count()>1 && s_operators.contains(op.operatorType())) {
        toret += ret.join(s_operators.value(op.operatorType()));
    } else if(ret.count()==1 && op.operatorType()==Operator::minus)
        toret += '-'+ret[0];
    else {
        QString bounding;
        if(!bounds.isEmpty() || !bvars.isEmpty()) {
            if(bvars.count()!=1) bounding +='(';
            bounding += bvars.join(QStringLiteral(", "));
            if(bvars.count()!=1) bounding +=')';
            
            bounding = ':'+bounding +bounds;
        }
            
        toret += QStringLiteral("%1(%2%3)").arg(op.accept(this).toString(), ret.join(QStringLiteral(", ")), bounding);
    }
    
    return toret;
}

QVariant StringExpressionWriter::visit(const Container* var)
{
    QStringList ret = allValues(var->constBegin(), var->constEnd(), this);
    
    QString toret;
    switch(var->containerType()) {
        case Container::declare:
            toret += ret.join(QStringLiteral(":="));
            break;
        case Container::lambda: {
            QString last=ret.takeLast();
            QStringList bvars = var->bvarStrings();
            if(bvars.count()!=1) toret +='(';
            toret += bvars.join(QStringLiteral(", "));
            if(bvars.count()!=1) toret +=')';
            toret += "->" + last;
        }    break;
        case Container::math:
            toret += ret.join(QStringLiteral("; "));
            break;
        case Container::uplimit: //x->(n1..n2) is put at the same time
        case Container::downlimit:
            break;
        case Container::bvar:
            if(ret.count()>1) toret += '(';
            toret += ret.join(QStringLiteral(", "));
            if(ret.count()>1) toret += ')';
            break;
        case Container::piece:
            toret += ret[1]+" ? "+ret[0];
            break;
        case Container::otherwise:
            toret += "? "+ret[0];
            break;
        default:
            toret += var->tagName()+" { "+ret.join(QStringLiteral(", "))+" }";
            break;
    }
    return toret;
}

QVariant StringExpressionWriter::visit(const CustomObject*)
{
    return "CustomObject";
}

QVariant StringExpressionWriter::visit(const None* )
{
    return QString();
}
