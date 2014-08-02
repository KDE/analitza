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

static inline bool epscompare(QVector<double> a, QVector<double> b)
{
	bool ret = (a.size() == b.size());
	
	for (int i = 0; (i < a.size()) && ret; ++i) {
		ret = epscompare(a.at(i), b.at(i));
	}
	
	return ret;
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

void AnalitzaJitTest::testCalculateUnaryBooleanLambda_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<double>("arg1value");
	QTest::addColumn<bool>("expected");
	
	QTest::newRow("1=2") << "t->t=2" << 1.0 << false;
	QTest::newRow("1!=2") << "t->t!=2" << 1.0 << true;
	QTest::newRow("-9!=2") << "t->t!=2" << -9.0 << true;
	QTest::newRow("2>3") << "t->t>3" << 2.0 << false;
	QTest::newRow("2<3") << "t->t<3" << 2.0 << true;
	QTest::newRow("3>=3") << "t->t>=3" << 3.0 << true;
	QTest::newRow("3<=3") << "t->t<=3" << 3.0 << true;
	QTest::newRow("test param eq") << "t->(4*t-6*t+1)=(cos(0)-30-t+power(2,2)*t)" << 6.0 << true;
	QTest::newRow("cos(0)>0") << "t->cos(t)>0" << 0.0 << true;
	QTest::newRow("cos(0)>=0") << "t->cos(t)>=0" << 0.0 << true;
	QTest::newRow("cos(0)<0") << "t->cos(t)<0" << 0.0 << false;
}

void AnalitzaJitTest::testCalculateUnaryBooleanLambda()
{
	QFETCH(QString, expression);
	QFETCH(double, arg1value);
	QFETCH(bool, expected);
	
	arg1->setValue(arg1value);
	
	bool result = 0;
	
	QVERIFY(a->setExpression(Analitza::Expression(expression)));
	QVERIFY(a->calculateLambda(result));
	QCOMPARE(result, expected);
}

void AnalitzaJitTest::testCalculateUnaryRealLambda_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<double>("arg1value");
	QTest::addColumn<double>("expected");
	
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
	QTest::newRow("root(9,2)") << "t->root(9,t)" << 2.0 << 3.0;
	QTest::newRow("root(9,0.5)") << "t->root(9,t)" << 0.5 << 81.0;
	QTest::newRow("piecewise 1") << "x->piecewise { x=4? 5 }" << 4.0 << 5.0;
	QTest::newRow("piecewise 2") << "x->piecewise { x<=4? -5, x>4? 5}" << -4.0 << -5.0;
	QTest::newRow("piecewise 3(1)") << "x->piecewise { x<0? -42, x=0? 0.42, x>0? 42}" << -1.0 << -42.0;
	QTest::newRow("piecewise 3(2)") << "x->piecewise { x<0? -42, x=0? 0.42, x>0? 42}" << 1.0 << 42.0;
	QTest::newRow("piecewise 3(3)") << "x->piecewise { x<0? -42, x=0? 0.42, x>0? 42}" << 0.0 << 0.42;
	QTest::newRow("piecewise otherwise 1") << "x->piecewise { x<4? -5, ? 5 }" << -99.0 << -5.0;
	QTest::newRow("piecewise otherwise 2") << "x->piecewise { x<4? -5, ? 5 }" << 99.0 << 5.0;
	QTest::newRow("basic piecewise otherwise 1") << "x->piecewise { 10=11? x, 11=11? -x }" << 42.0 << -42.0;
	QTest::newRow("basic piecewise otherwise 2") << "x->piecewise { x<0 ? x**2, ? x }" << -0.6 << 0.36;
	QTest::newRow("piecewise otherwise 1") << "x->piecewise { x<1? 99, x>1? 22, ? 96 }" << 7.0 << 22.0;
	QTest::newRow("piecewise otherwise 2") << "x->piecewise { x<1? 99, x>1? 22, ? 96 }" << 1.0 << 96.0;
	QTest::newRow("piecewise otherwise 3") << "x->piecewise { x<1? 99, x>1? 22, ? 96 }" << -97.0 << 99.0;
	QTest::newRow("long piecewise otherwise 1") << "x->piecewise { x=1? x, x=2? x*x, x=3? 3*x, x=4? 4, x=9? 81, ? 42 }" << 1.0 << 1.0;
	QTest::newRow("long piecewise otherwise 2") << "x->piecewise { x=1? x, x=2? x*x, x=3? 3*x, x=4? 44, x=9? 81, ? 42 }" << 2.0 << 4.0;
	QTest::newRow("long piecewise otherwise 3") << "x->piecewise { x=1? x, x=2? x*x, x=3? 2*x, x=4? 44, x=9? 81, ? 42 }" << 3.0 << 6.0;
	QTest::newRow("long piecewise otherwise 4") << "x->piecewise { x=1? x, x=2? x*x, x=3? 2*x, x=4? 44, x=9? 81, ? 42 }" << 9.0 << 81.0;
	QTest::newRow("long piecewise otherwise 4") << "x->piecewise { x=1? x, x=2? x*x, x=3? 2*x, x=4? 44, x=9? 81, ? 42 }" << 10.0 << 42.0;
}

void AnalitzaJitTest::testCalculateUnaryRealLambda()
{
	QFETCH(QString, expression);
	QFETCH(double, arg1value);
	QFETCH(double, expected);
	
	arg1->setValue(arg1value);
	
	double result = 0;
	
	QVERIFY(a->setExpression(Analitza::Expression(expression)));
	QVERIFY(a->calculateLambda(result));
	
	bool eq = epscompare(result, expected);
	
	if (!eq) {
		qDebug() << "Actual: " << result;
		qDebug() << "Expected: " << expected;
	}
	QVERIFY(eq);
}

void AnalitzaJitTest::testCalculateBinaryRealLambda_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<double>("arg1value");
	QTest::addColumn<double>("arg2value");
	QTest::addColumn<double>("expected");
	
	QTest::newRow("x^2+y^2") << "(x,y)->x*x+y*y" << 3.0 << 7.5 << 65.25;
	QTest::newRow("pow(x,y)") << "(x,y)->power(x,y)" << 6.0 << 2.0 << 36.0;
	QTest::newRow("min(x,y) 1") << "(x,y)->min(x,y)" << 6.0 << 2.0 << 2.0;
	QTest::newRow("min(x,y) 2") << "(x,y)->min(x,y)" << 2.0 << 6.0 << 2.0;
	QTest::newRow("min(x,y) 3") << "(x,y)->min(x,y)" << 6.0 << 6.0 << 6.0;
	QTest::newRow("min(x,y) -1") << "(x,y)->min(x,y)" << -6.0 << 2.0 << -6.0;
	QTest::newRow("min(x,y) -2") << "(x,y)->min(x,y)" << -2.0 << 6.0 << -2.0;
	QTest::newRow("min(x,y) -3") << "(x,y)->min(x,y)" << -6.0 << -6.0 << -6.0;
	QTest::newRow("max(x,y) 1") << "(x,y)->max(x,y)" << 6.0 << 2.0 << 6.0;
	QTest::newRow("max(x,y) 2") << "(x,y)->max(x,y)" << 2.0 << 6.0 << 6.0;
	QTest::newRow("max(x,y) 3") << "(x,y)->max(x,y)" << 6.0 << 6.0 << 6.0;
	QTest::newRow("max(x,y) -1") << "(x,y)->max(x,y)" << -6.0 << 2.0 << 2.0;
	QTest::newRow("max(x,y) -2") << "(x,y)->max(x,y)" << -2.0 << 6.0 << 6.0;
	QTest::newRow("max(x,y) -3") << "(x,y)->max(x,y)" << -6.0 << -6.0 << -6.0;
	//a->setLambdaExpression(Expression("(x,y)->x+y")/*, argtipos*/);
// 	a->setLambdaExpression(Analitza::Expression("(x,y,z)->x=3.0000")); // true i.e 1
	
	//a->setLambdaExpression(Analitza::Expression("x->piecewise { x<0 ? -x, x=0 ? -5, x>5 ? x*x, ? x }"));
}

void AnalitzaJitTest::testCalculateBinaryRealLambda()
{
	QFETCH(QString, expression);
	QFETCH(double, arg1value);
	QFETCH(double, arg2value);
	QFETCH(double, expected);
	
	arg1->setValue(arg1value);
	arg2->setValue(arg2value);
	
	double result = 0;
	
	QVERIFY(a->setExpression(Analitza::Expression(expression)));
	QVERIFY(a->calculateLambda(result));
	
	bool eq = epscompare(result, expected);
	
	if (!eq) {
		qDebug() << "Actual: " << result;
		qDebug() << "Expected: " << expected;
	}
	QVERIFY(eq);
}

void AnalitzaJitTest::testCalculateUnaryRealVectorLambda_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<double>("arg1value");
	QTest::addColumn< QVector<double> >("expected");
	
	QTest::newRow("simple param") << "t->vector{t*t, 7*t}" << 5.0 << (QVector<double>() << 25.0 << 35.0);
	QTest::newRow("3D param curv") << "t->vector{cos(t), sin(t), t}" << 0.0 << (QVector<double>() << 1.0 << 0.0 << 0.0);
}

void AnalitzaJitTest::testCalculateUnaryRealVectorLambda()
{
	QFETCH(QString, expression);
	QFETCH(double, arg1value);
	QFETCH( QVector<double> , expected );
	
	arg1->setValue(arg1value);
	
	QVector<double> result;
	
	QVERIFY(a->setExpression(Analitza::Expression(expression)));
	
	QVERIFY(a->calculateLambda(result));
	
	bool eq = epscompare(result, expected);
	
	if (!eq) {
		qDebug() << "Actual: " << result;
		qDebug() << "Expected: " << expected;
	}
	QVERIFY(eq);
}

void AnalitzaJitTest::testCalculateBinaryRealVectorLambda_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<double>("arg1value");
	QTest::addColumn<double>("arg2value");
	QTest::addColumn< QVector<double> >("expected");
	
	QTest::newRow("two params") << "(u,v)->vector{u+v, u-v, u*v}" << 7.0 << 3.0 << (QVector<double>() << 10.0 << 4.0 << 21.0);
}

void AnalitzaJitTest::testCalculateBinaryRealVectorLambda()
{
	QFETCH(QString, expression);
	QFETCH(double, arg1value);
	QFETCH(double, arg2value);
	QFETCH( QVector<double> , expected );
	
	arg1->setValue(arg1value);
	arg2->setValue(arg2value);
	
	QVector<double> result;
	
	QVERIFY(a->setExpression(Analitza::Expression(expression)));
	
	QVERIFY(a->calculateLambda(result));
	
	bool eq = epscompare(result, expected);
	
	if (!eq) {
		qDebug() << "Actual: " << result;
		qDebug() << "Expected: " << expected;
	}
	QVERIFY(eq);
}

#include "analitzajittest.moc"
