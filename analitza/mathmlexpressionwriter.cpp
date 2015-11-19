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

#include "mathmlexpressionwriter.h"
#include "value.h"
#include "vector.h"
#include "operator.h"
#include "container.h"
#include <QStringList>
#include "list.h"
#include "variable.h"
#include "apply.h"
#include "explexer.h"
#include "analitzautils.h"
#include "matrix.h"

using namespace Analitza;

MathMLExpressionWriter::MathMLExpressionWriter(const Object* o)
{
    m_result=o->accept(this);
}

QVariant MathMLExpressionWriter::visit(const Ci* var)
{
    QString attrib;
    if(var->isFunction())
        attrib=QStringLiteral(" type='function'");
    return QVariant::fromValue<QString>(QStringLiteral("<ci")+attrib+'>'+var->name()+QStringLiteral("</ci>"));
}

QVariant MathMLExpressionWriter::visit(const Operator* op)
{
    if(op->operatorType()==Operator::function)
        return QString();
    else
        return QStringLiteral("<%1 />").arg(op->name());
}

QVariant MathMLExpressionWriter::visit(const Cn* val)
{
    if(val->isBoolean()) {
        if(val->isTrue())
            return "<cn type='constant'>true</cn>";
        else
            return "<cn type='constant'>false</cn>";
    } else {
        QString type;
        if(val->format()==Cn::Real)
            type = QStringLiteral(" type='real'");
        
        return QStringLiteral("<cn%1>%2</cn>").arg(type).arg(val->value(), 0, 'g', 12);
    }
}

QVariant MathMLExpressionWriter::visit(const Vector* vec)
{
    QStringList elements;
    for(Vector::const_iterator it=vec->constBegin(); it!=vec->constEnd(); ++it)
    {
        elements += (*it)->accept(this).toString();
    }
    return QStringLiteral("<vector>%1</vector>").arg(elements.join(QString()));
}

QVariant MathMLExpressionWriter::visit(const Matrix* m)
{
    QStringList elements;
    for(Matrix::const_iterator it=m->constBegin(); it!=m->constEnd(); ++it)
    {
        elements += (*it)->accept(this).toString();
    }
    return QStringLiteral("<matrix>%1</matrix>").arg(elements.join(QString()));
}

QVariant MathMLExpressionWriter::visit(const MatrixRow* mr)
{
    QStringList elements;
    for(MatrixRow::const_iterator it=mr->constBegin(); it!=mr->constEnd(); ++it)
    {
        elements += (*it)->accept(this).toString();
    }
    return QStringLiteral("<matrixrow>%1</matrixrow>").arg(elements.join(QString()));
}

QVariant MathMLExpressionWriter::visit(const List* vec)
{
    QStringList elements;
    if(vec->size()==0)
        return "<list />";
    else if(vec->at(0)->type()==Object::value && static_cast<const Cn*>(vec->at(0))->format()==Cn::Char) {
        QString ret=AnalitzaUtils::listToString(vec);
        ret = ExpLexer::escape(ret);
        ret = "<cs>"+ret+"</cs>";
        return ret;
    } else {
        for(List::const_iterator it=vec->constBegin(); it!=vec->constEnd(); ++it)
            elements += (*it)->accept(this).toString();
        
        return QStringLiteral("<list>%1</list>").arg(elements.join(QString()));
    }
}

QVariant MathMLExpressionWriter::visit(const Container* c)
{
    QString ret;
    foreach(const Object* o, c->m_params)
        ret += o->accept(this).toString();
    
    return QStringLiteral("<%1>%2</%1>").arg(c->tagName(), ret);
}

QVariant MathMLExpressionWriter::visit(const Apply* a)
{
    QString ret;
    
    ret += a->firstOperator().accept(this).toString();
    foreach(const Ci* bvar, a->bvarCi())
        ret += "<bvar>"+bvar->accept(this).toString()+"</bvar>";
    if(a->ulimit()) ret += "<uplimit>"+a->ulimit()->accept(this).toString()+"</uplimit>";
    if(a->dlimit()) ret += "<downlimit>"+a->dlimit()->accept(this).toString()+"</downlimit>";
    if(a->domain()) ret += "<domainofapplication>"+a->domain()->accept(this).toString()+"</domainofapplication>";
    
    foreach(const Object* o, a->m_params)
        ret += o->accept(this).toString();
    
    return QStringLiteral("<apply>%1</apply>").arg(ret);
}

QVariant MathMLExpressionWriter::visit(const Analitza::CustomObject*)
{
    return "<!-- custom object -->";
}

QVariant MathMLExpressionWriter::visit(const None* )
{
    return QString();
}

