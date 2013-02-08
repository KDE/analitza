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
#include <analitza/localize.h>
#include <QFont>

using Analitza::Operator;

OperatorsModel::OperatorsModel(QObject *parent) : QAbstractTableModel(parent), m_vars(0)
{
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
	}
	return ret;
}

QVariant OperatorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant ret;
	if(role==Qt::DisplayRole && orientation==Qt::Horizontal) {
		switch(section) {
			case 0:
				ret=i18nc("@title:column", "Name");
				break;
			case 1:
				ret=i18nc("@title:column", "Description");
				break;
			case 2:
				ret=i18nc("@title:column", "Parameters");
				break;
			case 3:
				ret=i18nc("@title:column", "Example");
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
	reset();
}

QString OperatorsModel::sample(const Analitza::Operator& oper)
{
	QString funcname=oper.toString();
	QString bounds;
	if(oper.isBounded()) {
		bounds=i18nc("Syntax for function bounding", " : var");
		if(oper.operatorType()==Operator::sum || oper.operatorType()==Operator::product)
			bounds += i18nc("Syntax for function bounding values", "=from..to");
	}
	
	QString sample = i18n("%1(", funcname);
	
	if(oper.nparams()<0) {
		return i18n("%1... parameters, ...%2)", sample, bounds);
	} else {
		for(int i=0; i<oper.nparams(); ++i) {
			sample += i18n("par%1", i+1);
			if(i<oper.nparams()-1)
				sample += ", ";
		}
		return sample+bounds+')';
	}
}

QString OperatorsModel::description(const Analitza::Operator& o)
{
	QString s;
	switch(o.operatorType()) {
		case Operator::plus:
			s = i18n("Addition");
			break;
		case Operator::times:
			s = i18n("Multiplication");
			break;
		case Operator::divide:
			s = i18n("Division");
			break;
		case Operator::minus:
			s = i18n("Subtraction. Will remove all values from the first one.");
			break;
		case Operator::power:
			s = i18n("Power");
			break;
		case Operator::rem:
			s = i18n("Remainder");
			break;
		case Operator::quotient:
			s = i18n("Quotient");
			break;
		case Operator::factorof:
			s = i18n("The factor of");
			break;
		case Operator::factorial:
			s = i18n("Factorial. factorial(n)=n!");
			break;
		case Operator::sin:
			s = i18n("Function to calculate the sine of a given angle");
			break;
		case Operator::cos:
			s = i18n("Function to calculate the cosine of a given angle");
			break;
		case Operator::tan:
			s = i18n("Function to calculate the tangent of a given angle");
			break;
		case Operator::sec:
			s = i18n("Secant");
			break;
		case Operator::csc:
			s = i18n("Cosecant");
			break;
		case Operator::cot:
			s = i18n("Cotangent");
			break;
		case Operator::sinh:
			s = i18n("Hyperbolic sine");
			break;
		case Operator::cosh:
			s = i18n("Hyperbolic cosine");
			break;
		case Operator::tanh:
			s = i18n("Hyperbolic tangent");
			break;
		case Operator::sech:
			s = i18n("Hyperbolic secant");
			break;
		case Operator::csch:
			s = i18n("Hyperbolic cosecant");
			break;
		case Operator::coth:
			s = i18n("Hyperbolic cotangent");
			break;
		case Operator::arcsin:
			s = i18n("Arc sine");
			break;
		case Operator::arccos:
			s = i18n("Arc cosine");
			break;
		case Operator::arctan:
			s = i18n("Arc tangent");
			break;
		case Operator::arccot:
			s = i18n("Arc cotangent");
			break;
// 		case Operator::arccoth:
// 			s = i18n("Hyperbolic arc cotangent");
// 			break;
		case Operator::arctanh:
			s = i18n("Hyperbolic arc tangent");
			break;
		case Operator::sum:
			s = i18n("Summatory");
			break;
		case Operator::product:
			s = i18n("Productory");
			break;
		case Operator::forall:
			s = i18n("For all");
			break;
		case Operator::exists:
			s = i18n("Exists");
			break;
		case Operator::diff:
			s = i18n("Differentiation");
			break;
		case Operator::arcsinh:
			s = i18n("Hyperbolic arc sine");
			break;
		case Operator::arccosh:
			s = i18n("Hyperbolic arc cosine");
			break;
		case Operator::arccsc:
			s = i18n("Arc cosecant");
			break;
		case Operator::arccsch:
			s = i18n("Hyperbolic arc cosecant");
			break;
		case Operator::arcsec:
			s = i18n("Arc secant");
			break;
		case Operator::arcsech:
			s = i18n("Hyperbolic arc secant");
			break;
		case Operator::exp:
			s = i18n("Exponent (e^x)");
			break;
		case Operator::ln:
			s = i18n("Base-e logarithm");
			break;
		case Operator::log:
			s = i18n("Base-10 logarithm");
			break;
		case Operator::abs:
			s = i18n("Absolute value. abs(n)=|n|");
			break;
// 		case Operator::conjugate:
// 			s = i18n("Conjugate");
// 			break;
// 		case Operator::arg:
// 			s = "---";//i18n("Arg?");
// 			break;
// 		case Operator::real:
// 			s = i18n("Real");
// 			break;
// 		case Operator::imaginary:
// 			s = i18n("Imaginary");
// 			break;
		case Operator::floor:
			s = i18n("Floor value. floor(n)=⌊n⌋");
			break;
		case Operator::ceiling:
			s = i18n("Ceil value. ceil(n)=⌈n⌉");
			break;
		case Operator::min:
			s = i18n("Minimum");
			break;
		case Operator::max:
			s = i18n("Maximum");
			break;
		case Operator::gt:
			s = i18n("Greater than. gt(a,b)=a>b");
			break;
		case Operator::lt:
			s = i18n("Less than. lt(a,b)=a<b");
			break;
		case Operator::eq:
			s = i18n("Equal. eq(a,b) = a=b");
			break;
		case Operator::approx:
			s = i18n("Approximation. approx(a)=a±n");
			break;
		case Operator::neq:
			s = i18n("Not equal. neq(a,b)=a≠b");
			break;
		case Operator::geq:
			s = i18n("Greater or equal. geq(a,b)=a≥b");
			break;
		case Operator::leq:
			s = i18n("Less or equal. leq(a,b)=a≤b");
			break;
		case Operator::_and:
			s = i18n("Boolean and");
			break;
		case Operator::_not:
			s = i18n("Boolean not");
			break;
		case Operator::_or:
			s = i18n("Boolean or");
			break;
		case Operator::_xor:
			s = i18n("Boolean xor");
			break;
		case Operator::implies:
			s = i18n("Boolean implication");
			break;
		case Operator::gcd:
			s = i18n("Greatest common divisor");
			break;
		case Operator::lcm:
			s = i18n("Least common multiple");
			break;
		case Operator::root:
			s = i18n("Root");
			break;
		case Operator::card:
			s = i18n("Cardinal");
			break;
		case Operator::scalarproduct:
			s = i18n("Scalar product");
			break;
		case Operator::selector:
			s = i18n("Select the par1-th element of par2 list or vector");
			break;
		case Operator::_union:
			s = i18n("Joins several items of the same type");
			break;
		case Operator::map:
			s = i18n("Applies a function to every element in a list");
			break;
		case Operator::filter:
			s = i18n("Removes all elements that don't fit a condition");
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
			s="x+2";
			break;
		case Operator::times:
			s="x*2";
			break;
		case Operator::divide:
			s="x/2";
			break;
		case Operator::minus:
			s="x-2";
			break;
		case Operator::power:
			s="x^2";
			break;
		case Operator::rem:
			s="rem(x, 5)";
			break;
		case Operator::quotient:
			s="quotient(x, 2)";
			break;
		case Operator::factorof:
			s="factorof(x, 3)";
			break;
		case Operator::min:
			s="min(x, 4)";
			break;
		case Operator::max:
			s="max(x, 4)";
			break;
		case Operator::gt:
			s="piecewise { x>4 ? 1, ? 0 }";
			break;
		case Operator::lt:
			s="piecewise { x<4 ? 1, ? 0 }";
			break;
		case Operator::eq:
			s="piecewise { x=4 ? 1, ? 0 }";
			break;
		case Operator::approx:
			s="piecewise { approx(x, 4) ? 1, ? 0 }";
			break;
		case Operator::neq:
			s="piecewise { x!=4 ? 1, ? 0 }";
			break;
		case Operator::geq:
			s="piecewise { x>=4 ? 1, ? 0 }";
			break;
		case Operator::leq:
			s="piecewise { x<=4 ? 1, ? 0 }";
			break;
		case Operator::_and:
			s="piecewise { and(x>-2, x<2) ? 1, ? 0 }";
			break;
		case Operator::_or:
			s="piecewise { or(x>2, x>-2) ? 1, ? 0 }";
			break;
		case Operator::_xor:
			s="piecewise { xor(x>0, x<3) ? 1, ? 0 }";
			break;
		case Operator::implies:
			s="piecewise { implies(x<0, x<3) ? 1, ? 0 }";
			break;
		case Operator::forall:
			s="piecewise { forall(t:t@list { true, false, false }) ? 1, ? 0 }";
			break;
		case Operator::exists:
			s="piecewise { exists(t:t@list { true, false, false }) ? 1, ? 0 }";
			break;
		case Operator::_not:
			s="piecewise { not(x>0) ? 1, ? 0 }";
			break;
		case Operator::gcd:
			s="gcd(x, 3)";
			break;
		case Operator::lcm:
			s="lcm(x, 4)";
			break;
		case Operator::root:
			s="root(x, 2)";
			break;
		case Operator::selector:
			s="scalarproduct(vector { 0, x }, vector { x, 0 })[1]";
			break;
		case Operator::sum:
			s="x*sum(t*t:t=0..3)";
			break;
		case Operator::product:
			s="product(t+t:t=1..3)";
			break;
		case Operator::card:
			s="card(vector { x, 1, 2 })";
			break;
		case Operator::scalarproduct:
			s="scalarproduct(vector { 0, x }, vector { x, 0 })[1]";
			break;
		case Operator::diff:
			s="(diff(x^2:x))(x)";
			break;
		case Operator::_union:
			s="union(list { 1, 2, 3 }, list { 4, 5, 6 })[rem(floor(x), 5)+3]";
			break;
		case Operator::map:
			s=QString("map(x->x+x, list { 1, 2, 3, 4, 5, 6 })[rem(floor(x), 5)+3]").arg(o.toString());
			break;
		case Operator::filter:
			s=QString("filter(u->rem(u, 2)=0, list { 2, 4, 3, 4, 8, 6 })[rem(floor(x), 5)+3]").arg(o.toString());
			break;
		case Operator::factorial:
		case Operator::arcsech:
		case Operator::arcsec:
		case Operator::arccsch:
		case Operator::arccsc:
// 		case Operator::arccoth:
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
// 		case Operator::arccsc:
// 		case Operator::arccsch:
// 		case Operator::arcsec:
// 		case Operator::arcsech:
		case Operator::arctanh:
		case Operator::exp:
		case Operator::ln:
		case Operator::log:
		case Operator::abs:
		//case Object::conjugate:
		//case Object::arg:
		//case Object::real:
		//case Object::imaginary:
		case Operator::floor:
		case Operator::ceiling:
			s=QString("%1(x)").arg(o.toString());
			break;
		case Operator::nOfOps:
		case Operator::none:
		case Operator::function:
// 		case Operator::real:
// 		case Operator::conjugate:
// 		case Operator::arg:
// 		case Operator::imaginary:
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
		ret=i18nc("n-ary function prototype", "<em>%1</em>(..., <b>par%2</b>, ...)",
							funcname, param+1);
	} else {
		ret=standardFunctionCallHelp(funcname, param, op, inbounds, oper.isBounded());
	}
	return ret;
}

QString OperatorsModel::standardFunctionCallHelp(const QString& funcname, int param, int paramcount, bool inbounds, bool isbounded)
{
	QString sample = (param < paramcount || (inbounds && isbounded)) ?
		i18nc("Function name in function prototype", "<em>%1</em>(", funcname) :
		i18nc("Uncorrect function name in function prototype", "<em style='color:red'><b>%1</b></em>(", funcname);
		
	for(int i=0; i<paramcount; ++i) {
		QString current=i18nc("Parameter in function prototype", "par%1", i+1);
		
		if(i==param)
			current=i18nc("Current parameter in function prototype", "<b>%1</b>", current);
		sample += current;
		if(i<paramcount-1)
			sample += i18nc("Function parameter separator", ", ");
	}
	
	if(isbounded) {
		static QString bounds=i18nc("Current parameter is the bounding", " : bounds");
		QString p=bounds;
		if(inbounds)
			p=i18nc("Current parameter in function prototype", "<b>%1</b>", p);
		sample += p;
	}
	
	return sample+')';

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


