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

#include "commandstest.h"

#include <QTest>

#include "analyzer.h"
#include <config-analitza.h>

using Analitza::Expression;

QTEST_MAIN( CommandsTest )

CommandsTest::CommandsTest(QObject *parent)
 : QObject(parent)
{}

CommandsTest::~CommandsTest()
{}

void CommandsTest::initTestCase()
{
    a=new Analitza::Analyzer;
}

void CommandsTest::cleanupTestCase()
{
    delete a;
}

void CommandsTest::testCorrect_data()
{
    QTest::addColumn<QStringList>("expression");
    QTest::addColumn<QString>("result");
    
    QStringList script;
    
    script.clear();
    script << QStringLiteral("range(10)");
    QTest::newRow("simple range") << script << "list { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }";
    
    script.clear();
    script << QStringLiteral("range(-1.5, 2)");
    QTest::newRow("range(-1.5, 2)") << script << "list { -1.5, -0.5, 0.5, 1.5 }";
    
    script.clear();
    script << QStringLiteral("range(0, 1, 0.2)");
    QTest::newRow("range(0, 1, 0.2)") << script << "list { 0, 0.2, 0.4, 0.6, 0.8, 1 }";
    
    script.clear();
    script << QStringLiteral("vector(3, -2.3)");
    QTest::newRow("simple fill vector") << script << "vector { -2.3, -2.3, -2.3 }";
    
    script.clear();
    script << QStringLiteral("7*vector(34, 14.2)[23]");
    QTest::newRow("select fill vector") << script << "99.4";
    
    script.clear();
    script << QStringLiteral("seq := range(14, 20, 2)");
    script << QStringLiteral("vector(seq)");
    QTest::newRow("vector by range/seq") << script << "vector { 14, 16, 18, 20 }";
    
    script.clear();
    script << QStringLiteral("matrix(3, 2, -15.7)");
    QTest::newRow("simple fill matrix") << script << "matrix { matrixrow { -15.7, -15.7 }, matrixrow { -15.7, -15.7 }, matrixrow { -15.7, -15.7 } }";
    
    script.clear();
    script << QStringLiteral("matrix(2)");
    QTest::newRow("simple fill square matrix") << script << "matrix { matrixrow { 0, 0 }, matrixrow { 0, 0 } }";
    
    script.clear();
    script << QStringLiteral("matrix(3, 2)");
    QTest::newRow("simple matrix") << script << "matrix { matrixrow { 0, 0 }, matrixrow { 0, 0 }, matrixrow { 0, 0 } }";
    
    script.clear();
    script << QStringLiteral("matrix(3, 2, -9.8)[2][1]");
    QTest::newRow("select simple matrix") << script << "-9.8";
    
    script.clear();
    script << QStringLiteral("matrix(vector{2,3}, vector{7, 4})");
    QTest::newRow("matrix by vectors/columns") << script << "matrix { matrixrow { 2, 7 }, matrixrow { 3, 4 } }";
    
    script.clear();
    script << QStringLiteral("matrix(matrixrow{2,3}, matrixrow{7, 4})");
    QTest::newRow("matrix by rows") << script << "matrix { matrixrow { 2, 3 }, matrixrow { 7, 4 } }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{13,6}, matrixrow{-2,5}}");
    script << QStringLiteral("matrix(vector{2,3}, vector{7, 4}) + A");
    QTest::newRow("matrix by vectors/columns + A") << script << "matrix { matrixrow { 15, 13 }, matrixrow { 1, 9 } }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{2, 3, 6}, matrixrow{-5, 0, 2.3}}");
    script << QStringLiteral("matrix(2, 3, -9) + A");
    QTest::newRow("fill + A") << script << "matrix { matrixrow { -7, -6, -3 }, matrixrow { -14, -9, -6.7 } }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{2, 3}, matrixrow{-5, 1}}");
    script << QStringLiteral("B := matrix{matrixrow{12, 13}, matrixrow{-15, 11}}");
    script << QStringLiteral("blockmatrix(matrixrow{A, B})");
    QTest::newRow("simple block matrix") << script << "matrix { matrixrow { 2, 3, 12, 13 }, matrixrow { -5, 1, -15, 11 } }";
    
    const QString resultblockmatrix = QStringLiteral("matrix { matrixrow { 1, 8, 7, 6 }, matrixrow { 3, 5, 0, 2 }, matrixrow { 1, 4, 9, 3 } }");
    
    script.clear();
    script << QStringLiteral("A := matrix(vector{1,3}, vector{8,5})");
    script << QStringLiteral("B := matrix{matrixrow{7,6}, matrixrow{0,2}}");
    script << QStringLiteral("C := matrix(matrixrow{1,4})");
    script << QStringLiteral("D := matrix(vector{9}, vector{3})");
    script << QStringLiteral("blockmatrix(matrixrow{A, B}, matrixrow{C, D})");
    QTest::newRow("block matrix 4 blocks conf 1") << script << resultblockmatrix;
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{1,8,7}, matrixrow{3,5,0}}");
    script << QStringLiteral("B := matrix(vector{6,2})");
    script << QStringLiteral("C := matrix(matrixrow{1,4,9})");
    script << QStringLiteral("D := matrix(1,1,3)");
    script << QStringLiteral("blockmatrix(matrixrow{A, B}, matrixrow{C, D})");
    QTest::newRow("block matrix by rows, conf 2") << script << resultblockmatrix;
    
    script.clear();
    script << QStringLiteral("A := matrix(matrixrow{1,8})");
    script << QStringLiteral("B := transpose(matrix(diag(diag(7,6))))");
    script << QStringLiteral("C := matrix{matrixrow{3,5}}");
    script << QStringLiteral("D := matrix(matrixrow{0,2})");
    script << QStringLiteral("E := matrix(vector{1},vector{4})");
    script << QStringLiteral("F := matrix{matrixrow{9,3}}");
    script << QStringLiteral("blockmatrix(matrixrow{A, B}, matrixrow{C, D}, matrixrow{E, F})");
    QTest::newRow("block matrix by rows, conf 3") << script << resultblockmatrix;
    
    script.clear();
    script << QStringLiteral("A := matrix(vector{1,3}, vector{8,5})");
    script << QStringLiteral("B := matrix(matrixrow{1,4})");
    script << QStringLiteral("C := matrix{matrixrow{7,6}, matrixrow{0,2}}");
    script << QStringLiteral("D := matrix(vector{9}, vector{3})");
    script << QStringLiteral("blockmatrix(vector{A, B}, vector{C, D})");
    QTest::newRow("block matrix by cols, conf 1") << script << resultblockmatrix;
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{1,8}}");
    script << QStringLiteral("B := matrix(matrixrow{3,5})");
    script << QStringLiteral("C := matrix(matrixrow{1,4})");
    script << QStringLiteral("D := matrix{matrixrow{7,6}}");
    script << QStringLiteral("E := matrix(matrixrow{0,2})");
    script << QStringLiteral("F := matrix(matrixrow{9,3})");
    script << QStringLiteral("blockmatrix(vector{A, B, C}, vector{D, E, F})");
    QTest::newRow("block matrix by cols, conf 2") << script << resultblockmatrix;
    
    script.clear();
    script << QStringLiteral("matrix(2,5)");
    QTest::newRow("simple 0") << script << "matrix { matrixrow { 0, 0, 0, 0, 0 }, matrixrow { 0, 0, 0, 0, 0 } }";
    
    script.clear();
    script << QStringLiteral("identitymatrix(3)");
    QTest::newRow("simple I") << script << "matrix { matrixrow { 1, 0, 0 }, matrixrow { 0, 1, 0 }, matrixrow { 0, 0, 1 } }";
    
    script.clear();
    script << QStringLiteral("identitymatrix(3)-identitymatrix(3)");
    QTest::newRow("I - I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
    
    script.clear();
    script << QStringLiteral("0*identitymatrix(3)");
    QTest::newRow("0*I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
    
    script.clear();
    script << QStringLiteral("matrix(3,3) + identitymatrix(3)");
    QTest::newRow("0 + I") << script << "matrix { matrixrow { 1, 0, 0 }, matrixrow { 0, 1, 0 }, matrixrow { 0, 0, 1 } }";
    
    script.clear();
    script << QStringLiteral("diag(5, 3.2, 6, -9)");
    QTest::newRow("simple diag") << script << "matrix { matrixrow { 5, 0, 0, 0 }, matrixrow { 0, 3.2, 0, 0 }, matrixrow { 0, 0, 6, 0 }, matrixrow { 0, 0, 0, -9 } }";
    
    script.clear();
    script << QStringLiteral("diag(vector{5, 3.2, 6, -9})");
    QTest::newRow("simple diag by vector") << script << "matrix { matrixrow { 5, 0, 0, 0 }, matrixrow { 0, 3.2, 0, 0 }, matrixrow { 0, 0, 6, 0 }, matrixrow { 0, 0, 0, -9 } }";
    
    script.clear();
    script << QStringLiteral("diag(1, 1, 1) - identitymatrix(3)");
    QTest::newRow("zeromatrix: diag - I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
    
    script.clear();
    script << QStringLiteral("diag(vector{1, 1, 1}) - identitymatrix(3)");
    QTest::newRow("zeromatrix: diag by vector - I") << script << "matrix { matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 }, matrixrow { 0, 0, 0 } }";
    
    script.clear();
    script << QStringLiteral("diag(5, 7.8, -0.6, 3.5)[3][3] + diag(5, 7.8, -0.6, 3.5)[2][3]");
    QTest::newRow("selector diag") << script << "-0.6";
    
    script.clear();
    script << QStringLiteral("v := vector{5, 7.8, -0.6, 3.5}");
    script << QStringLiteral("diag(v)[3][3] + diag(v)[2][3]");
    QTest::newRow("selector diag by vector") << script << "-0.6";
    
    script.clear();
    script << QStringLiteral("blockdiag(matrix{matrixrow{1, 3}, matrixrow{-6, 8}}, matrix{matrixrow{5, 6}, matrixrow{14, -1.2}})");
    QTest::newRow("simple block diagonal") << script << "matrix { matrixrow { 1, 3, 0, 0 }, matrixrow { -6, 8, 0, 0 }, matrixrow { 0, 0, 5, 6 }, matrixrow { 0, 0, 14, -1.2 } }";
    
    script.clear();
    script << QStringLiteral("I := identitymatrix(3)");
    script << QStringLiteral("A := matrix(2,3, -6)");
    script << QStringLiteral("B := 3*I");
    script << QStringLiteral("blockdiag(I, A, B)");
    QTest::newRow("block diagonal") << script << "matrix { matrixrow { 1, 0, 0, 0, 0, 0, 0, 0, 0 }, matrixrow { 0, 1, 0, 0, 0, 0, 0, 0, 0 }, matrixrow { 0, 0, 1, 0, 0, 0, 0, 0, 0 }, matrixrow { 0, 0, 0, -6, -6, -6, 0, 0, 0 }, matrixrow { 0, 0, 0, -6, -6, -6, 0, 0, 0 }, matrixrow { 0, 0, 0, 0, 0, 0, 3, 0, 0 }, matrixrow { 0, 0, 0, 0, 0, 0, 0, 3, 0 }, matrixrow { 0, 0, 0, 0, 0, 0, 0, 0, 3 } }";
    
    script.clear();
    script << QStringLiteral("tridiag(-2.1, 3.6, 48, 5)");
    QTest::newRow("simple tridiag") << script << "matrix { matrixrow { 3.6, 48, 0, 0, 0 }, matrixrow { -2.1, 3.6, 48, 0, 0 }, matrixrow { 0, -2.1, 3.6, 48, 0 }, matrixrow { 0, 0, -2.1, 3.6, 48 }, matrixrow { 0, 0, 0, -2.1, 3.6 } }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -1.2, cos(pi)}, matrixrow{-45, 9.6, -2.3}}");
    script << QStringLiteral("tridiag(-8, 10, 7.2, 3) - identitymatrix(3) + A");
    QTest::newRow("tridiag - I + A") << script << "matrix { matrixrow { 1.2, 9.5, 5 }, matrixrow { -8, 7.8, 6.2 }, matrixrow { -45, 1.6, 6.7 } }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -12, 1}, matrixrow{-45, 9.6, cos(pi)}}");
    script << QStringLiteral("diag(A)");
    QTest::newRow("simple diag(A)") << script << "vector { -7.8, -12, -1 }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{8, 2.3, 5}, matrixrow{-45, -cos(pi), 12}}");
    script << QStringLiteral("diag(A)");
    QTest::newRow("getdiag fat") << script << "vector { 8, 1 }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{8, 2.3, 1}, matrixrow{3, 32, 2}, matrixrow{-45, 12, 3}, matrixrow{1, 0, 3}, matrixrow{-5, 1, 0}}");
    script << QStringLiteral("diag(A)");
    QTest::newRow("getdiag skinny") << script << "vector { 8, 32, 3 }";
    
    script.clear();
    script << QStringLiteral("A := matrix{matrixrow{-7.8, 2.3, 5}, matrixrow{0, -12, 1}, matrixrow{-45, 9.6, cos(pi)}}");
    script << QStringLiteral("diag(A)[2]");
    QTest::newRow("selector getdiag") << script << "-12";
    
    script.clear();
    script << QStringLiteral("v := vector{5,5,0}");
    script << QStringLiteral("A := matrix{matrixrow{0, -1, 2}, matrixrow{4, 0, -3.2}, matrixrow{5.8, -15, 0}}");
    script << QStringLiteral("B := matrix(3,3, 4.5)");
    script << QStringLiteral("D := diag(v)");
    script << QStringLiteral("I := identitymatrix(3)");
    script << QStringLiteral("O := matrix(3,3)");
    script << QStringLiteral("T := tridiag(2,1,8,3)");
    script << QStringLiteral("A + B + D - cos(pi)*I + O + T");
    QTest::newRow("complex exp") << script << "matrix { matrixrow { 11.5, 11.5, 6.5 }, matrixrow { 10.5, 11.5, 9.3 }, matrixrow { 10.3, -8.5, 6.5 } }";
    
// 3.6     48      0     9    80
// 2.1    3.2     49    20   100
//   1   -2.1    3.6    47    90
//   0     -7   -2.1   3.3    42
//   5      4      3  -2.1   3.5
    const QString square = QStringLiteral("A := matrix{matrixrow{3.6, 48, 0, 9, 80}, matrixrow {2.1, 3.2, 49, 20, 100}, matrixrow{1, -2.1, 3.6, 47, 90}, matrixrow{0, -7, -2.1, 3.3, 42}, matrixrow{5, 4,  3, -2.1, 3.5}}");
    
    script.clear();
    script << square;
    script << QStringLiteral("diag(A,2)");
    QTest::newRow("getndiag square +2") << script << "vector { 0, 20, 90 }";
    
    script.clear();
    script << square;
    script << QStringLiteral("diag(A,-2)");
    QTest::newRow("getndiag square -2") << script << "vector { 1, -7, 3 }";
    
    script.clear();
    script << square;
    script << QStringLiteral("diag(A,-3)");
    QTest::newRow("getndiag square -3") << script << "vector { 0, 4 }";
    
    script.clear();
    script << square;
    script << QStringLiteral("diag(A,3)");
    QTest::newRow("getndiag square +3") << script << "vector { 9, 100 }";
    
    script.clear();
    script << square;
    script << QStringLiteral("diag(A,-3)");
    QTest::newRow("getndiag square -3") << script << "vector { 0, 4 }";
    
    script.clear();
    script << square;
    script << QStringLiteral("diag(A,4)[1]");
    QTest::newRow("selector getndiag square corner +") << script << "80";

    script.clear();
    script << square;
    script << QStringLiteral("diag(A,-4)[1]");
    QTest::newRow("selector getndiag square corner -") << script << "5";
    
//  3.6    42     0      9     80   4   15
// -4.1   7.8    45      0    100   6    7 
//    1   2.2   3.6     47      0   8    9
//    0     0  -2.3    5.6     48   2    1
//    2     4     3   -2.1    3.6   3    5
    const QString fat = QStringLiteral("A := matrix{matrixrow{3.6, 42, 0, 9, 80, 4, 15}, matrixrow{-4.1, 7.8, 45, 0, 100, 6,7}, matrixrow{1, 2.2, 5.6, 47, 0 , 8, 9}, matrixrow{0, 0, -2.3, 3.6, 48, 2,1}, matrixrow{2, 4, 3, -2.1, 3.6, 3,5}}");
    
    script.clear();
    script << fat;
    script << QStringLiteral("diag(A,5)");
    QTest::newRow("getndiag fat +5") << script << "vector { 4, 7 }";
    
    script.clear();
    script << fat;
    script << QStringLiteral("diag(A,1)");
    QTest::newRow("getndiag fat +1") << script << "vector { 42, 45, 47, 48, 3 }";
    
    script.clear();
    script << fat;
    script << QStringLiteral("diag(A,-1)");
    QTest::newRow("getndiag fat -1") << script << "vector { -4.1, 2.2, -2.3, -2.1 }";
    
    script.clear();
    script << fat;
    script << QStringLiteral("diag(A,6)[1]");
    QTest::newRow("selector getndiag fat corner +") << script << "15";

    script.clear();
    script << fat;
    script << QStringLiteral("diag(A,-4)[1]");
    QTest::newRow("selector getndiag fat corner -") << script << "2";
    
//  3.6    42
// -4.1   7.8
//    1   2.2
//    0     1
//    2     4
    const QString skinny = QStringLiteral("A := matrix{matrixrow{3.6, 42}, matrixrow{-4.1, 7.8}, matrixrow{1, 2.2}, matrixrow{0, 1}, matrixrow{2, 4}}");
    
    script.clear();
    script << fat;
    script << QStringLiteral("diag(A,1)[1]");
    QTest::newRow("selector getndiag skinny corner +") << script << "42";
    
    script.clear();
    script << skinny;
    script << QStringLiteral("diag(A,-1)");
    QTest::newRow("getndiag skinny -1") << script << "vector { -4.1, 2.2 }";
    
    script.clear();
    script << skinny;
    script << QStringLiteral("diag(A,-3)");
    QTest::newRow("getndiag skinny -3") << script << "vector { 0, 4 }";
    
    script.clear();
    script << skinny;
    script << QStringLiteral("diag(A,-4)");
    QTest::newRow("getndiag skinny -4") << script << "vector { 2 }";
    
    script.clear();
    script << skinny;
    script << QStringLiteral("isdiag(A)");
    QTest::newRow("is not diag") << script << "false";
    
    script.clear();
    script << QStringLiteral("iszeromatrix(matrix(8,5))");
    QTest::newRow("is zero matrix") << script << "true";
    
    script.clear();
    script << QStringLiteral("isidentitymatrix(identitymatrix(5))");
    QTest::newRow("Id is identity matrix") << script << "true";
    
    script.clear();
    script << QStringLiteral("isidentitymatrix(blockdiag(identitymatrix(3), matrix{matrixrow{1}}, identitymatrix(2)))");
    QTest::newRow("block of Id is Id matrix") << script << "true";
    
    script.clear();
    script << QStringLiteral("isdiag(identitymatrix(4))");
    QTest::newRow("Id is diag matrix") << script << "true";
    
#ifdef HAVE_EIGEN3
    script.clear();
    script << QStringLiteral("eigenvalues(identitymatrix(4))");
    QTest::newRow("eigenvalues: from Id") << script << "list { 1, 1, 1, 1 }";
    
    script.clear();
    script << QStringLiteral("eigenvalues(matrix(vector{3,4}, vector{-2, -1}))");
    QTest::newRow("eigenvalues: full complex") << script << "list { 1+2*i, 1-2*i }";
    
    script.clear();
    script << QStringLiteral("eigenvectors(identitymatrix(4))");
    QTest::newRow("eigenvectors: from Id") << script << "list { vector { 1, 0, 0, 0 }, vector { 0, 1, 0, 0 }, vector { 0, 0, 1, 0 }, vector { 0, 0, 0, 1 } }";
    
    script.clear();
    script << QStringLiteral("eigenvectors(matrix(vector{3,4}, vector{-2, -1}))");
    QTest::newRow("complex eigenvectors") << script << "list { vector { -0.408248290464+0.408248290464*i, 0.816496580928*i }, vector { -0.408248290464-0.408248290464*i, -0.816496580928*i } }";
#endif
}

void CommandsTest::testCorrect()
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

    QString script = expression.join(QStringLiteral("\n"));
    script+=QLatin1String("\n\n\n");
    QTextStream stream(&script);
    a->importScript(&stream);
    QVERIFY(a->isCorrect());
}

void CommandsTest::testIncorrect_data()
{
    QTest::addColumn<QString>("expression");
    
    QTest::newRow("range: bad input") << "range(list{3,4}-9)";
    QTest::newRow("range: bad arg type") << "range(list{3}, 31, true)";
    QTest::newRow("range: bad arg type2") << "range(list{3}, 31)";
    QTest::newRow("range: bad arg count") << "range(2,3,vector{3}, list{2}, 4)";
    QTest::newRow("range: bad args1") << "range(2,3,9, list{2}, 4)";
    QTest::newRow("range: bad args2") << "range(2,3,9, 9, vector{4})";
    QTest::newRow("range: bad args3") << "range(2,3,9, 9, 3,4,5,5)";
    
    QTest::newRow("vector: bad arg type") << "vector(list{23},4)";
    QTest::newRow("vector: bad number of args") << "vector(4, list{23}, 44)";
    
    QTest::newRow("matrix: 0 args") << "matrix()";
    QTest::newRow("matrix: bad args") << "matrix(list{3}, 31)";
    QTest::newRow("matrix: invalid parameter") << "matrix(range(list{matrix{2}}))";
    QTest::newRow("matrix: empty matrix result") << "matrix(0, 0)";
    QTest::newRow("matrix: fill empty matrix result") << "matrix(0, 0, sin(1))";
    QTest::newRow("matrix: not all vectors") << "matrix(vector{1}, 3)";
    QTest::newRow("matrix: not all matrixrow elements") << "matrix(matrixrow{1}, list{2})";
    QTest::newRow("matrix: not all matrixrow elements 2") << "matrix(matrixrow{1}, vector{2})";
    QTest::newRow("matrix: neg square") << "matrix(-9)";
    
    QTest::newRow("zero matrix: empty matrix result") << "matrix(0, 0)";
    QTest::newRow("zero matrix: bad number of args") << "matrix()";
    QTest::newRow("zero matrix: bad number of args") << "matrix(3,list{3},4,6)";
    QTest::newRow("zero matrix: bad dim") << "matrix(23, -3.5)";
    QTest::newRow("zero matrix: bad dim2") << "matrix(-23, 5)";
    
    QTest::newRow("identity matrix: bad arg") << "identitymatrix(-98)";
    QTest::newRow("identity matrix: empty matrix result") << "identitymatrix(0)";
    
    QTest::newRow("diag: 0 args") << "diag()";
    QTest::newRow("diag: bad arg, one empty matrix") << "diag(identitymatrix(0))";
    QTest::newRow("diag: bad arg, one empty matrix2") << "diag(matrix{matrixrow{}})";
    QTest::newRow("diag: bad diag index") << "diag(matrix(4,6,3.2), -98)";
    QTest::newRow("diag: bad diag index type") << "diag(matrix(4,6,3.2), list{-98})";
    QTest::newRow("diag: invalid parameter") << "diag(matrix(-8,5))";
    
    QTest::newRow("tridiag: empty matrix result") << "tridiag(1,2,3,0)";
    QTest::newRow("tridiag: bad number of args") << "tridiag(1,2,2)";
    QTest::newRow("tridiag: bad number of args2") << "tridiag(1,2,2, 4, list{2})";
    
    QTest::newRow("iszeromatrix: bad number of args") << "iszeromatrix()";
    QTest::newRow("iszeromatrix: empty matrix") << "iszeromatrix(matrix{})";
    QTest::newRow("iszeromatrix: invalid parameter") << "iszeromatrix(matrix(-8,5))";
    
    QTest::newRow("isidentitymatrix: bad number of args") << "isidentitymatrix(matrix{matrixrow{1}}, 2)";
    QTest::newRow("isidentitymatrix: bad number of args2") << "isidentitymatrix()";
    QTest::newRow("isidentitymatrix: empty matrix") << "isidentitymatrix(matrix{})";
    QTest::newRow("isidentitymatrix: empty matrix2") << "isidentitymatrix(matrix{matrixrow{}})";
    QTest::newRow("isidentitymatrix: invalid parameter") << "isidentitymatrix(matrix(-8,5))";
    QTest::newRow("isidentitymatrix: invalid parameter2") << "isidentitymatrix(matrix(list{},5))";
    
    QTest::newRow("isdiag: bad number of args") << "isdiag(matrix{matrixrow{1}}, 2)";
    QTest::newRow("isdiag: bad number of args2") << "isdiag()";
    QTest::newRow("isdiag: empty matrix") << "isdiag(matrix{})";
    QTest::newRow("isdiag: empty matrix2") << "isdiag(matrix{matrixrow{}})";
    QTest::newRow("isdiag: invalid parameter") << "isdiag(matrix(-8,5))";
    
    QTest::newRow("blockmatrix: bad block matrix size") << "blockmatrix(vector{matrix(1,2), matrix(32,13)})";
    QTest::newRow("blockmatrix: empty matrix1") << "blockmatrix(vector{matrix{}}, vector{matrix{matrixrow{}}})";
    QTest::newRow("blockmatrix: empty matrix2") << "blockmatrix(vector{identitymatrix(0)})";
    QTest::newRow("blockmatrix: bad arg type") << "blockmatrix(vector{-5})";
    QTest::newRow("blockmatrix: err arg") << "blockmatrix(vector{matrix{-98}})";
    QTest::newRow("blockmatrix: bad block matrix args size") << "blockmatrix(vector{matrix(1,2), matrix(32,13)}, vector{matrix(7,13)})";
    QTest::newRow("blockmatrix: bad block matrix args type 1") << "blockmatrix(vector{matrix(32,13), list{23}}, vector{matrix(7,13), matrix(32,1)})";
    QTest::newRow("blockmatrix: bad block matrix args type 2") << "matrix(vector{list{23}, zeromatrix(32,13)}, vector{zeromatrix(7,13), zeromatrix(32,1)})";
    
    QTest::newRow("blockdiag: bad block diag, empty matrix 1") << "blockdiag(matrix(0,0), matrix(2,2,1))";
    QTest::newRow("blockdiag: empty matrix1") << "blockdiag(matrix{})";
    QTest::newRow("blockdiag: empty matrix2") << "blockdiag(identitymatrix(0))";
    QTest::newRow("blockdiag: bad arg type") << "blockdiag(vector{-5})";
    QTest::newRow("blockdiag: bad block diag, empty matrix 2") << "blockdiag(matrix{matrixrow{1}}, tridiag(1,2,3,0))";
    QTest::newRow("blockdiag: invalid parameter") << "blockdiag(matrix(-8,5))";
    
    QTest::newRow("bad dimensions:2x2identitymatrix and 2x1zeromatrix") << "2*(identitymatrix(2) + matrix(2,1))";
    QTest::newRow("bad dimensions:2x2identitymatrix and -2x2matrix") << "2*(identitymatrix(2) + matrix(-2, 2,1))";

#ifdef HAVE_EIGEN3
    QTest::newRow("eigenvalues: bad args type1") << "eigenvalues(list{23},4)";
    QTest::newRow("eigenvalues: bad args type2") << "eigenvalues(list{23})";
    QTest::newRow("eigenvalues: empty matrix2") << "eigenvalues(matrix{})";
    QTest::newRow("eigenvalues: empty invalid matrix2") << "eigenvalues(matrix{1}, matrix{})";
    QTest::newRow("eigenvalues: bad numbe of args") << "eigenvalues(identitymatrix(3), zeromatrix(4,5))";
    QTest::newRow("eigenvalues: empty matrix") << "eigenvalues(matrix{matrixrow{1,2}}, matrix{})";
    QTest::newRow("eigenvalues: invalid arg types1") << "eigenvalues(matrix{matrixrow{1,2}}, matrix{matrixrow{list{5},6}})";
    QTest::newRow("eigenvalues: invalid arg types2") << "eigenvalues(matrix{matrixrow{1,list{2}}})";
    QTest::newRow("eigenvalues: non square matrix") << "eigenvalues(matrix{matrixrow{1,2,3}})";
    //TODO nan and inf cases
#endif
}

void CommandsTest::testIncorrect()
{
    QFETCH(QString, expression);
    
    Expression exp(expression, false);
    
    if (exp.isCorrect()) {
        Analitza::Analyzer a;
        a.setExpression(exp);
        
        if (a.isCorrect()) {
            Expression calc = a.calculate();
            QVERIFY(!a.isCorrect());
            QCOMPARE(calc.toString(), QString());
            qDebug() << "calc errors:" << a.errors();
            
            Expression eval = a.evaluate();
            QVERIFY(!a.isCorrect());
            QCOMPARE(eval.toString(), QString());
            qDebug() << "eval errors:" << a.errors();
        } else {
            QVERIFY(!a.isCorrect());
            qDebug() << "set expression errors:" << a.errors();
        }
    } else {
        qDebug() << "expression errors:" << exp.error();
    }
}


