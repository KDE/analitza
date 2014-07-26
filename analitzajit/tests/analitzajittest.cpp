/*************************************************************************************
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "analitzajittest.h"

#include <QtTest/QTest>

#include "jitanalyzer.h"
#include <value.h>
#include <container.h>
#include "analitza/expressiontype.h"
#include <llvm/IR/Value.h>
#include <llvm-3.3/llvm/IR/Constants.h>

QTEST_MAIN( AnalitzaJitTest )

AnalitzaJitTest::AnalitzaJitTest(QObject *parent)
 : QObject(parent)
{}

AnalitzaJitTest::~AnalitzaJitTest()
{}

void AnalitzaJitTest::initTestCase()
{
	a=new Analitza::JitAnalyzer;
}

void AnalitzaJitTest::cleanupTestCase()
{
	delete a;
}

void AnalitzaJitTest::testCalculate_data()
{
//TODO
}

#include <llvm/Support/raw_ostream.h>
using namespace Analitza;
void AnalitzaJitTest::testCalculate()
{
//TODO this is just a foo test
// 	std::string str;
// 	llvm::raw_string_ostream stringwriter(str);
// 	a->foojiteval()->print(stringwriter);
// 	
// 	qDebug() << QString::fromStdString(str);
	
	Analitza::Cn* val1 = new Analitza::Cn(3);
// 	Analitza::Cn* val2 = new Analitza::Cn(2);
// 	Analitza::Cn* val3 = new Analitza::Cn(7);
	QStack<Analitza::Object*> stack;
	stack.push(val1);
// 	stack.push(val2);
// 	stack.push(val3);
	
	//a->setLambdaExpression(Analitza::Expression("t->sin(t)"));
	//a->setLambdaExpression(Analitza::Expression("t->4+5"));
	//a->setLambdaExpression(Analitza::Expression("t->0.3-5.2"));s
	//a->setLambdaExpression(Analitza::Expression("t->20*0.5"));
	//a->setLambdaExpression(Analitza::Expression("t->20*0.5"));
	//a->setLambdaExpression(Analitza::Expression("t->3*4*5"));
	//a->setLambdaExpression(Analitza::Expression("t->4*5*t+0.5*6"));
	//a->setLambdaExpression(Analitza::Expression("t->4*5*t^2+0.5*6"));
	//a->setLambdaExpression(Analitza::Expression("t->cos(0)+t"));
	//a->setLambdaExpression(Analitza::Expression("t->t/2"));
	//a->setLambdaExpression(Analitza::Expression("t->tan(0)+t"));
// 	a->setLambdaExpression(Analitza::Expression("t->tan(2.3)")); //-1.1192136
	//a->setLambdaExpression(Analitza::Expression("(x,y)->x*x+y*y"));
	//a->setLambdaExpression(Analitza::Expression("(x,y,z)->x+y*z"));
	//a->setLambdaExpression(Analitza::Expression("(x,y,z)->-9"));
	//ExpressionType mt = ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -2);
	
	//BEGIN experimental
// 	ExpressionType mt = ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1);
// 	
// 	QMap<QString, Analitza::ExpressionType> argtipos;
// 	argtipos["x"] = mt;
// 	argtipos["y"] = mt;
	//END experimental
	//a->setLambdaExpression(Expression("(x,y)->x+y")/*, argtipos*/);
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x=3.0000")); // true i.e 1
	
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x=3")); // x=3 => 1
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x!=3")); // x=3 => 0
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->y>3")); // y=2 => 0
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->y<3")); // y=2 => 1
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x<=3")); // x=3 => 1
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x>=3")); // x=3 => 1
	a->setLambdaExpression(Analitza::Expression("x->piecewise { 4=4? 3 }"));
	//a->setLambdaExpression(Analitza::Expression("x->piecewise { x<0 ? -x, x=0 ? -5, x>5 ? x*x, ? x }"));
	
	
	a->setStack(stack);
	a->foojiteval();
// 	qDebug() << "PEPEPEPEPE " << ((llvm::ConstantFP*)(a->foojiteval()))->getValueAPF().convertToDouble();
	
	qDeleteAll(stack);
	
// 	Analitza::Analyzer aa;
// 	aa.setExpression(Analitza::Expression("t->vector {t,t**2,t}"));
// 	aa.calculateLambda();
// 	qDebug() << "s0 " << aa.evaluate().toString();
	
// 	Analitza::Container *cc = (Analitza::Container*)(aa.expression().tree());
// 	cc = (Analitza::Container*)(cc->m_params.first());
// 	qDebug() << "ty " << cc->containerType();
// 	
// 	
// 	
// 	qDebug() << "s1 " << aa.calculateLambda().toString();
	
}

#include "analitzajittest.moc"
