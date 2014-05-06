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

#include "matrixtest.h"

#include <QtTest/QTest>
#include <QDebug>

#include "analyzer.h"
//#include <operations.h>

using Analitza::Expression;

QTEST_MAIN( MatrixTest )

MatrixTest::MatrixTest(QObject *parent)
 : QObject(parent)
{}

MatrixTest::~MatrixTest()
{}

void MatrixTest::initTestCase()
{
	a=new Analitza::Analyzer;
}

void MatrixTest::cleanupTestCase()
{
	delete a;
}

void MatrixTest::testBuiltinMethods_data()
{
	//add matrix strings in analitza language
// 	QTest::addColumn<QString>("expression");
// 	QTest::addColumn<double>("result");
// 
// 	QTest::newRow("a value") << "2" << 2.;
// 	QTest::newRow("val.e0") << "12.0e-02" << 12e-2;
// 	QTest::newRow("vale") << "12e-2" << 12e-2;
// 	QTest::newRow("val") << "12e2" << 12e2;
// 	
// 	QTest::newRow("simple addition") << "2+2" << 4.;
// 	QTest::newRow("simple power") << "2**99" << pow(2., 99.);
// 	QTest::newRow("simple multiplication") << "3*3" << 9.;
// 	QTest::newRow("sinus") << "sin(3*3)" << sin(9.);
// 	QTest::newRow("declare") << "x:=3" << 3.;
// 	QTest::newRow("sum") << "sum(x : x=1..99)" << 4950.;
// 	QTest::newRow("diff") << "(diff(x:x))(1)" << 1.;
// 	QTest::newRow("diffz") <<"(diff(z:z))(1)" << 1.;
// 	
// 	QTest::newRow("product") << "product(n : n=1..5)" << 120.;
// 	QTest::newRow("factorial") << "factorial(5)" << 120.;
// 	
// 	QTest::newRow("simple piecewise") << "piecewise { pi=0? 3, pi=pi?33 }" << 33.;
// 	QTest::newRow("simple piecewise with otherwise") << "piecewise { pi=0? 3, ?33 }" << 33.;
// 	QTest::newRow("boolean and") << "and(true,false)" << 0.;
// 	QTest::newRow("boolean or") << "or(false,true)" << 1.;
// 	QTest::newRow("boolean not") << "not(false)" << 1.;
// 	QTest::newRow("lambda")  << "(x->x+2)(2)" << 4.;
// 	QTest::newRow("lambda2") << "(x->3*x^2)(1)" << 3.;
// 	QTest::newRow("lambda3") << "(x->x*sum(t:t=0..3))(2)" << 12.;
// 	QTest::newRow("imaginarypow") << "(-4)^(1/4)" << 1.;
// 	QTest::newRow("imaginaryroot") << "root(-4, 4)" << 1.;
// 
// 	//comprehension
// 	QTest::newRow("sum.2bvars") << "sum(x*y : (x, y)=1..3)" << 36.;
// 	QTest::newRow("sum.list") << "sum(x : x@list{1,5,44})" << 50.;
// 
// 	QTest::newRow("sum.sum") << "sum(sum(x : x=0..i) : i=0..10)" << 220.;
// 
// 	QTest::newRow("exists") << "exists(x : x@list{true,true,false})" << 1.;
// 	QTest::newRow("forall") << "forall(x : x@list{true,true,false})" << 0.;
// // 	QTest::newRow("emptysum") << "sum(x : x@list{})" << 0.;
// 	
// 	QTest::newRow("lambdacall") << "f:=x->f(x)" << 0.;
}
using Analitza::ExpressionType;
void MatrixTest::testBuiltinMethods()
{
// 	a->setExpression(Expression("matrix( col(6,47,8,5), col(4, 7,6,7) )"));
// 	a->setExpression(Expression("matrix( row(6,47,8,5), row(4, 7,6,7) )"));
// 	a->setExpression(Expression("matrix(row(3,4), row(1,7), row(4,5))")); //GSOC
// 	a->setExpression(Expression("matrix(3,4)"));
// 	qDebug() << Expression("identitymatrix(3)").isCorrect();
// 	a->setExpression(Expression("identitymatrix(3)"));
// 	a->setExpression(Expression("diag(5,7,9,16)"));
// 	a->setExpression(Expression("diag(matrix{matrixrow{3,2,0}, matrixrow{1,4,7}, matrixrow{9,6,5}})"));
// 	a->setExpression(Expression("diag(matrix( row(6,47,8,5), row(4, 7,6,7) , row(4, 7,56,7) , row(4, 7,8,79) ))"));
// 	a->setExpression(Expression("tridiag(3, 85)"));
// 	Analitza::ExpressionType t1 = a->type();
// 	a->setExpression(Expression("matrix { matrixrow { 8, 9, 0, 0, 0 }, matrixrow { 3, 8, 9, 0, 0 }, matrixrow { 0, 3, 8, 9, 0 }, matrixrow { 0, 0, 3, 8, 9 }, matrixrow { 0, 0, 0, 3, 8 } }"));
// 	Analitza::ExpressionType t2 = a->type();
// 	a->setExpression(Expression("a[1][1]"));
	
// 	qDebug() << t1.toString();
// 	qDebug() << t2.toString();

// 	a->setExpression(Expression("matrix { matrixrow { 8, 9, 0, 0, 0 }, matrixrow { 3, 8, 9, 0, 0 }, matrixrow { 0, 3, 8, 9, 0 }, matrixrow { 0, 0, 3, 8, 9 }, matrixrow { 0, 0, 0, 3, 8 } }"));
// 	qDebug() << a->type().toString();
// 	Analitza::ExpressionType ty = ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), 5), 5);
// 	qDebug() << ty.toString();
// 	a->setExpression(Expression("tridiag(3, 8, 9, 5)"));
// 	qDebug() << a->calculate().toString();
// 	a->setExpression(Expression("matrix { matrixrow { 8, 9, 0, 0, 0 }, matrixrow { 3, 8, 9, 0, 0 }, matrixrow { 0, 3, 8, 9, 0 }, matrixrow { 0, 0, 3, 8, 9 }, matrixrow { 0, 0, 0, 3, 8 } }"));
// 	qDebug() << a->calculate().toString();
	//beg times
// 	Expression ee("2*matrix { matrixrow { 8, 9, 0, 0, 0 }, matrixrow { 3, 8, 9, 0, 0 }, matrixrow { 0, 3, 8, 9, 0 }, matrixrow { 0, 0, 3, 8, 9 }, matrixrow { 0, 0, 0, 3, 8 } }");
// 	
// 	a->setExpression(ee);
// 	qDebug() << a->isCorrect() << a->errors();
// 	qDebug() << a->type().toString() ;
// 	Expression cal = a->calculate();
// 	qDebug() << cal.toString(); /// listo el tipo es reconociado, ahora el problema es calculate
	//times
	
// 	qDebug() << a->errors();
	
// 	a->setExpression(Expression("tridiag(3, 8, 9, 5)+matrix { matrixrow { 8, 9, 0, 0, 0 }, matrixrow { 3, 8, 9, 0, 0 }, matrixrow { 0, 3, 8, 9, 0 }, matrixrow { 0, 0, 3, 8, 9 }, matrixrow { 0, 0, 0, 3, 8 } }"));
// 	qDebug() << a->type().toString() ;
// 	qDebug() << a->calculate().toString();
	

// 	a->setExpression(Expression("4*identitymatrix(3)+matrix { matrixrow { 8, 9, 0},matrixrow { 7, 5, 3},matrixrow { 1, 2, 10}}"));
// 	qDebug() << a->type().toString() << a->errors();
// 	Expression cal = a->calculate();
// 	qDebug() << cal.toString();
// 	qDebug() << a->errors();


	
// 	a->setExpression(Expression("matrix(col(1,3),col(5,7))"));
	a->setExpression(Expression("col(1,3)"));
// 	qDebug() << a->type().toString() << a->errors();
// 	Expression cal = a->calculate();
// 	qDebug() << cal.toString();
// 	qDebug() << a->errors();

	
	
	
// 	qDebug() << a->calculate().toString();
// 	qDebug() << a->errors();
	/// GSOC info
//A := matrix{matrixrow{3,2}, matrixrow{1,7}, matrixrow{4,5}} 
//A := matrix(row(3,4), row(1,7), row(4,5))
	
	//get hos matrix strinx invalid Analitza lang and test commands
// 	QFETCH(QString, expression);
// 	QFETCH(double, result);
// 	Expression e(expression, false);
// 	if(!e.isCorrect()) qDebug() << "error: " << e.error();
// 	QCOMPARE(e.isCorrect(), true);
// 	
// 	a->setExpression(e);
// 	
// 	if(!a->isCorrect()) qDebug() << "error: " << a->errors();
// 	QVERIFY(a->isCorrect());
// 	QCOMPARE(a->evaluate().toReal().value(), result);
// 	QVERIFY(a->isCorrect());
// 	Expression ee=a->calculate();
// 	if(!a->isCorrect()) qDebug() << "error: " << a->errors();
// 	QVERIFY(a->isCorrect());
// 	QCOMPARE(ee.toReal().value(), result);
// 	QVERIFY(a->isCorrect());
}

#include "matrixtest.moc"
