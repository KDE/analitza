/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
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

#include "operatorsmodel.h"
#include <analitza/operator.h>
#include <analitza/variables.h>
#include <QFont>
#include <QCoreApplication>

using Analitza::Operator;

OperatorsModel::OperatorsModel(QObject *parent) : QAbstractTableModel(parent), m_vars(nullptr)
{
}

QHash<int, QByteArray> OperatorsModel::roleNames() const
{
    auto ret = QAbstractTableModel::roleNames();
    ret.insert(IsVariableRole, "isVariable");
    ret.insert(DescriptionRole, "description");
    return ret;
}

QVariant OperatorsModel::data(const QModelIndex & index, int role) const
{
    QVariant ret;
    if(role==Qt::DisplayRole) {
        if(index.row()<Analitza::Operator::nOfOps-2) {
            Analitza::Operator oper((Analitza::Operator::OperatorType) (index.row()+1));
            
            switch(index.column()) {
                case 0:
                    ret=oper.toString();
                    break;
                case 1:
                    ret=description(oper);
                    break;
                case 2:
                    ret=sample(oper);
                    break;
                case 3:
                    ret=example(oper);
                    break;
            }
        } else if(m_vars) {
            int var=index.row()-Analitza::Operator::nOfOps+2;
            QString key=m_vars->keys()[var];
            switch(index.column()) {
                case 0:
                    ret=key;
                    break;
                case 1:
                    ret=m_vars->value(key)->toString();
                    break;
            }
        }
    } else if(role==Qt::FontRole && index.column()==1) {
        QFont f;
        f.setItalic(true);
        ret=f;
    } else if(role==DescriptionRole && index.column()==0) {
        Analitza::Operator oper((Analitza::Operator::OperatorType) (index.row()+1));
        switch(index.column()) {
            case 0:
                ret=description(oper);
                break;
        }
    } else if(role==IsVariableRole && index.column()==0) {
        ret=index.row()<Analitza::Operator::nOfOps-2;
    }
    return ret;
}

QVariant OperatorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal) {
        switch(section) {
            case 0:
                ret=QCoreApplication::translate("@title:column", "Name");
                break;
            case 1:
                ret=QCoreApplication::translate("@title:column", "Description");
                break;
            case 2:
                ret=QCoreApplication::translate("@title:column", "Parameters");
                break;
            case 3:
                ret=QCoreApplication::translate("@title:column", "Example");
                break;
        }
    }
    return ret;
}

int OperatorsModel::rowCount(const QModelIndex &) const
{
    int count=Analitza::Operator::nOfOps;
    if(m_vars)
        count+=m_vars->count();
    return count-2;
}

int OperatorsModel::columnCount(const QModelIndex &) const
{
    return 4;
}

void OperatorsModel::updateInformation()
{
    beginResetModel();
    endResetModel();
}

QString OperatorsModel::sample(const Analitza::Operator& oper)
{
    QString funcname=oper.toString();
    QString bounds;
    if(oper.isBounded()) {
        bounds=QCoreApplication::translate("Syntax for function bounding", " : var");
        if(oper.operatorType()==Operator::sum || oper.operatorType()==Operator::product)
            bounds += QCoreApplication::translate("Syntax for function bounding values", "=from..to");
    }
    
    QString sample = QCoreApplication::tr("%1(").arg(funcname);
    
    if(oper.nparams()<0) {
        return QCoreApplication::tr("%1... parameters, ...%2)").arg(sample, bounds);
    } else {
        for(int i=0; i<oper.nparams(); ++i) {
            sample += QCoreApplication::tr("par%1").arg(i+1);
            if(i<oper.nparams()-1)
                sample += QLatin1String(", ");
        }
        return sample+bounds+')';
    }
}

QString OperatorsModel::description(const Analitza::Operator& o)
{
    QString s;
    switch(o.operatorType()) {
        case Operator::plus:
            s = QCoreApplication::tr("Addition");
            break;
        case Operator::times:
            s = QCoreApplication::tr("Multiplication");
            break;
        case Operator::divide:
            s = QCoreApplication::tr("Division");
            break;
        case Operator::minus:
            s = QCoreApplication::tr("Subtraction. Will remove all values from the first one.");
            break;
        case Operator::power:
            s = QCoreApplication::tr("Power");
            break;
        case Operator::rem:
            s = QCoreApplication::tr("Remainder");
            break;
        case Operator::quotient:
            s = QCoreApplication::tr("Quotient");
            break;
        case Operator::factorof:
            s = QCoreApplication::tr("The factor of");
            break;
        case Operator::factorial:
            s = QCoreApplication::tr("Factorial. factorial(n)=n!");
            break;
        case Operator::sin:
            s = QCoreApplication::tr("Function to calculate the sine of a given angle");
            break;
        case Operator::cos:
            s = QCoreApplication::tr("Function to calculate the cosine of a given angle");
            break;
        case Operator::tan:
            s = QCoreApplication::tr("Function to calculate the tangent of a given angle");
            break;
        case Operator::sec:
            s = QCoreApplication::tr("Secant");
            break;
        case Operator::csc:
            s = QCoreApplication::tr("Cosecant");
            break;
        case Operator::cot:
            s = QCoreApplication::tr("Cotangent");
            break;
        case Operator::sinh:
            s = QCoreApplication::tr("Hyperbolic sine");
            break;
        case Operator::cosh:
            s = QCoreApplication::tr("Hyperbolic cosine");
            break;
        case Operator::tanh:
            s = QCoreApplication::tr("Hyperbolic tangent");
            break;
        case Operator::sech:
            s = QCoreApplication::tr("Hyperbolic secant");
            break;
        case Operator::csch:
            s = QCoreApplication::tr("Hyperbolic cosecant");
            break;
        case Operator::coth:
            s = QCoreApplication::tr("Hyperbolic cotangent");
            break;
        case Operator::arcsin:
            s = QCoreApplication::tr("Arc sine");
            break;
        case Operator::arccos:
            s = QCoreApplication::tr("Arc cosine");
            break;
        case Operator::arctan:
            s = QCoreApplication::tr("Arc tangent");
            break;
        case Operator::arccot:
            s = QCoreApplication::tr("Arc cotangent");
            break;
//         case Operator::arccoth:
//             s = QCoreApplication::tr("Hyperbolic arc cotangent");
//             break;
        case Operator::arctanh:
            s = QCoreApplication::tr("Hyperbolic arc tangent");
            break;
        case Operator::sum:
            s = QCoreApplication::tr("Summatory");
            break;
        case Operator::product:
            s = QCoreApplication::tr("Productory");
            break;
        case Operator::forall:
            s = QCoreApplication::tr("For all");
            break;
        case Operator::exists:
            s = QCoreApplication::tr("Exists");
            break;
        case Operator::diff:
            s = QCoreApplication::tr("Differentiation");
            break;
        case Operator::arcsinh:
            s = QCoreApplication::tr("Hyperbolic arc sine");
            break;
        case Operator::arccosh:
            s = QCoreApplication::tr("Hyperbolic arc cosine");
            break;
        case Operator::arccsc:
            s = QCoreApplication::tr("Arc cosecant");
            break;
        case Operator::arccsch:
            s = QCoreApplication::tr("Hyperbolic arc cosecant");
            break;
        case Operator::arcsec:
            s = QCoreApplication::tr("Arc secant");
            break;
        case Operator::arcsech:
            s = QCoreApplication::tr("Hyperbolic arc secant");
            break;
        case Operator::exp:
            s = QCoreApplication::tr("Exponent (e^x)");
            break;
        case Operator::ln:
            s = QCoreApplication::tr("Base-e logarithm");
            break;
        case Operator::log:
            s = QCoreApplication::tr("Base-10 logarithm");
            break;
        case Operator::abs:
            s = QCoreApplication::tr("Absolute value. abs(n)=|n|");
            break;
        case Operator::conjugate:
            s = QCoreApplication::tr("Conjugate");
            break;
        case Operator::arg:
            s = QCoreApplication::tr("Arg");
            break;
        case Operator::real:
            s = QCoreApplication::tr("Real");
            break;
        case Operator::imaginary:
            s = QCoreApplication::tr("Imaginary");
            break;
        case Operator::floor:
            s = QCoreApplication::tr("Floor value. floor(n)=⌊n⌋");
            break;
        case Operator::ceiling:
            s = QCoreApplication::tr("Ceil value. ceil(n)=⌈n⌉");
            break;
        case Operator::min:
            s = QCoreApplication::tr("Minimum");
            break;
        case Operator::max:
            s = QCoreApplication::tr("Maximum");
            break;
        case Operator::gt:
            s = QCoreApplication::tr("Greater than. gt(a,b)=a>b");
            break;
        case Operator::lt:
            s = QCoreApplication::tr("Less than. lt(a,b)=a<b");
            break;
        case Operator::eq:
            s = QCoreApplication::tr("Equal. eq(a,b) = a=b");
            break;
        case Operator::approx:
            s = QCoreApplication::tr("Approximation. approx(a)=a±n");
            break;
        case Operator::neq:
            s = QCoreApplication::tr("Not equal. neq(a,b)=a≠b");
            break;
        case Operator::geq:
            s = QCoreApplication::tr("Greater or equal. geq(a,b)=a≥b");
            break;
        case Operator::leq:
            s = QCoreApplication::tr("Less or equal. leq(a,b)=a≤b");
            break;
        case Operator::_and:
            s = QCoreApplication::tr("Boolean and");
            break;
        case Operator::_not:
            s = QCoreApplication::tr("Boolean not");
            break;
        case Operator::_or:
            s = QCoreApplication::tr("Boolean or");
            break;
        case Operator::_xor:
            s = QCoreApplication::tr("Boolean xor");
            break;
        case Operator::implies:
            s = QCoreApplication::tr("Boolean implication");
            break;
        case Operator::gcd:
            s = QCoreApplication::tr("Greatest common divisor");
            break;
        case Operator::lcm:
            s = QCoreApplication::tr("Least common multiple");
            break;
        case Operator::root:
            s = QCoreApplication::tr("Root");
            break;
        case Operator::card:
            s = QCoreApplication::tr("Cardinal");
            break;
        case Operator::scalarproduct:
            s = QCoreApplication::tr("Scalar product");
            break;
        case Operator::selector:
            s = QCoreApplication::tr("Select the par1-th element of par2 list or vector");
            break;
        case Operator::_union:
            s = QCoreApplication::tr("Joins several items of the same type");
            break;
        case Operator::map:
            s = QCoreApplication::tr("Applies a function to every element in a list");
            break;
        case Operator::filter:
            s = QCoreApplication::tr("Removes all elements that don't fit a condition");
            break;
        case Operator::transpose:
            s = QCoreApplication::tr("Transpose");
            break;
        case Operator::function:
        case Operator::nOfOps:
        case Operator::none:
            break;
    }
    return s;
}

QString OperatorsModel::example(const Analitza::Operator& o)
{
    QString s;
    
    switch(o.operatorType()) {
        case Operator::plus:
            s=QStringLiteral("x+2");
            break;
        case Operator::times:
            s=QStringLiteral("x*2");
            break;
        case Operator::divide:
            s=QStringLiteral("x/2");
            break;
        case Operator::minus:
            s=QStringLiteral("x-2");
            break;
        case Operator::power:
            s=QStringLiteral("x^2");
            break;
        case Operator::rem:
            s=QStringLiteral("rem(x, 5)");
            break;
        case Operator::quotient:
            s=QStringLiteral("quotient(x, 2)");
            break;
        case Operator::factorof:
            s=QStringLiteral("factorof(x, 3)");
            break;
        case Operator::min:
            s=QStringLiteral("min(x, 4)");
            break;
        case Operator::max:
            s=QStringLiteral("max(x, 4)");
            break;
        case Operator::gt:
            s=QStringLiteral("piecewise { x>4 ? 1, ? 0 }");
            break;
        case Operator::lt:
            s=QStringLiteral("piecewise { x<4 ? 1, ? 0 }");
            break;
        case Operator::eq:
            s=QStringLiteral("piecewise { x=4 ? 1, ? 0 }");
            break;
        case Operator::approx:
            s=QStringLiteral("piecewise { approx(x, 4) ? 1, ? 0 }");
            break;
        case Operator::neq:
            s=QStringLiteral("piecewise { x!=4 ? 1, ? 0 }");
            break;
        case Operator::geq:
            s=QStringLiteral("piecewise { x>=4 ? 1, ? 0 }");
            break;
        case Operator::leq:
            s=QStringLiteral("piecewise { x<=4 ? 1, ? 0 }");
            break;
        case Operator::_and:
            s=QStringLiteral("piecewise { and(x>-2, x<2) ? 1, ? 0 }");
            break;
        case Operator::_or:
            s=QStringLiteral("piecewise { or(x>2, x>-2) ? 1, ? 0 }");
            break;
        case Operator::_xor:
            s=QStringLiteral("piecewise { xor(x>0, x<3) ? 1, ? 0 }");
            break;
        case Operator::implies:
            s=QStringLiteral("piecewise { implies(x<0, x<3) ? 1, ? 0 }");
            break;
        case Operator::forall:
            s=QStringLiteral("piecewise { forall(t:t@list { true, false, false }) ? 1, ? 0 }");
            break;
        case Operator::exists:
            s=QStringLiteral("piecewise { exists(t:t@list { true, false, false }) ? 1, ? 0 }");
            break;
        case Operator::_not:
            s=QStringLiteral("piecewise { not(x>0) ? 1, ? 0 }");
            break;
        case Operator::gcd:
            s=QStringLiteral("gcd(x, 3)");
            break;
        case Operator::lcm:
            s=QStringLiteral("lcm(x, 4)");
            break;
        case Operator::root:
            s=QStringLiteral("root(x, 2)");
            break;
        case Operator::selector:
            s=QStringLiteral("scalarproduct(vector { 0, x }, vector { x, 0 })[1]");
            break;
        case Operator::sum:
            s=QStringLiteral("x*sum(t*t:t=0..3)");
            break;
        case Operator::product:
            s=QStringLiteral("product(t+t:t=1..3)");
            break;
        case Operator::card:
            s=QStringLiteral("card(vector { x, 1, 2 })");
            break;
        case Operator::scalarproduct:
            s=QStringLiteral("scalarproduct(vector { 0, x }, vector { x, 0 })[1]");
            break;
        case Operator::diff:
            s=QStringLiteral("(diff(x^2:x))(x)");
            break;
        case Operator::_union:
            s=QStringLiteral("union(list { 1, 2, 3 }, list { 4, 5, 6 })[rem(floor(x), 5)+3]");
            break;
        case Operator::map:
            s=QStringLiteral("map(x->x+x, list { 1, 2, 3, 4, 5, 6 })[rem(floor(x), 5)+3]");
            break;
        case Operator::filter:
            s=QStringLiteral("filter(u->rem(u, 2)=0, list { 2, 4, 3, 4, 8, 6 })[rem(floor(x), 5)+3]");
            break;
        case Operator::transpose:
            s = QStringLiteral("transpose(matrix { matrixrow { 1, 2, 3, 4, 5, 6 } })[rem(floor(x), 5)+3][1]");
            break;
        case Operator::real:
            s = QStringLiteral("real(x*i)");
            break;
        case Operator::conjugate:
            s = QStringLiteral("conjugate(x*i)");
            break;
        case Operator::arg:
            s = QStringLiteral("arg(x*i)");
            break;
        case Operator::imaginary:
            s = QStringLiteral("imaginary(x*i)");
            break;
        case Operator::factorial:
        case Operator::arcsech:
        case Operator::arcsec:
        case Operator::arccsch:
        case Operator::arccsc:
//         case Operator::arccoth:
        case Operator::sin:
        case Operator::cos:
        case Operator::tan:
        case Operator::sec:
        case Operator::csc:
        case Operator::cot:
        case Operator::sinh:
        case Operator::cosh:
        case Operator::tanh:
        case Operator::sech:
        case Operator::csch:
        case Operator::coth:
        case Operator::arcsin:
        case Operator::arccos:
        case Operator::arctan:
        case Operator::arccot:
        case Operator::arcsinh:
        case Operator::arccosh:
//         case Operator::arccsc:
//         case Operator::arccsch:
//         case Operator::arcsec:
//         case Operator::arcsech:
        case Operator::arctanh:
        case Operator::exp:
        case Operator::ln:
        case Operator::log:
        case Operator::abs:
        case Operator::floor:
        case Operator::ceiling:
            s=QStringLiteral("%1(x)").arg(o.toString());
            break;
        case Operator::nOfOps:
        case Operator::none:
        case Operator::function:
            break;
    }
    return "x->"+s;
}

QModelIndex OperatorsModel::indexForOperatorName(const QString& id) const
{
    Operator::OperatorType opt=Analitza::Operator::toOperatorType(id);
    if(opt==Operator::none)
        return QModelIndex();
    else
        return index(opt-1, 0);
}

QString OperatorsModel::parameterHelp(const QModelIndex& index, int param, bool inbounds) const
{
    Q_ASSERT(index.isValid());
    QString ret;
    Analitza::Operator oper((Analitza::Operator::OperatorType) (index.row()+1));
    QString funcname = oper.toString();
    const int op=oper.nparams();
    if(op == -1) {
        ret=QCoreApplication::translate("n-ary function prototype", "<em>%1</em>(..., <b>par%2</b>, ...)").arg(funcname).arg(param+1);
    } else {
        ret=standardFunctionCallHelp(funcname, param, op, inbounds, oper.isBounded());
    }
    return ret;
}

QString OperatorsModel::standardFunctionCallHelp(const QString& funcname, int param, int paramcount, bool inbounds, bool isbounded)
{
    QString sample = (param < paramcount || (inbounds && isbounded)) ?
        QCoreApplication::translate("Function name in function prototype", "<em>%1</em>(").arg(funcname) :
        QCoreApplication::translate("Uncorrect function name in function prototype", "<em style='color:red'><b>%1</b></em>(").arg(funcname);
        
    for(int i=0; i<paramcount; ++i) {
        QString current=QCoreApplication::translate("Parameter in function prototype", "par%1").arg(i+1);
        
        if(i==param)
            current=QCoreApplication::translate("Current parameter in function prototype", "<b>%1</b>").arg(current);
        sample += current;
        if(i<paramcount-1)
            sample += QCoreApplication::translate("Function parameter separator", ", ");
    }
    
    if(isbounded) {
        static QString bounds=QCoreApplication::translate("Current parameter is the bounding", " : bounds");
        QString p=bounds;
        if(inbounds)
            p=QCoreApplication::translate("Current parameter in function prototype", "<b>%1</b>").arg(p);
        sample += p;
    }
    
    return sample+')';

}

QString OperatorsModel::lastWord(int pos, const QString& exp)
{
    int act=pos-1;
    for(; act>=0 && exp[act].isLetter(); act--) {}

    return exp.mid(act+1, pos-act-1);
}

/*QString OperatorsModel::operToString(const Operator& op) const
{
    QStandardItem *it;
    
    for(int i=0; i<KEYWORDNUM; i++) {
        it=item(i,2);
        if(it!=NULL && it->data(Qt::EditRole).toInt()==op.operatorType()) {
            return item(i,0)->data(Qt::EditRole).toString();
}
}
    return QString();
}*/


