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

#include "analyzer.h"

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
	QTest::addColumn<QStringList>("expression");
	QTest::addColumn<QString>("result");
	
	QStringList script;
	/*
	script.clear();
	script << "fillmatrix(3, 2, -15.7)";
	QTest::newRow("simple fill") << script << "matrix { matrixrow { -15.7, -15.7 }, matrixrow { -15.7, -15.7 }, matrixrow { -15.7, -15.7 } }";
	
	script.clear();
	script << "A := matrix{matrixrow{2, 3, 6}, matrixrow{-5, 0, 2.3}}";
	script << "fillmatrix(2, 3, -9) + A";
	QTest::newRow("fill + A") << script << "matrix { matrixrow { -7, -6, -3 }, matrixrow { -14, -9, -6.7 } }";
	
	script.clear();
	script << "zeromatrix(2,5)";
	QTest::newRow("simple 0") << script << "matrix { matrixrow { 0, 0, 0, 0, 0 }, matrixrow { 0, 0, 0, 0, 0 } }";
	
	script.clear();
	script << "identitymatrix(3)";
	QTest::newRow("simple I") << script << "matrix { matrixrow { 1, 0, 0 }, matrixrow { 0, 1, 0 }, matrixrow { 0, 0, 1 } }";
	
	script.clear();
	script << "identitymatrix(3)-identitymatrix(3)";
	QTest::newRow("I - I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
	
	script.clear();
	script << "0*identitymatrix(3)";
	QTest::newRow("0*I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
	
	script.clear();
	script << "zeromatrix(3,3) + identitymatrix(3)";
	QTest::newRow("0 + I") << script << "matrix { matrixrow { 1, 0, 0 }, matrixrow { 0, 1, 0 }, matrixrow { 0, 0, 1 } }";
	
	script.clear();
	script << "diag(vector{5, 3.2, 6, -9})";
	QTest::newRow("simple diag") << script << "matrix { matrixrow { 5, 0, 0, 0 }, matrixrow { 0, 3.2, 0, 0 }, matrixrow { 0, 0, 6, 0 }, matrixrow { 0, 0, 0, -9 } }";
	
	script.clear();
	script << "diag(vector{1, 1, 1}) - identitymatrix(3)";
	QTest::newRow("zeromatrix: diag - I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
	
	script.clear();
	script << "v := vector{5, 7.8, -0.6, 3.5}";
	script << "diag(v)[3][3] + diag(v)[2][3]";
	QTest::newRow("selector diag") << script << "-0.6";
	
	script.clear();
	script << "tridiag(-2.1, 3.6, 48, 5)";
	QTest::newRow("simple tridiag") << script << "matrix { matrixrow { 3.6, 48, 0, 0, 0 }, matrixrow { -2.1, 3.6, 48, 0, 0 }, matrixrow { 0, -2.1, 3.6, 48, 0 }, matrixrow { 0, 0, -2.1, 3.6, 48 }, matrixrow { 0, 0, 0, -2.1, 3.6 } }";
	
	script.clear();
	script << "A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -1.2, cos(pi)}, matrixrow{-45, 9.6, -2.3}}";
	script << "tridiag(-8, 10, 7.2, 3) - identitymatrix(3) + A";
	QTest::newRow("tridiag - I + A") << script << "matrix { matrixrow { 1.2, 9.5, 5 }, matrixrow { -8, 7.8, 6.2 }, matrixrow { -45, 1.6, 6.7 } }";
	
	script.clear();
	script << "A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -12, 1}, matrixrow{-45, 9.6, cos(pi)}}";
	script << "getdiag(A)";
	QTest::newRow("simple getdiag(A)") << script << "vector { -7.8, -12, -1 }";
	
	script.clear();
	script << "A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -12, 1}, matrixrow{-45, 9.6, cos(pi)}}";
	script << "getdiag(A)[2]";
	QTest::newRow("selector getdiag") << script << "-12";
	
	script.clear();
	script << "v := vector{5,5,0}";
	script << "A := matrix{matrixrow{0, -1, 2}, matrixrow{4, 0, -3.2}, matrixrow{5.8, -15, 0}}";
	script << "B := fillmatrix(3,3, 4.5)";
	script << "D := diag(v)";
	script << "I := identitymatrix(3)";
	script << "O := zeromatrix(3,3)";
	script << "T := tridiag(2,1,8,3)";
	script << "A + B + D - cos(pi)*I + O + T";
	QTest::newRow("complex exp") << script << "matrix { matrixrow { 11.5, 11.5, 6.5 }, matrixrow { 10.5, 11.5, 9.3 }, matrixrow { 10.3, -8.5, 6.5 } }";
	*/
	script.clear();
/*
  matrix{  \n
	matrixrow { 	3.6,	48, 	 0, 	9, 		80 } \n
	matrixrow { -	2.1,	3.6,	 48,	 0, 	100 }  \n
	matrixrow { 	1,	    -2.1,	 3.6,	 48, 	0 }  \n
	matrixrow { 	0,	 	0, 		-2.1,	 3.6, 	48 } \n
	matrixrow { 	5,	 	4, 		  3, 	-2.1, 	3.6 } }
	
	script << "A := matrix{matrixrow {3.6,48,  0,9,80 }, matrixrow { -	2.1,	3.6,	 48,	 0, 	100 },matrixrow { 	1, -2.1,	 3.6,	 48, 	0 },matrixrow { 	0,	 	0, 		-2.1,	 3.6, 	48 },matrixrow { 	5,	 	4, 		  3, 	-2.1, 	3.6 } }";


  matrix{  \n
	matrixrow { 	3.6,	42, 	 0, 	9, 		80,		4,		5 } \n
	matrixrow {    -4.1,	3.6,	 45,	 0, 	100,	6,		7 }  \n
	matrixrow { 	1,	    -2.1,	 3.6,	 47, 	0 ,		8,		9}  \n
	matrixrow { 	0,	 	0, 		-2.3,	 5.6, 	48 ,	2,		1} \n
	matrixrow { 	5,	 	4, 		  3, 	-2.1, 	3.6 ,	3,		5} }

	*/
 	script << "A := matrix{matrixrow {3.6,42,  0,9,80 ,4,5}, matrixrow { -4.1,	3.6,	 45,	 0, 	100, 6,7 },matrixrow { 	1, -2.1,	 5.6,	 47, 	0 ,8,9},matrixrow { 	0,	 	0, 		-2.3,	 3.6, 	48,2,1 },matrixrow { 	5,	 	4, 		  3, 	-2.1, 	3.6,3,5 } }";
//  	script << "A := matrix{matrixrow{2,3}}";
// script << "A:=identitymatrix(4)";
// 	script << "getndiag(A,1)";
// 	script << "matrix{matrixrow{23,3},matrixrow{32,sin(5)}}";
	QTest::newRow("getndiag") << script << "-12";
}

void MatrixTest::testBuiltinMethods()
{
	QFETCH(QStringList, expression);
	QFETCH(QString, result);
	
	Expression last;
	Analitza::Analyzer b1;
	foreach(const QString &exp, expression) {
		Expression e(exp, false);
		if(!e.isCorrect()) qDebug() << "error:" << e.error();
		QVERIFY(e.isCorrect());
		
		b1.setExpression(e);
		
		if(!b1.isCorrect()) qDebug() << "errors: " << b1.errors();
		QVERIFY(b1.isCorrect());
		last = b1.calculate();
		if(!b1.isCorrect()) qDebug() << "errors:" << e.toString() << b1.errors();
		QVERIFY(b1.isCorrect());
	}
	QCOMPARE(last.toString(), result);

	QString script = expression.join("\n");
	script+="\n\n\n";
	QTextStream stream(&script);
	a->importScript(&stream);
	QVERIFY(a->isCorrect());
}

#include "matrixtest.moc"
