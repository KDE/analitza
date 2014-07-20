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

void AnalitzaJitTest::testCalculate()
{
//TODO this is just a foo test
// 	std::string str;
// 	llvm::raw_string_ostream stringwriter(str);
// 	a->foojiteval()->print(stringwriter);
// 	
// 	qDebug() << QString::fromStdString(str);
	
	Analitza::Cn* val = new Analitza::Cn(89);
	QStack<Analitza::Object*> stack;
	stack.push(val);
	
	//a->setLambdaExpression(Analitza::Expression("t->sin(t)"));
	a->setLambdaExpression(Analitza::Expression("t->4+5"));
	a->setStack(stack);
	a->foojiteval();
// 	qDebug() << "PEPEPEPEPE " << ((llvm::ConstantFP*)(a->foojiteval()))->getValueAPF().convertToDouble();
	
	delete val;
	
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
