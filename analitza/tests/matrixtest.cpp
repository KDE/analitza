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
#include <matrix.h>
#include <container.h>
#include <apply.h>

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

void MatrixTest::testCorrect_data()
{
	QTest::addColumn<QStringList>("expression");
	QTest::addColumn<QString>("result");
	
	QStringList script;
	//TODO test diferent sizes like 2*(identitymatrix(2) + zeromatrix(2,1))
	//TODO test neg ndexes and extra args ... eg 2*(identitymatrix(2) + testcmd(-2, 2,1))
	//TODO error cases too (matrix and diag)
	
	script.clear();
	script << "vector(3, -2.3)";
	QTest::newRow("simple fill vector") << script << "vector { -2.3, -2.3, -2.3 }";
	
	script.clear();
	script << "7*vector(34, 14.2)[23]";
	QTest::newRow("select fill vector") << script << "99.4";
	
	script.clear();
	script << "matrix(3, 2, -15.7)";
	QTest::newRow("simple fill matrix") << script << "matrix { matrixrow { -15.7, -15.7 }, matrixrow { -15.7, -15.7 }, matrixrow { -15.7, -15.7 } }";
	
	script.clear();
	script << "matrix(2)";
	QTest::newRow("simple fill square matrix") << script << "matrix { matrixrow { 0, 0 }, matrixrow { 0, 0 } }";
	
	script.clear();
	script << "matrix(3, 2)";
	QTest::newRow("simple matrix") << script << "matrix { matrixrow { 0, 0 }, matrixrow { 0, 0 }, matrixrow { 0, 0 } }";
	
	script.clear();
	script << "matrix(3, 2, -9.8)[2][1]";
	QTest::newRow("select simple matrix") << script << "-9.8";
	
	script.clear();
	script << "matrix(vector{2,3}, vector{7, 4})";
	QTest::newRow("matrix by vectors/columns") << script << "matrix { matrixrow { 2, 7 }, matrixrow { 3, 4 } }";
	
	script.clear();
	script << "matrix(matrixrow{2,3}, matrixrow{7, 4})";
	QTest::newRow("matrix by rows") << script << "matrix { matrixrow { 2, 3 }, matrixrow { 7, 4 } }";
	
	script.clear();
	script << "A := matrix{matrixrow{13,6}, matrixrow{-2,5}}";
	script << "matrix(vector{2,3}, vector{7, 4}) + A";
	QTest::newRow("matrix by vectors/columns + A") << script << "matrix { matrixrow { 15, 13 }, matrixrow { 1, 9 } }";
	
	script.clear();
	script << "A := matrix{matrixrow{2, 3, 6}, matrixrow{-5, 0, 2.3}}";
	script << "matrix(2, 3, -9) + A";
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
	script << "diag(5, 3.2, 6, -9)";
	QTest::newRow("simple diag") << script << "matrix { matrixrow { 5, 0, 0, 0 }, matrixrow { 0, 3.2, 0, 0 }, matrixrow { 0, 0, 6, 0 }, matrixrow { 0, 0, 0, -9 } }";
	
	script.clear();
	script << "diag(vector{5, 3.2, 6, -9})";
	QTest::newRow("simple diag by vector") << script << "matrix { matrixrow { 5, 0, 0, 0 }, matrixrow { 0, 3.2, 0, 0 }, matrixrow { 0, 0, 6, 0 }, matrixrow { 0, 0, 0, -9 } }";
	
	script.clear();
	script << "diag(1, 1, 1) - identitymatrix(3)";
	QTest::newRow("zeromatrix: diag - I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
	
	script.clear();
	script << "diag(vector{1, 1, 1}) - identitymatrix(3)";
	QTest::newRow("zeromatrix: diag by vector - I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
	
	script.clear();
	script << "diag(5, 7.8, -0.6, 3.5)[3][3] + diag(5, 7.8, -0.6, 3.5)[2][3]";
	QTest::newRow("selector diag") << script << "-0.6";
	
	script.clear();
	script << "v := vector{5, 7.8, -0.6, 3.5}";
	script << "diag(v)[3][3] + diag(v)[2][3]";
	QTest::newRow("selector diag by vector") << script << "-0.6";
	
	script.clear();
	script << "diag(matrix{matrixrow{1, 3}, matrixrow{-6, 8}}, matrix{matrixrow{5, 6}, matrixrow{14, -1.2}})";
	QTest::newRow("simple block diagonal") << script << "matrix { matrixrow { 1, 3, 0, 0 }, matrixrow { -6, 8, 0, 0 }, matrixrow { 0, 0, 5, 6 }, matrixrow { 0, 0, 14, -1.2 } }";
	
	script.clear();
	script << "I := identitymatrix(3)";
	script << "A := matrix(2,3, -6)";
	script << "B := 3*I";
	script << "diag(I, A, B)";
	QTest::newRow("block diagonal") << script << "matrix { matrixrow { 1, 0, 0, 0, 0, 0, 0, 0, 0 }, matrixrow { 0, 1, 0, 0, 0, 0, 0, 0, 0 }, matrixrow { 0, 0, 1, 0, 0, 0, 0, 0, 0 }, matrixrow { 0, 0, 0, -6, -6, -6, 0, 0, 0 }, matrixrow { 0, 0, 0, -6, -6, -6, 0, 0, 0 }, matrixrow { 0, 0, 0, 0, 0, 0, 3, 0, 0 }, matrixrow { 0, 0, 0, 0, 0, 0, 0, 3, 0 }, matrixrow { 0, 0, 0, 0, 0, 0, 0, 0, 3 } }";
	
	script.clear();
	script << "tridiag(-2.1, 3.6, 48, 5)";
	QTest::newRow("simple tridiag") << script << "matrix { matrixrow { 3.6, 48, 0, 0, 0 }, matrixrow { -2.1, 3.6, 48, 0, 0 }, matrixrow { 0, -2.1, 3.6, 48, 0 }, matrixrow { 0, 0, -2.1, 3.6, 48 }, matrixrow { 0, 0, 0, -2.1, 3.6 } }";
	
	script.clear();
	script << "A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -1.2, cos(pi)}, matrixrow{-45, 9.6, -2.3}}";
	script << "tridiag(-8, 10, 7.2, 3) - identitymatrix(3) + A";
	QTest::newRow("tridiag - I + A") << script << "matrix { matrixrow { 1.2, 9.5, 5 }, matrixrow { -8, 7.8, 6.2 }, matrixrow { -45, 1.6, 6.7 } }";
	
	script.clear();
	script << "A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -12, 1}, matrixrow{-45, 9.6, cos(pi)}}";
	script << "diag(A)";
	QTest::newRow("simple diag(A)") << script << "vector { -7.8, -12, -1 }";
	
	script.clear();
	script << "A := matrix{matrixrow{8, 2.3, 5}, matrixrow{-45, -cos(pi), 12}}";
	script << "diag(A)";
	QTest::newRow("getdiag fat") << script << "vector { 8, 1 }";
	
	script.clear();
	script << "A := matrix{matrixrow{8, 2.3, 1}, matrixrow{3, 32, 2}, matrixrow{-45, 12, 3}, matrixrow{1, 0, 3}, matrixrow{-5, 1, 0}}";
	script << "diag(A)";
	QTest::newRow("getdiag skinny") << script << "vector { 8, 32, 3 }";
	
	script.clear();
	script << "A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -12, 1}, matrixrow{-45, 9.6, cos(pi)}}";
	script << "diag(A)[2]";
	QTest::newRow("selector getdiag") << script << "-12";
	
	script.clear();
	script << "v := vector{5,5,0}";
	script << "A := matrix{matrixrow{0, -1, 2}, matrixrow{4, 0, -3.2}, matrixrow{5.8, -15, 0}}";
	script << "B := matrix(3,3, 4.5)";
	script << "D := diag(v)";
	script << "I := identitymatrix(3)";
	script << "O := zeromatrix(3,3)";
	script << "T := tridiag(2,1,8,3)";
	script << "A + B + D - cos(pi)*I + O + T";
	QTest::newRow("complex exp") << script << "matrix { matrixrow { 11.5, 11.5, 6.5 }, matrixrow { 10.5, 11.5, 9.3 }, matrixrow { 10.3, -8.5, 6.5 } }";
	
// 3.6     48      0     9    80
// 2.1    3.2     49    20   100
//   1   -2.1    3.6    47    90
//   0     -7   -2.1   3.3    42
//   5      4      3  -2.1   3.5
	QString square = "A := matrix{matrixrow{3.6, 48, 0, 9, 80}, matrixrow {2.1, 3.2, 49, 20, 100}, matrixrow{1, -2.1, 3.6, 47, 90}, matrixrow{0, -7, -2.1, 3.3, 42}, matrixrow{5, 4,  3, -2.1, 3.5}}";
	
	script.clear();
	script << square;
	script << "diag(A,2)";
	QTest::newRow("getndiag square +2") << script << "vector { 0, 20, 90 }";
	
	script.clear();
	script << square;
	script << "diag(A,-2)";
	QTest::newRow("getndiag square -2") << script << "vector { 1, -7, 3 }";
	
	script.clear();
	script << square;
	script << "diag(A,-3)";
	QTest::newRow("getndiag square -3") << script << "vector { 0, 4 }";
	
	script.clear();
	script << square;
	script << "diag(A,3)";
	QTest::newRow("getndiag square +3") << script << "vector { 9, 100 }";
	
	script.clear();
	script << square;
	script << "diag(A,-3)";
	QTest::newRow("getndiag square -3") << script << "vector { 0, 4 }";
	
	script.clear();
	script << square;
	script << "diag(A,4)[1]";
	QTest::newRow("selector getndiag square corner +") << script << "80";

	script.clear();
	script << square;
	script << "diag(A,-4)[1]";
	QTest::newRow("selector getndiag square corner -") << script << "5";
	
//  3.6    42     0      9     80   4   15
// -4.1   7.8    45      0    100   6    7 
//    1   2.2   3.6     47      0   8    9
//    0     0  -2.3    5.6     48   2    1
//    2     4     3   -2.1    3.6   3    5
	QString fat = "A := matrix{matrixrow{3.6, 42, 0, 9, 80, 4, 15}, matrixrow{-4.1, 7.8, 45, 0, 100, 6,7}, matrixrow{1, 2.2, 5.6, 47, 0 , 8, 9}, matrixrow{0, 0, -2.3, 3.6, 48, 2,1}, matrixrow{2, 4, 3, -2.1, 3.6, 3,5}}";
	
	script.clear();
	script << fat;
	script << "diag(A,5)";
	QTest::newRow("getndiag fat +5") << script << "vector { 4, 7 }";
	
	script.clear();
	script << fat;
	script << "diag(A,1)";
	QTest::newRow("getndiag fat +1") << script << "vector { 42, 45, 47, 48, 3 }";
	
	script.clear();
	script << fat;
	script << "diag(A,-1)";
	QTest::newRow("getndiag fat -1") << script << "vector { -4.1, 2.2, -2.3, -2.1 }";
	
	script.clear();
	script << fat;
	script << "diag(A,6)[1]";
	QTest::newRow("selector getndiag fat corner +") << script << "15";

	script.clear();
	script << fat;
	script << "diag(A,-4)[1]";
	QTest::newRow("selector getndiag fat corner -") << script << "2";
	
//  3.6    42
// -4.1   7.8
//    1   2.2
//    0     1
//    2     4
	QString skinny = "A := matrix{matrixrow{3.6, 42}, matrixrow{-4.1, 7.8}, matrixrow{1, 2.2}, matrixrow{0, 1}, matrixrow{2, 4}}";
	
	script.clear();
	script << fat;
	script << "diag(A,1)[1]";
	QTest::newRow("selector getndiag skinny corner +") << script << "42";
	
	script.clear();
	script << skinny;
	script << "diag(A,-1)";
	QTest::newRow("getndiag skinny -1") << script << "vector { -4.1, 2.2 }";
	
	script.clear();
	script << skinny;
	script << "diag(A,-3)";
	QTest::newRow("getndiag skinny -3") << script << "vector { 0, 4 }";
	
	script.clear();
	script << skinny;
	script << "diag(A,-4)";
	QTest::newRow("getndiag skinny -4") << script << "vector { 2 }";
	
	script.clear();
	script << skinny;
	script << "isdiag(A)";
	QTest::newRow("is not diag") << script << "false";
	
	script.clear();
	script << "iszeromatrix(zeromatrix(8,5))";
	QTest::newRow("is zero matrix") << script << "true";
	
	script.clear();
	script << "isidentitymatrix(identitymatrix(5))";
	QTest::newRow("Id is identity matrix") << script << "true";
	
	script.clear();
	script << "isidentitymatrix(diag(identitymatrix(3), identitymatrix(2)))";
	QTest::newRow("block of Id is Id matrix") << script << "true";
	
	script.clear();
	script << "isdiag(identitymatrix(4))";
	QTest::newRow("Id is diag matrix") << script << "true";
}

void MatrixTest::testCorrect()
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

void MatrixTest::testIncorrect_data()
{
	QTest::addColumn<QString>("expression");
	
	QTest::newRow("matrix 0 args") << "matrix()";
	QTest::newRow("matrix: empty matrix result") << "matrix(0, 0)";
	QTest::newRow("matrix: fill empty matrix result") << "matrix(0, 0, sin(1))";
	QTest::newRow("matrix: not all vectors") << "matrix(vector{1}, 3)";
	QTest::newRow("matrix: not all matrixrow elements") << "matrix(matrixrow{1}, list{2})";
	QTest::newRow("matrix: neg square") << "matrix(-9)";
	QTest::newRow("zero matrix: empty matrix result") << "zeromatrix(0, 0)";
	QTest::newRow("zero matrix: bad number of args") << "zeromatrix()";
	QTest::newRow("zero matrix: bad dim") << "zeromatrix(23, -3.5)";
	QTest::newRow("identity matrix: matrix result") << "identitymatrix(0)";
	QTest::newRow("diag: 0 args") << "diag()";
	QTest::newRow("tridiag: empty matrix result") << "tridiag(1,2,3,0)";
	QTest::newRow("tridiag: bad number of args") << "tridiag(1,2,2)";
	QTest::newRow("iszeromatrix: bad number of args") << "iszeromatrix()";
	QTest::newRow("isdiag: bad number of args") << "isdiag(matrix{matrixrow{1}}, 2)";
	QTest::newRow("isdiag: bad number of args2") << "isdiag()";
}

void MatrixTest::testIncorrect()
{
	QFETCH(QString, expression);
	
	Analitza::Analyzer a;
	Expression exp(expression, false);
	QVERIFY(exp.isCorrect());
	a.setExpression(exp);
	Expression calc = a.calculate();
	QVERIFY(!a.isCorrect());
	QCOMPARE(calc.toString(), QString());
	Expression eval = a.evaluate();
	QVERIFY(!a.isCorrect());
	QCOMPARE(eval.toString(), QString());
	qDebug() << "errors:" << a.errors();
}

#include "matrixtest.moc"
