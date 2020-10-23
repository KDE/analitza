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

#include "mathmlpresentationexpressionwriter.h"
#include "value.h"
#include "container.h"
#include <QStringList>
#include "vector.h"
#include "list.h"
#include "variable.h"
#include "apply.h"
#include <analitza/analitzautils.h>
#include "matrix.h"

using namespace Analitza;

namespace
{

template <class T>
QStringList convertElements(T it, const T& itEnd, MathMLPresentationExpressionWriter* w)
{
    QStringList elems;
    for(; it!=itEnd; ++it) {
        elems += (*it)->accept(w).toString();
    }
    return elems;
}

template <const char **C>
static QString joinOp(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    QString op=QStringLiteral("<mo>%1</mo>").arg(*C);
    return convertElements<Apply::const_iterator>(c->firstValue(), c->constEnd(), w).join(op);
}

template <const char **C, const char **D>
static QString infix(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    QString exp=QStringLiteral("<mrow><mo>%1</mo>%2<mo>%3</mo></mrow>").arg(*C)
        .arg(convertElements<Apply::const_iterator>(c->firstValue(), c->constEnd(), w).join(QString())).arg(*D);
    return exp;
}

template <const char **C>
static QString prefix(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    return QStringLiteral("<mo>%1</mo>").arg(*C)+convertElements(c->firstValue(), c->constEnd(), w).join(QString());
}

template <const char **C>
static QString prefixOp(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    return QString(*C)+convertElements(c->firstValue(), c->constEnd(), w).join(QString());
}

template <const char **C>
static QString postfix(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    return convertElements(c->firstValue(), c->constEnd(), w).join(QString())+QStringLiteral("<mo>%1</mo>").arg(*C);
}

QString minus(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    QStringList e=convertElements(c->firstValue(), c->constEnd(), w);
    if(e.count()==1)
        return "<mo>-</mo>"+e[0];
    else
        return e.join(QStringLiteral("<mo>-</mo>"));
}

QString power(const Apply* c, MathMLPresentationExpressionWriter* w)
{    return "<msup>"+convertElements(c->firstValue(), c->constEnd(), w).join(QString())+"</msup>"; }

QString divide(const Apply* c, MathMLPresentationExpressionWriter* w)
{    return "<mfrac><mrow>"+convertElements(c->firstValue(), c->constEnd(), w).join(QStringLiteral("</mrow><mrow>"))+"</mrow></mfrac>"; }

QString quotient(const Apply* c, MathMLPresentationExpressionWriter* w)
{    return divide(c, w); }

QString root(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    Cn two(2);
    if(AnalitzaUtils::equalTree(c->values().at(1), &two))
        return "<msqrt>"+(*c->firstValue())->accept(w).toString()+"</msqrt>";
    else
        return "<mroot>"+convertElements<Apply::const_iterator>(c->firstValue(), c->constEnd(), w).join(QString())+"</mroot>";
}

QString diff(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    QStringList bv=c->bvarStrings();
    return "<msubsup><mfenced>"+convertElements<Apply::const_iterator>(c->firstValue(), c->constEnd(), w).join(QString())+"</mfenced>"
            "<mrow>"+bv.join(QStringLiteral("<mo>,</mo>"))+"</mrow><mo>'</mo></msubsup>";
}

QString exp(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    return "<msup><mn>&ExponentialE;</mn>"+convertElements<Apply::const_iterator>(c->firstValue(), c->constEnd(), w).at(0)+"</msup>";
}

QString iterative(Operator::OperatorType t, const Apply* c, MathMLPresentationExpressionWriter* w)
{
    QString op= t==Operator::sum ? QStringLiteral("&Sum;") : QStringLiteral("&Prod;");
    QString ul="<mrow>"+c->ulimit()->toString()+"</mrow>";
    QString dl="<mrow>"+c->bvarStrings().join(QStringLiteral(", "))+"<mo>=</mo>"+c->dlimit()->toString()+"</mrow>";
    
    return "<mrow><msubsup><mo>"+op+"</mo>"+dl+ul+"</msubsup>"+convertElements(c->firstValue(), c->constEnd(), w).join(QString())+"</mrow>";
}

QString sum(const Apply* c, MathMLPresentationExpressionWriter* w)
{ return iterative(Operator::sum, c, w); }

QString product(const Apply* c, MathMLPresentationExpressionWriter* w)
{ return iterative(Operator::product, c, w); }

QString selector(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    QStringList el=convertElements(c->firstValue(), c->constEnd(), w);
    return "<msub><mrow>"+el.last()+"</mrow><mrow>"+el.first()+"</mrow></msub>";
}

QString function(const Apply* c, MathMLPresentationExpressionWriter* w)
{
    QString ret=QStringLiteral("<mrow>");
    foreach(const Ci* bvar, c->bvarCi())
        ret+=bvar->accept(w).toString();
    foreach(const Object* o, c->values())
        ret+=o->accept(w).toString();
    ret+=QLatin1String("</mrow>");
    return ret;
}

const char* plus="+", *times="*", *equal="=";
const char* lt="&lt;", *gt="&gt;", *_not="&not;";
const char* leq="&leq;", *geq="&geq;", *neq="&NotEqual;", *approx="&asymp;";
const char* implies="&DoubleRightArrow;", *_and="&and;", *_or="&or;", *_xor="&CirclePlus;";
const char* mabs="|", *factorial="!";
const char *lfloor="&lfloor;", *rfloor="&rfloor;";
const char *lceil="&lceil;", *rceil="&rceil;";
const char *cardinal="#", *scalarproduct="X";
const char *_log10="<msub><mo>log</mo><mn>10</mn></msub>", *logE="<msub><mo>log</mo><mn>&ExponentialE;</mn></msub>";
}

MathMLPresentationExpressionWriter::operatorToString
    MathMLPresentationExpressionWriter::m_operatorToPresentation[] = { nullptr,
            joinOp<&plus>, joinOp<&times>,
            minus, divide, quotient,
            power, root, postfix<&factorial>,
            joinOp<&_and>,joinOp<&_or>,joinOp<&_xor>, prefix<&_not>,
            nullptr,nullptr,nullptr,nullptr,//gcd, lcm, rem, factorof,
            nullptr,nullptr,//max, min,
            joinOp<&lt>, joinOp<&gt>,
            joinOp<&equal>,
            joinOp<&neq>, joinOp<&leq>, joinOp<&geq>, joinOp<&implies>,
            joinOp<&approx>, infix<&mabs, &mabs>, infix<&lfloor, &rfloor>, infix<&lceil, &rceil>,
            // approx, abs, floor, ceiling,
            nullptr,nullptr,nullptr,// sin, cos, tan,
            nullptr,nullptr,nullptr,// sec, csc, cot,
            nullptr,nullptr,nullptr,// sinh, cosh, tanh,
            nullptr,nullptr,nullptr,// sech, csch, coth,
            nullptr,nullptr,nullptr,// arcsin, arccos, arctan,
            nullptr,// arccot,// arccoth,
            nullptr,nullptr,nullptr,// arccosh, arccsc, arccsch,
            nullptr,nullptr,nullptr,nullptr,// arcsec, arcsech, arcsinh, arctanh,
            exp, prefixOp<&_log10>, prefixOp<&logE>,// exp, ln, log,
            nullptr,nullptr,nullptr,nullptr,// //             conjugate, arg, real, imaginary,
            sum, product, diff,// sum, product, diff,
            prefix<&cardinal>, joinOp<&scalarproduct>, selector, nullptr,
            function // function
    };

MathMLPresentationExpressionWriter::MathMLPresentationExpressionWriter(const Object* o)
{
    m_result=o->accept(this);
}

QVariant MathMLPresentationExpressionWriter::visit(const Ci* var)
{
    return QVariant::fromValue<QString>(QStringLiteral("<mi>") + var->name() + QStringLiteral("</mi>"));
}

QVariant MathMLPresentationExpressionWriter::visit(const Operator* op)
{
    return op->name();
}

QVariant MathMLPresentationExpressionWriter::visit(const Cn* val)
{
    if(val->isBoolean()) {
        if(val->isTrue())
            return "<mo>true</mo>";
        else
            return "<mo>false</mo>";
    } else
        return QStringLiteral("<mn>%1</mn>").arg(val->value(), 0, 'g', 12);

}

QString piecewise(const Container* c, MathMLPresentationExpressionWriter* w)
{
    QString ret=QStringLiteral("<mrow>"
    "<mo stretchy='true'> { </mo>"
    "<mtable columnalign='left left'>");
    for(Container::const_iterator it=c->constBegin(); it!=c->constEnd(); ++it) {
        Q_ASSERT((*it)->type()==Object::container);
        Container *piece=static_cast<Container*>(*it);
        if(piece->containerType()==Container::piece) {
            ret += "<mtr>"
            "<mtd>"
                +piece->m_params.first()->accept(w).toString()+
            "</mtd>"
            "<mtd>"
                "<mtext>if </mtext>"
                +piece->m_params.last()->accept(w).toString()+
            "</mtd>"
            "</mtr>";
        } else {
            ret += "<mtr>"
            "<mtd>"
                +piece->m_params.first()->accept(w).toString()+
            "</mtd>"
            "<mtd>"
                "<mtext>otherwise</mtext>"
            "</mtd>"
            "</mtr>";
        }
    }
    
    ret+=QLatin1String("</mtable></mrow>");
    return ret;
}

QString lambda(const Container* c, MathMLPresentationExpressionWriter* w)
{
    QString ret=QStringLiteral("<mrow>");
    foreach(const Ci* bvar, c->bvarCi())
        ret+=bvar->accept(w).toString();
    ret+=QLatin1String("<mo>&RightArrow;</mo>");
    ret+=c->m_params.last()->accept(w).toString();
    ret+=QLatin1String("</mrow>");
    return ret;
}

QVariant MathMLPresentationExpressionWriter::visit(const Container* c)
{
    QString ret;
//     objectWalker(c);
//     qDebug() << "ttttttttttt" << m_operatorToPresentation << op.operatorType()
//              << m_operatorToPresentation[op.operatorType()] << op.name();
    
    switch(c->containerType()) {
        case Container::math:
            ret="<math><mrow>"+convertElements(c->constBegin(), c->constEnd(), this).join(QString())+"</mrow></math>";
            break;
        case Container::piecewise:
            ret=piecewise(c, this);
            break;
        case Container::lambda:
            ret=lambda(c, this);
            break;
        case Container::otherwise:
        case Container::piece:
        case Container::bvar:
        case Container::uplimit:
        case Container::downlimit:
        case Container::declare:
        case Container::domainofapplication:
        case Container::none:
            qDebug() << "error" << c->tagName();
            Q_ASSERT(false);
            break;
    }
    
    return ret;
}

QVariant MathMLPresentationExpressionWriter::visit(const Vector* var)
{
    return QVariant::fromValue<QString>(QStringLiteral("<mrow><mo>&lt;</mo>")+convertElements(var->constBegin(), var->constEnd(), this).join(QStringLiteral("<mo>,</mo>"))+"<mo>&gt;</mo></mrow>");
}

QVariant MathMLPresentationExpressionWriter::visit(const List* var)
{
    return QVariant::fromValue<QString>(QStringLiteral("<mrow><mo>[</mo>")+convertElements(var->constBegin(), var->constEnd(), this).join(QStringLiteral("<mo>,</mo>"))+"<mo>]</mo></mrow>");
}

QVariant MathMLPresentationExpressionWriter::visit(const Matrix* m)
{
    return QVariant::fromValue<QString>(QStringLiteral("<mrow><mo>[</mo>")+convertElements(m->constBegin(), m->constEnd(), this).join(QStringLiteral("<mo>,</mo>"))+"<mo>]</mo></mrow>");
}

QVariant MathMLPresentationExpressionWriter::visit(const MatrixRow* m)
{
    return QVariant::fromValue<QString>(QStringLiteral("<mrow><mo>[</mo>")+convertElements(m->constBegin(), m->constEnd(), this).join(QStringLiteral("<mo>,</mo>"))+"<mo>]</mo></mrow>");
}

QVariant Analitza::MathMLPresentationExpressionWriter::visit(const Analitza::Apply* a)
{
    QString ret;
    Operator op=a->firstOperator();
    
    operatorToString call=m_operatorToPresentation[op.operatorType()];
    
    if(call!=nullptr) {
        ret = call(a, this);
    } else if(op.operatorType()!=0) {
        QString bvars;
        if(!a->bvarStrings().isEmpty()) {
            bvars=a->bvarStrings().join(QString());
            if(a->bvarStrings().size()>1)
                bvars="<mfenced>"+bvars+"</mfenced>";
            const Object *ul=a->ulimit(), *dl=a->dlimit();
            if(ul || dl) {
                bvars += QLatin1String("<mo>=</mo>");
                if(dl) bvars += dl->accept(this).toString();
                bvars += QLatin1String("<mo>..</mo>");
                if(ul) bvars += ul->accept(this).toString();
            } else if(a->domain())
                bvars += "<mo>@</mo>" + a->domain()->accept(this).toString();
            bvars="<mo>:</mo>"+bvars;
        }
        
        ret="<mi>"+op.name()+"</mi>"
            "<mo> &ApplyFunction; </mo>"
            "<mfenced>"
            +convertElements(a->firstValue(), a->constEnd(), this).join(QString())
            +bvars
            +"</mfenced>";
    }
    return ret;
}

QVariant MathMLPresentationExpressionWriter::visit(const CustomObject*)
{
    return QStringLiteral("<!-- custom object -->");
}

QVariant MathMLPresentationExpressionWriter::visit(const None* )
{
    return QString();
}

