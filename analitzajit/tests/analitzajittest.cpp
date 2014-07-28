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

#include "analitza/value.h"
#include "analitzajit/jitanalyzer.h"

QTEST_MAIN( AnalitzaJitTest )

static inline bool epscompare(double a, double b)
{
  return a==b || std::abs(a-b)<std::abs(std::min(a,b))*std::numeric_limits<double>::epsilon();
}

AnalitzaJitTest::AnalitzaJitTest(QObject *parent)
 : QObject(parent)
{}

AnalitzaJitTest::~AnalitzaJitTest()
{}

void AnalitzaJitTest::initTestCase()
{
	arg1=new Analitza::Cn(0.0);
	arg2=new Analitza::Cn(0.0);
	
	a=new Analitza::JITAnalyzer;
	a->setStack(QVector<Analitza::Object*>() << arg1 << arg2);
}

void AnalitzaJitTest::cleanupTestCase()
{
	delete arg1;
	delete arg2;
	delete a;
}

void AnalitzaJitTest::testCalculateUnaryRealLambda_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<double>("arg1value");
	QTest::addColumn<double>("resultvalue");
	
	QTest::newRow("sin(0)") << "t->sin(t)" << 0.0 << 0.0;
	QTest::newRow("tan(2.3)") << "t->tan(t)" << 2.3 << -1.1192136417341325;
	QTest::newRow("-4+5") << "t->-4+5" << 0.0 << 1.0;
	QTest::newRow("0.3-5.2") << "t->0.3-5.2" << 0.0 << -4.9;
	QTest::newRow("20*0.5") << "t->20*0.5" << 0.0 << 10.0;
	QTest::newRow("3*4*5") << "t->3*4*5" << 0.0 << 60.0;
	QTest::newRow("lin") << "t->4*5*t+0.5*6" << 1.3 << 29.0;
	QTest::newRow("lin and sqr") << "t->4*5*t^2+0.5*4" << 2.2 << 98.8;
	QTest::newRow("cos const + t") << "t->cos(0)+t" << -9.0 << -8.0;
	QTest::newRow("t/2") << "t->t/2" << 5.3 << 2.65;
	QTest::newRow("t+2t") << "t->t+2*t" << 2.5 << 7.5;
	QTest::newRow("9.6+3.5") << "t->9.6+3.5" << 0.0 << 13.1;
}

void AnalitzaJitTest::testCalculateUnaryRealLambda()
{
	QFETCH(QString, expression);
	QFETCH(double, arg1value);
	QFETCH(double, resultvalue);
	
	arg1->setValue(arg1value);
	
	double result = 0;
	
	a->setExpression(Analitza::Expression(expression));
	a->calculateLambda(result);
	
	bool eq = epscompare(result, resultvalue);
	
	if (!eq) {
		qDebug() << "Actual: " << result;
		qDebug() << "Expected: " << resultvalue;
	}
	QVERIFY(eq);
}

void AnalitzaJitTest::testCalculateBinaryRealLambda_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<double>("arg1value");
	QTest::addColumn<double>("arg2value");
	QTest::addColumn<double>("resultvalue");
	
	QTest::newRow("x^2+y^2") << "(x,y)->x*x+y*y" << 3.0 << 7.5 << 65.25;
	QTest::newRow("pow(x,y)") << "(x,y)->power(x,y)" << 6.0 << 2.0 << 36.0;
	//a->setLambdaExpression(Expression("(x,y)->x+y")/*, argtipos*/);
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x=3.0000")); // true i.e 1
	
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x=3")); // x=3 => 1
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x!=3")); // x=3 => 0
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->y>3")); // y=2 => 0
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->y<3")); // y=2 => 1
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x<=3")); // x=3 => 1
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x>=3")); // x=3 => 1
// 	a->setLambdaExpression(Analitza::Expression("x->piecewise { 4=4? 3 }"));
	//a->setLambdaExpression(Analitza::Expression("x->piecewise { x<0 ? -x, x=0 ? -5, x>5 ? x*x, ? x }"));
}

void AnalitzaJitTest::testCalculateBinaryRealLambda()
{
	QFETCH(QString, expression);
	QFETCH(double, arg1value);
	QFETCH(double, arg2value);
	QFETCH(double, resultvalue);
	
	arg1->setValue(arg1value);
	arg2->setValue(arg2value);
	
	double result = 0;
	
	a->setExpression(Analitza::Expression(expression));
	a->calculateLambda(result);
	
	bool eq = epscompare(result, resultvalue);
	
	if (!eq) {
		qDebug() << "Actual: " << result;
		qDebug() << "Expected: " << resultvalue;
	}
	QVERIFY(eq);
}

#include "analitzajittest.moc"
