/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
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

#include "analitzatest.h"
#include "analyzer.h"
#include <cmath>

#include "apply.h"
#include "container.h"
#include "variables.h"
#include "vector.h"
#include "value.h"
#include <variable.h>
#include <analitzautils.h>
#include <QTest>
//#include <operations.h>

using namespace std;
using Analitza::Cn;
using Analitza::Ci;
using Analitza::Vector;
using Analitza::Object;
using Analitza::Operator;
using Analitza::Container;
using Analitza::Expression;

QTEST_MAIN( AnalitzaTest )

namespace QTest {

template <> char *toString(const Analitza::Cn &cn)
{ return qstrdup(QStringLiteral("Cn(%1)").arg(cn.toString()).toLatin1().constData()); }

}

Q_DECLARE_METATYPE(Cn)

AnalitzaTest::AnalitzaTest(QObject *parent)
 : QObject(parent)
{}

AnalitzaTest::~AnalitzaTest()
{}

void AnalitzaTest::initTestCase()
{
    a=new Analitza::Analyzer;
}

void AnalitzaTest::cleanupTestCase()
{
    delete a;
}

void AnalitzaTest::testTrivialCalculate_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<Cn>("result");

    QTest::newRow("a value") << "2" << Cn(2.);
    QTest::newRow("val.e0") << "12.0e-02" << Cn(12e-2);
    QTest::newRow("vale") << "12e-2" << Cn(12e-2);
    QTest::newRow("val") << "12e2" << Cn(12e2);

    QTest::newRow("factorial5") << "factorial(5)" << Cn(120);
    QTest::newRow("factorial12") << "factorial(12)" << Cn(12*11*10*9*8*7*6*5*4*3*2*1);
    
    QTest::newRow("simple addition") << "2+2" << Cn(4.);
    QTest::newRow("simple power") << "2**99" << Cn(pow(2., 99.));
    QTest::newRow("simple multiplication") << "3*3" << Cn(9.);
    QTest::newRow("sinus") << "sin(3*3)" << Cn(sin(9.));
    QTest::newRow("declare") << "x:=3" << Cn(3.);
    QTest::newRow("sum") << "sum(x : x=1..99)" << Cn(4950.);
    QTest::newRow("diff") << "(diff(x:x))(1)" << Cn(1.);
    QTest::newRow("diffz") <<"(diff(z:z))(1)" << Cn(1.);
    
    QTest::newRow("product") << "product(n : n=1..5)" << Cn(120.);
    QTest::newRow("factorial") << "factorial(5)" << Cn(120.);
    
    QTest::newRow("simple piecewise") << "piecewise { pi=0? 3, pi=pi?33 }" << Cn(33.);
    QTest::newRow("simple piecewise with otherwise") << "piecewise { pi=0? 3, ?33 }" << Cn(33.);
    QTest::newRow("boolean and") << "and(true,false)" << Cn(false);
    QTest::newRow("boolean or") << "or(false,true)" << Cn(true);
    QTest::newRow("boolean not") << "not(false)" << Cn(true);
    QTest::newRow("lambda")  << "(x->x+2)(2)" << Cn(4.);
    QTest::newRow("lambda2") << "(x->3*x^2)(1)" << Cn(3.);
    QTest::newRow("lambda3") << "(x->x*sum(t:t=0..3))(2)" << Cn(12.);
    QTest::newRow("imaginarypow") << "(-4)^(1/4)" << Cn(1, 1);
    QTest::newRow("imaginaryroot") << "root(-4, 4)" << Cn(1.);
    QTest::newRow("squareroot-1") << "(-1)^(1/2)" << Cn(0, 1);

    //comprehension
    QTest::newRow("sum.2bvars") << "sum(x*y : (x, y)=1..3)" << Cn(36.);
    QTest::newRow("sum.list") << "sum(x : x@list{1,5,44})" << Cn(50.);

    QTest::newRow("sum.sum") << "sum(sum(x : x=0..i) : i=0..10)" << Cn(220.);

    QTest::newRow("exists") << "exists(x : x@list{true,true,false})" << Cn(true);
    QTest::newRow("forall") << "forall(x : x@list{true,true,false})" << Cn(false);
//     QTest::newRow("emptysum") << "sum(x : x@list{})" << 0.;
    
    QTest::newRow("lambdacall") << "f:=x->f(x)" << Cn(0.);
    QTest::newRow("cpx1") << "i" << Cn(0, 1);
    QTest::newRow("cpx2") << "i*i" << Cn(-1);
    QTest::newRow("cpx3") << "2+i*i" << Cn(1);
    QTest::newRow("complex number") << "3+4*(5-6*i)" << Cn(23, -24);
}

void AnalitzaTest::testTrivialCalculate()
{
    QFETCH(QString, expression);
    QFETCH(Cn, result);
    Expression e(expression, false);
    if(!e.isCorrect()) qDebug() << "error: " << e.error();
    QCOMPARE(e.isCorrect(), true);
    
    a->setExpression(e);
    
    if(!a->isCorrect()) qDebug() << "error: " << a->errors();
    QVERIFY(a->isCorrect());
    QCOMPARE(a->evaluate().toReal(), result);
    QVERIFY(a->isCorrect());
    Expression ee=a->calculate();
    if(!a->isCorrect()) qDebug() << "error: " << a->errors();
    QVERIFY(a->isCorrect());
    QCOMPARE(ee.toReal(), result);
    QVERIFY(a->isCorrect());
}

void AnalitzaTest::testTrivialEvaluate_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("result");
    
    QTest::newRow("simple value") << "2" << "2";
    QTest::newRow("complex") << "i*5" << "5*i";
    QTest::newRow("simple complex value") << "6*(2+i)" << "12+6*i";
    QTest::newRow("complex irreductibility") << "i" << "i";
    QTest::newRow("from complex value") << "i*i" << "-1";
    QTest::newRow("from power complex") << "power(i, 2)" << "-1";
    QTest::newRow("sin complex") << "sin(i)" << "1.17520119364*i";
    QTest::newRow("cos complex") << "cos(5-9*i)" << "1149.26926545-3885.12187972*i";
    QTest::newRow("complex*complex") << "(5.3-9.8*i)*(-6.2+3.7*i)" << "3.4+80.37*i";
    QTest::newRow("simple complex/complex") << "i/i" << "1";
    QTest::newRow("complex/complex") << "(9.3-5.4*i)/(3.6-9.5*i)" << "0.82143203178+0.667667861641*i";
    QTest::newRow("simple complex conjugate") << "conjugate(i)" << "-i";
    QTest::newRow("complex conjugate") << "conjugate(-9.3+5.87*i)" << "-9.3-5.87*i";
    QTest::newRow("complex arg") << "arg(i)" << "1.57079632679";
    QTest::newRow("complex real part") << "real(45-9*i)" << "45";
    QTest::newRow("complex imag part") << "imaginary(45-9*i)" << "-9";
    QTest::newRow("simply complex mod") << "abs(i)" << "1";
    QTest::newRow("complex mod") << "abs(8-9*i)" << "12.0415945788";
    QTest::newRow("simple addition") << "2+2" << "4";
    QTest::newRow("simple addition with var") << "2+x" << "x+2";
    QTest::newRow("minus irreductibility") << "-x" << "-x";
    QTest::newRow("minus0") << "x-y" << "x-y";
    QTest::newRow("minus1") << "minus(x, y, x)" << "-y";
    QTest::newRow("minus2") << "x-y-y-y-x" << "-3*y";
    QTest::newRow("minus2.1") << "minus(x,y,y,y,x)" << "-3*y";
    QTest::newRow("minus3") << "x-x-x-x-x-x" << "-4*x";
    QTest::newRow("minus3.1") << "x-x-x-x" << "-2*x";
    QTest::newRow("minus3.2") << "minus(x,x,x,x,x,x)" << "-4*x";
    QTest::newRow("addition") << "x+x" << "2*x";
    QTest::newRow("simple polynomial") << "x+x+x**2+x**2" << "2*x+2*x^2";
    QTest::newRow("simplification of unary minus in times") << "x*(-x)" << "-x^2";
    QTest::newRow("strange") << "0*x-1*1" << "-1";
    QTest::newRow("strange2") << "x-x" << "0";
    QTest::newRow("old leak") << "x^1" << "x";
    QTest::newRow("declare") << "wockawocka:=3" << "3";
    QTest::newRow("nested multiplication") << "x*(x+x)" << "2*x^2";
    QTest::newRow("multiplication") << "x*x" << "x^2";
    QTest::newRow("undefined function call") << "f(2)" << "f(2)";
    QTest::newRow("--simplification") << "-(-x)" << "x";
    QTest::newRow("unneeded --simplification") << "-(x-x)" << "0";
    QTest::newRow("minus order") << "1-x" << "-x--1";
    QTest::newRow("minus order2") << "x-1" << "x-1";
    QTest::newRow("after simp(minus) --simplification") << "-(x-x-x)" << "x";
    QTest::newRow("and") << "and(6>5, 4<5)" << "true";
    QTest::newRow("or") << "or(6>5, 6<5)" << "true";
    
    QTest::newRow("sum") << "sum(n : n=1..99)" << "4950";
    QTest::newRow("sum times") << "x*sum(n : n=0..99)" << "4950*x";
    QTest::newRow("unrelated sum") << "sum(x : n=0..99)" << "100*x";
    
    QTest::newRow("product") << "product(n : n=1..5)" << "120";
    QTest::newRow("factorial") << "factorial(5)" << "120";
    
    QTest::newRow("simple piecewise") << "piecewise { eq(pi,0)? 3, eq(pi, pi)?33}" << "33";
    QTest::newRow("simple piecewise with otherwise") << "piecewise { eq(pi,0)? 3, ?33}" << "33";
    
    QTest::newRow("lambda") << "f:=q->2" << "q->2";
//     QTest::newRow("selector lambda") << "selector(2, vector{x->x, x->x+2})" << "x->x+2";
//     QTest::newRow("boolean and") << "and(x,0)" << "false";

    QTest::newRow("irreductible vector") << "vector { x, y, z }" << "vector { x, y, z }";
    QTest::newRow("in-vector operations") << "vector { x+x, y+y, z-z }" << "vector { 2*x, 2*y, 0 }";
    
    QTest::newRow("vect+vect") << "x+vector { 2, 3, 4 }+vector { 4, 3, 2 }" << "x+vector { 6, 6, 6 }";
    QTest::newRow("vect+2vect") << "2*vector { x, y, z }+vector{x,y,z}" << "3*vector { x, y, z }";
    QTest::newRow("vect+null") << "vector { x, y, z }+vector{0,0,0}" << "vector { x, y, z }";
    QTest::newRow("card") << "card(vector { x, y, z })" << "3";
    QTest::newRow("card+var") << "card(x)" << "card(x)";
    
    QTest::newRow("selector+idx") << "selector(1, vector{x,y,z})" << "x";
    QTest::newRow("selector+var") << "(vector { x, y, z })[x]" << "vector { x, y, z }[x]";
    QTest::newRow("selector+impossible") << "v[1]" << "v[1]";
    
    QTest::newRow("in lists") << "list{w+w}" << "list { 2*w }";
    QTest::newRow("lists") << "union(list{w}, list{x}, list{y,z})" << "list { w, x, y, z }";
    QTest::newRow("lists2") << "union(list{w}, x, list{y}, list{z})" << "union(list { w }, x, list { y, z })";
    
    QTest::newRow("sum.2bvars") << "sum(x*w : (x, y)=1..3)" << "18*w";
    QTest::newRow("sum.list") << "sum(x+y : x@list{x,y,z})" << "x+4*y+z";
    
    QTest::newRow("forall") << "forall(x : x@list{x,true,true})" << "forall(x:x@list { x, true, true })";
    QTest::newRow("exists") << "exists(x : x@list{x,false,false})" << "exists(x:x@list { x, false, false })";
    
    QTest::newRow("map") << "map(x->x**2, list {1,2,3})" << "list { 1, 4, 9 }";
    QTest::newRow("filter") << "filter(x->x>5, list {3,4,5,6,7})" << "list { 6, 7 }";
    
    QTest::newRow("forall1") << "forall(a<w:a@list { 2 })" << "2<w";
    
    QTest::newRow("matrix") << "matrix { matrixrow { 1, 2 } }" << "matrix { matrixrow { 1, 2 } }";
    QTest::newRow("matrix+") << "matrix { matrixrow { 1, 2 } }+matrix{ matrixrow { 1, 2 } }" << "matrix { matrixrow { 2, 4 } }";
    QTest::newRow("matrix++") << "matrix { matrixrow { 5, 6 }, matrixrow { 4, 0 }}+matrix { matrixrow { 2, 3 }, matrixrow { 4, 0 }}" << "matrix { matrixrow { 7, 9 }, matrixrow { 8, 0 } }";
    //TODO aucahuasi: we support only matrix and vector over a scalar field (numbers), but I think we could have matrix/vector over other structures too (e.g. functions, vectors, etc.)
    //QTest::newRow("matrix+++") << "matrix { matrixrow { vector { 1, 2 } } }+matrix { matrixrow { vector { 1.8, 2.4 } } }" << "matrix { matrixrow { vector { 2.8, 4.4 } } }";
    QTest::newRow("matrix@") << "selector(1, matrix { matrixrow { 1, 2 } })" << "vector { 1, 2 }";
    QTest::newRow("matrix@@") << "selector(1, selector(1, matrix { matrixrow { 1, 2 } }))" << "1";
    QTest::newRow("scalar multiplication of matrix") << "3*matrix { matrixrow { 5, 6 }, matrixrow { 4, 0 }}" << "matrix { matrixrow { 15, 18 }, matrixrow { 12, 0 } }";
    QTest::newRow("transpose vector") << "transpose(vector{12,45})" << "matrix { matrixrow { 12, 45 } }";
    QTest::newRow("row x column") << "matrix { matrixrow { 1, 2 } }*matrix { matrixrow { 3 }, matrixrow { 5 } }" << "matrix { matrixrow { 13 } }";
    QTest::newRow("column x row") << "matrix { matrixrow { 3 }, matrixrow { 5 } }*matrix { matrixrow { 1, 2 } }" << "matrix { matrixrow { 3, 6 }, matrixrow { 5, 10 } }";
    QTest::newRow("row x vector") << "matrix { matrixrow { 1, 2 } }*vector{ 3, 5 }" << "vector { 13 }";
    QTest::newRow("vector x row") << "vector{ 3, 5 }*matrix { matrixrow { 1, 2 } }" << "matrix { matrixrow { 3, 6 }, matrixrow { 5, 10 } }";
    QTest::newRow("vector x transpose(vector)") << "vector{ 3, 5 }*transpose(vector{1,2})" << "matrix { matrixrow { 3, 6 }, matrixrow { 5, 10 } }";
    QTest::newRow("matrix x vector") << "matrix { matrixrow { 3, 3 }, matrixrow { 2, 2 }, matrixrow { 3, 4 } }*vector{ 1, 2 }" << "vector { 9, 6, 11 }";
    QTest::newRow("matrix x matrix") << "matrix { matrixrow { 3, 3 }, matrixrow { 2, 2 }, matrixrow { 3, 4 } }*matrix { matrixrow{3, 3, 4, 5, 6}, matrixrow{2, 4, 5, 6, 2} }" << "matrix { matrixrow { 15, 21, 27, 33, 24 }, matrixrow { 10, 14, 18, 22, 16 }, matrixrow { 17, 25, 32, 39, 26 } }";
    QTest::newRow("matrix^0") << "power(matrix { matrixrow { 3, 3 }, matrixrow { 2, 2 } }, 0)" << "matrix { matrixrow { 1, 0 }, matrixrow { 0, 1 } }";
    QTest::newRow("matrix^1") << "power(matrix { matrixrow { 3, 3 }, matrixrow { 2, 2 } }, 1)" << "matrix { matrixrow { 3, 3 }, matrixrow { 2, 2 } }";
    QTest::newRow("matrix^2") << "power(matrix { matrixrow { 3, 3 }, matrixrow { 2, 2 } }, 2)" << "matrix { matrixrow { 15, 15 }, matrixrow { 10, 10 } }";
    QTest::newRow("matrix^64") << "power(matrix { matrixrow { 1.63, 2.4}, matrixrow { -0.36,7.128 } }, 64)" << "matrix { matrixrow { -2.79721542669e+52, 4.14615974023e+53 }, matrixrow { -6.21923961035e+52, 9.21843939558e+53 } }";
    QTest::newRow("matrix^6464") << "power(matrix { matrixrow { 1.0019 } }, 6464)"  << "matrix { matrixrow { 213191.74219 } }";
}

void AnalitzaTest::testTrivialEvaluate()
{
    QFETCH(QString, expression);
    QFETCH(QString, result);
    
    Expression e(expression, false);
    a->setExpression(e);
    if(!a->isCorrect())
        qDebug() << "errors:" << a->errors();
    
    qDeleteAll(*a->variables());
    a->variables()->clear();
    a->variables()->initializeConstants();
    
    QVERIFY(a->isCorrect());
    QCOMPARE(a->evaluate().toString(), result);
}

void AnalitzaTest::testDerivativeSimple_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("result");
    
    QTest::newRow("dumb") << "x" << "1";
    QTest::newRow("simple polynomial") << "x^3+1" << "3*x^2";
    QTest::newRow("power and sinus") << "x^2+sin(x)" << "2*x+cos(x)";
    QTest::newRow("power") << "x^2" << "2*x";
    QTest::newRow("division") << "1/x" << "-1/x^2";
    QTest::newRow("logarithm") << "ln x" << "1/x";
    QTest::newRow("times") << "x*y" << "y";
    QTest::newRow("powere") << "e^x" << "e^x"; // power derivative and logarithm simplification
    QTest::newRow("chain rule") << "sin(x**2)" << "2*x*cos(x^2)";
    QTest::newRow("tangent") << "tan(x**2)" << "(2*x)/cos(x^2)^2";
    QTest::newRow("piecewise") << "piecewise { x<0 ? x**2, ? x } " << "piecewise { x<0 ? 2*x, ? 1 }";
    QTest::newRow("lambda") << "x->3" << "0";
    QTest::newRow("timesminus") << "1-x*sin(x)" << "-sin(x)-x*cos(x)";
    QTest::newRow("timesminus2") << "cos(x)-x*sin(x)" << "-2*sin(x)-x*cos(x)";
    QTest::newRow("log") << "log(x)" << "1/(2.30258509299*x)";
    QTest::newRow("vector") << "vector { x, x^2 }" << "vector { 1, 2*x }";
    QTest::newRow("exp") << "exp(x**2)" << "2*x*exp(x^2)";
    QTest::newRow("halfx") << "(1/2)*x" << "0.5";
    QTest::newRow("halfx2") << "1/2 x" << "-2/(2*x)^2"; //TODO: could improve the simplification
}

void AnalitzaTest::testDerivativeSimple()
{
    QFETCH(QString, expression);
    QFETCH(QString, result);
    
    qDeleteAll(*a->variables());
    a->variables()->clear();
    a->variables()->initializeConstants();
    
    Expression e(expression, false);
    a->setExpression(e);
    QVERIFY(a->isCorrect());
    a->setExpression(a->derivative(QStringLiteral("x")));
    a->simplify();
    Expression deriv=a->expression();
    QCOMPARE(deriv.toString(), QString(QStringLiteral("x->")+result));
    if(!a->isCorrect()) qDebug() << "errors: " << a->errors();
    QVERIFY(a->isCorrect());
    
    double val=1.;
    QVector<Object*> vars;
    vars.append(new Cn(val));
    
    a->setExpression(Expression("x->"+expression, false));
    double valCalc=a->derivative(vars);
    qDeleteAll(vars);
    
    if(a->isCorrect()) {
        Expression ee(QStringLiteral("(x->%1)(%2)").arg(result).arg(val));
        a->setExpression(ee);
        QVERIFY(a->isCorrect());
        
        Expression r=a->calculate();
        
        if(a->isCorrect())
            QCOMPARE(QString::number(valCalc).left(5), QString::number(r.toReal().value()).left(5));
    }
    a->setExpression(Expression("diff("+expression+":x)", false));
    a->simplify();
    QVERIFY(a->isCorrect());
    deriv=a->evaluate();
    
    QCOMPARE(deriv.toString(), QString(QStringLiteral("x->")+result));
    QVERIFY(a->isCorrect());
}

void AnalitzaTest::testCorrection_data()
{
    QTest::addColumn<QStringList>("expression");
    QTest::addColumn<QString>("result");
    
    QStringList script;
    
    script.clear();
    script << QStringLiteral("f:=y->y*y");
    script << QStringLiteral("f(i)");
    QTest::newRow("from complex function") << script << "-1";
    
    script.clear();
    script << QStringLiteral("n:=2");
    script << QStringLiteral("n+1");
    QTest::newRow("simple") << script << "3";
    
    script.clear();
    script << QStringLiteral("f:=x->x+2");
    script << QStringLiteral("f(1)");
    QTest::newRow("simple func") << script << "3";
    
//     script.clear();
//     script << "t:=(c, c1, c2, t1, t2)->(t2-t1)/(c2-c1)*(c-c1)+t1";
//     script << "t(1,2,3,4,5)";
//     QTest::newRow("long func") << script << "3";
    
    script.clear();
    script << QStringLiteral("fact:=n->piecewise { n=1?1, ? n*fact(n-1) }");
    script << QStringLiteral("fact(5)");
    QTest::newRow("piecewise factorial") << script << "120";
    
    script.clear();
    script << QStringLiteral("fib:=n->piecewise { n=0?0, n=1?1, ?fib(n-1)+fib(n-2) }");
    script << QStringLiteral("fib(6)");
    QTest::newRow("piecewise fibonacci") << script << "8";
    
    script.clear();
    script << QStringLiteral("n:=vector{1}");
    script << QStringLiteral("func:=n->n+1");
    script << QStringLiteral("func(5)");
    QTest::newRow("simple function, shadowed parameter") << script << "6";
    
    script.clear();
    script << QStringLiteral("x:=3");
    script << QStringLiteral("x*sum(x : x=0..99)");
    QTest::newRow("bounded scope") << script << "14850";
    
    script.clear();
    script << QStringLiteral("f:=diff(x^2:x)");
    script << QStringLiteral("f(3)");
    QTest::newRow("diff function") << script << "6";
    
    script.clear();
    script << QStringLiteral("fv:=vector{x->x, x->x+2}");
    script << QStringLiteral("(selector(1, fv))(1)");
    script << QStringLiteral("(selector(1, fv))(1)+(selector(2, fv))(2)");
    QTest::newRow("selector+lambda") << script << "5";
    
    QTest::newRow("lists") << QStringList(QStringLiteral("union(list{0}, list{1}, list{2,3})")) << "list { 0, 1, 2, 3 }";
    
    script.clear();
    script <<    QStringLiteral("valueTableRec := (func, antimages, i) ->"
                "piecewise { i=0 ? list{}, "
                    "? union(list{func(selector(i, antimages))}, valueTableRec(func, antimages, i-1))"
                " }")
            << QStringLiteral("valueTableRec(x->x**2, list{1,2,3}, 3)");
    QTest::newRow("yay") << script << "list { 9, 4, 1 }";
    
    script.clear();
    script << QStringLiteral("f:=ff->(y->ff(y))");
//     script << "f(x->x**2)";
    script << QStringLiteral("(f(x->x**2))(2)");
    QTest::newRow("yay2") << script << "4";
    
    script.clear();
    script <<    QStringLiteral("findroot:=(der, dee)->piecewise { dee>1 ?"
                "piecewise { rem(der, dee)=0 ? true, ? findroot(der, dee-1)  }, ? false }");
    script << QStringLiteral("isprime:=n->not(findroot(n, floor(root(n, 2))))");
    script << QStringLiteral("primes:=(from, to)->piecewise { or(from<0, to<0, from>=to)? list{},"
                " isprime(from)? union(list{from}, primes(from+1, to)), ? primes(from+1, to)}");
    script << QStringLiteral("primes(1, 25)");
    QTest::newRow("primes") << script << "list { 1, 2, 3, 5, 7, 11, 13, 17, 19, 23 }";
    
    script.clear();
    script << QStringLiteral("f:=v->sum(i**2 : i@v)");
    script << QStringLiteral("f(list{1,2,3})");
    script << QStringLiteral("f(vector{1,2,3})");
    QTest::newRow("sum.list") << script << "14";
    
    script.clear();
    script << QStringLiteral("f:=o->vector { x->x+o, x->x*o }");
    script << QStringLiteral("vector { selector(1, f(3)), selector(1, f(4)) }");
    QTest::newRow("lambda") << script << "vector { x->x+3, x->x+4 }";
    
    script.clear();
    script << QStringLiteral("comb:=(n, i)->factorial(n)/(factorial(i)*factorial(n-i))")
           << QStringLiteral("p:=10^-2")
           << QStringLiteral("pu:=n->sum(  comb(n,i)*p^(n-i)*(1-p)*sum(x:x=0..i)  :i=0..(floor((n-1)/2)))")
           << QStringLiteral("pu(5)");
    
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0) && QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    QTest::newRow("bug241047") << script << "2.97495e-5";
#else
    QTest::newRow("bug241047") << script << "2.97495e-05";
#endif
    
    script.clear();
    script << QStringLiteral("comb:=(n, i)->factorial(n)/(factorial(i)*factorial(n-i))")
           << QStringLiteral("probability:=(place, case, totalprobability,"
                    "positive, negative)->(comb(place,"
                    "case)*(positive/totalprobability)^case)*(negative/totalprobability)^(place-case)")
           << QStringLiteral("sum(probability(5, t, 6, 1, 5):t=0..5)");
    
    QTest::newRow("probabilities") << script << "1";
    
    script.clear();
    script
            << QStringLiteral("rtail:=(elems,i)->piecewise { card(elems)>=i ? union(list{elems[i]}, rtail(elems, i+1)), ? list{} }")
            << QStringLiteral("tail:=elems->rtail(elems,2)")
            << QStringLiteral("foldr:=(f,z,elems)->piecewise {card(elems)=0 ? z, ? f(elems[1], foldr(f, z, tail(elems))) }")
            << QStringLiteral("sumsum:=elems->foldr((x,y)->x+y, 0, elems)")
            << QStringLiteral("sumsum(list{1,2,3})");
    QTest::newRow("sumsum") << script << "6";
    
    script.clear();
    script
            << QStringLiteral("rtail:=(elems,i)->piecewise { card(elems)>=i ? union(list{elems[i]}, rtail(elems, i+1)), ? list{} }")
            << QStringLiteral("tail:=elems->rtail(elems,2)")
            << QStringLiteral("foldr:=(f,z,elems)->piecewise {card(elems)=0 ? z, ? f(elems[1], foldr(f, z, tail(elems))) }")
            << QStringLiteral("cfilter:=(condition,elems)->foldr((v,pred)->piecewise{ condition(v) ? union(list{v}, pred), ? pred }, list{}, elems)")
            
            << QStringLiteral("cfilter(x->x>3, list{1,2,3,4,5})");
    QTest::newRow("custom filter") << script << "list { 4, 5 }";
    
    script.clear();
    script
            << QStringLiteral("pmap:=(func, list, i)->piecewise { i>=card(list)+1 ? list {}, ? union(list { func(selector(i, list)) }, pmap(func, list, i+1)) }")
            << QStringLiteral("cmap:=(func, list)->pmap(func, list, 1)")
            << QStringLiteral("refImport:=x->x>3")
            << QStringLiteral("importedRef:=imports->cmap(refImport, imports)")
            
            << QStringLiteral("importedRef(list{1,2,3,4,5})");
    QTest::newRow("custom map") << script << "list { false, false, false, true, true }";
    
    script.clear();
    script
            << QStringLiteral("f:=v->sum(i**2 : i@v)")
            << QStringLiteral("g:=(u, v)->f(u)+f(v)")
            << QStringLiteral("g(vector{1,2}, vector{3,4})");
    QTest::newRow("aaa") << script << "30";

    script.clear();
    script << QStringLiteral("f := (w,zz) -> list{zz} | acs->forall(a<w : a@acs)");
    script << QStringLiteral("f(2,3)");
    QTest::newRow("lambda1") << script << "false";

    script.clear();
    script << QStringLiteral("f := x -> matrix { matrixrow { x, x } }");
    script << QStringLiteral("f(90)");
    QTest::newRow("matrix-f") << script << "matrix { matrixrow { 90, 90 } }";

    script.clear();
    script << QStringLiteral("rotate := angle -> matrix { matrixrow { cos(angle), -sin(angle) }, matrixrow { sin(angle), cos(angle) } }");
    script << QStringLiteral("rotate(90) * vector { 20, 20 }");
    QTest::newRow("rotate-vector") << script << "vector { -26.8414055946, 8.91846094943 }";
}

//testCalculate
void AnalitzaTest::testCorrection()
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
    
    Analitza::Analyzer b;
    Expression evalResult;
    foreach(const QString &exp, expression) {
        Expression e(exp, false);
        QVERIFY(e.isCorrect());
        
        b.setExpression(e);
        QVERIFY(b.isCorrect());
        evalResult=b.evaluate();
        QVERIFY(b.isCorrect());
    }
    QCOMPARE(evalResult.toString(), result);

    QString script = expression.join(QStringLiteral("\n"));
    script+=QLatin1String("\n\n\n");
    QTextStream stream(&script);
    a->importScript(&stream);
    QVERIFY(a->isCorrect());
}

void AnalitzaTest::testTypeUncorrection()
{
    QFETCH(QStringList, expression);
    
    bool correct=false;
    Analitza::Analyzer b;
    
    foreach(const QString &exp, expression) {
        Expression e(exp, false);
        b.setExpression(e);
        correct=b.isCorrect();
        
        if(correct)
            b.calculate().toReal().value();
        
        if(!correct || !b.isCorrect())
            break;
    }
    QVERIFY(!correct);
}

void AnalitzaTest::testTypeUncorrection_data()
{
    QTest::addColumn<QStringList>("expression");
    QTest::newRow("vect+sin") << QStringList(QStringLiteral("3+sin(vector{3,4,2})"));
    QTest::newRow("scalar+card") << QStringList(QStringLiteral("card(3)"));
    QTest::newRow("wrong operation") << QStringList(QStringLiteral("lcm(vector{0}, vector{0})"));
    
    QStringList script;
    script << QStringLiteral("x:=3");
    script << QStringLiteral("x(3)");
    QTest::newRow("value call") << script;
    
    script.clear();
    script << QStringLiteral("f:=(x,y)->x*y");
    script << QStringLiteral("f(3)");
    QTest::newRow("call missing parameter") << script;
}

void AnalitzaTest::testUncorrection_data()
{
    QTest::addColumn<QStringList>("expression");
    QTest::newRow("summatory with uncorrect downlimit1") << QStringList(QStringLiteral("sum(x : x=y..3)"));
    QTest::newRow("summatory with uncorrect downlimit2") << QStringList(QStringLiteral("sum(x : x=x..3)"));
    QTest::newRow("wrong sum") << QStringList(QStringLiteral("sum(x : x=10..0)"));
    
    QStringList script;
    script << QStringLiteral("a:=b");
    script << QStringLiteral("b:=a");
    QTest::newRow("var dependency cycle") << script;
    
    
    QTest::newRow("unsupported diff") << QStringList(QStringLiteral("diff(arccos(x):x)"));
}

void AnalitzaTest::testUncorrection()
{
    QFETCH(QStringList, expression);
    
    bool correct=false;
    Analitza::Analyzer b;
    foreach(const QString &exp, expression) {
        Expression e(exp, false);
        correct=e.isCorrect();
        
        if(correct) {
            b.setExpression(e);
            Expression res=b.evaluate();
            correct=b.isCorrect();
        }
//         qDebug() << "cycle" << b.isCorrect() << e.toString() << b.errors();
        if(!correct) break;
    }
//     QVERIFY(!correct);
    
    foreach(const QString &exp, expression) {
        Expression e(exp, false);
        correct=e.isCorrect();
        b.setExpression(e);
        
        if(correct) {
            /*double val=*/b.calculate().toReal().value();
            correct=b.isCorrect();
//             qDebug() << "aaaaaaaaagh"  << b.errors() << val << correct;
        }
        if(!correct) break;
    }
    QVERIFY(!correct);
}

void AnalitzaTest::testSimplify_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("result");
    
    QTest::newRow("identity") << "1*x" << "x";
    QTest::newRow("minus") << "x-x-x" << "-x";
    QTest::newRow("minus1") << "x-1" << "x-1";
    QTest::newRow("minus2") << "x-2*x" << "-x";
    QTest::newRow("compensation") << "-(4*x)+3*x" << "-x";
    QTest::newRow("compensation1") << "(-(4*x))+3*x" << "-x";
    QTest::newRow("compensation2") << "((-4)*x)+3*x" << "-x";
    QTest::newRow("compensation*") << "-(x^4)*x^3" << "-x^7";
    QTest::newRow("powers") << "3**x*5**x" << "3^x*5^x";
    QTest::newRow("poli1") << "x-1+2" << "x+1";
    QTest::newRow("poli2") << "(x+y)-z" << "x--y-z";
    QTest::newRow("poli3") << "2-13-(x+1)" << "-x-12";
    QTest::newRow("poli4") << "-x-1-2-4" << "-x-7";
    QTest::newRow("poli4.0") << "-x-y-z" << "-x-y-z";
    QTest::newRow("poli4.1") << "minus(-x, 1, 2, 4)" << "-x-7";
    QTest::newRow("poli5") << "y+3*(x-1)" << "y+3*(x-1)";
//     QTest::newRow("powerscomb") << "3**x*3**x" << "9^x";
    QTest::newRow("no var") << "2+2" << "4";
    QTest::newRow("simple") << "x+x" << "2*x";
    QTest::newRow("lambda") << "(x->x+1)(2)" << "3";
    QTest::newRow("lambda1") << "(x->x+1)(y)" << "y+1";
    QTest::newRow("lambda2") << "(x->x+1)(x+1)" << "x+2";
    QTest::newRow("lambda3") << "zz->(x->card(x)>0)(list{zz})" << "zz->card(list { zz })>0";
    QTest::newRow("lambda4") << "f(3) | a-> (g(a) | b-> useForIndex(a, b))" << "(a->(b->useForIndex(a, b))(g(a)))(f(3))";
//     QTest::newRow("lambda3")<< "(x->x+x)(y)" << "2*y";
    QTest::newRow("diff") << "diff(x^2:x)" << "x->2*x";
    QTest::newRow("sum times") << "sum(n*x : n=0..99)" << "4950*x";
    QTest::newRow("levelout") << "-y-(x+y)" << "-2*y-x";
    QTest::newRow("sum") << "n->sum((s+n) * s : s=0..9)" << "n->sum((s+n)*s:s=0..9)";
    QTest::newRow("sum.sum") << "k->sum(sum(x:x=0..s):s=0..k)" << "k->sum(sum(x:x=0..s):s=0..k)";
    QTest::newRow("unrelated sum") << "sum(x : n=0..99)" << "100*x";
    QTest::newRow("ln") << "ln(x)" << "ln(x)";
    
    QTest::newRow("piecewise1") << "piecewise { 1=2 ? 4, ? 3}" << "3";
    QTest::newRow("piecewise2") << "piecewise { x=2 ? 4, ? 3}" << "piecewise { x=2 ? 4, ? 3 }";
    QTest::newRow("piecewise3") << "piecewise { 2=2 ? 4, ? 3}" << "4";
    
    QTest::newRow("sum.dlul") << "w->sum(x : x=(floor(2.5)+w)..(ceiling(2.5)))" << "w->sum(x:x=w+2..3)";
    QTest::newRow("sum.times") << "sum(2*x : x=0..y)" << "2*sum(x:x=0..y)";
    QTest::newRow("trig") << "sin(x)/cos(x)" << "sin(x)/cos(x)";
    
    QTest::newRow("mono") << "2*x*y+3*x*y" << "5*x*y";
    QTest::newRow("mono1") << "2*y+y" << "3*y";
    QTest::newRow("mono2") << "-y+1" << "-y+1";
    
    QTest::newRow("matrix") << "matrix { matrixrow { x, y, z } }" << "matrix { matrixrow { x, y, z } }";
    
    //equations
    QTest::newRow("eqminus") << "x-3=0" << "x=3";
    QTest::newRow("eqplus") << "x+3=0" << "x=-3";
    QTest::newRow("eqtimes") << "3x=0" << "x=0";
    QTest::newRow("eqtimes1") << "(x-3)*(x-2)=0" << "or(x=3, x=2)";
    QTest::newRow("eqdiv") << "x/2=0" << "x=0";
    QTest::newRow("eqdiv1") << "(x-1)/2=0" << "x=1";
    QTest::newRow("eqdiv2") << "(x*(x-1))/x=0" << "x=1";
    QTest::newRow("eqdiv3") << "(x*(x-1))/(x+3)=0" << "or(x=0, x=1)";
    QTest::newRow("eqsin") << "sin(x)=0" << "x=0";
    QTest::newRow("eqcos") << "cos(x)=1" << "x=0";
    QTest::newRow("eqmin") << "x=3-2" << "x=1";
    QTest::newRow("different") << "x+3=x+2" << "false";
}

void AnalitzaTest::testSimplify()
{
    QFETCH(QString, expression);
    QFETCH(QString, result);
    
    a->setExpression(Expression(expression, false));
    if(!a->isCorrect()) qDebug() << "error:" << a->errors();
    QVERIFY(a->isCorrect());
    a->simplify();
    QCOMPARE(a->expression().toString(), result);
}

void AnalitzaTest::testEvaluate_data()
{
    QTest::addColumn<QStringList>("expression");
    QTest::addColumn<QString>("result");
    
    QStringList script;
    script << QStringLiteral("f:=x->x");
    script << QStringLiteral("f(x)");
    QTest::newRow("function parameter") << script << "x";
    
    script.clear();
    script << QStringLiteral("pu:=n->sum(p**i:i=0..floor(n))");
    script << QStringLiteral("pu(3)");
    QTest::newRow("calls") << script << "sum(p^i:i=0..3)";
}

void AnalitzaTest::testEvaluate()
{
    QFETCH(QStringList, expression);
    QFETCH(QString, result);
    
    Analitza::Analyzer b;
    Expression res;
    foreach(const QString &exp, expression) {
        Expression e(exp, false);
        if(!e.isCorrect()) qDebug() << "XXXX" << e.error();
        QVERIFY(e.isCorrect());
        
        b.setExpression(e);
        if(!b.isCorrect()) qDebug() << "XXXX" << b.errors();
        QVERIFY(b.isCorrect());
        res=b.evaluate();
        
        if(!b.isCorrect()) qDebug() << "XXXX" << b.errors();
        QVERIFY(b.isCorrect());
//         b.calculate(); //we can do that just if we know that all variables doesn't have dependencies
    }
    QCOMPARE(res.toString(), result);
}

void AnalitzaTest::testVector()
{
    QFETCH(QString, expression);
    QFETCH(QString, result);
    Expression e(expression, false);
    QCOMPARE(e.isCorrect(), true);
    
    a->setExpression(e);
    if(!a->isCorrect()) qDebug() << "error:" << a->errors();
    QVERIFY(a->isCorrect());
    QCOMPARE(a->calculate().toString(), result);
    QCOMPARE(a->evaluate().toString(), result);
}

void AnalitzaTest::testVector_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("result");

    QTest::newRow("avector") << "vector { 1, 2, 3 }" << "vector { 1, 2, 3 }";
    QTest::newRow("card(vect)") << "card(vector { 1, 2, 3 })" << "3";
    QTest::newRow("in-vector operations") << "vector { 2+2, 3*3, 3^3 }" << "vector { 4, 9, 27 }";
    
    QTest::newRow("vect+vect") << "vector { 1, 2, 3 }+vector { 3, 2, 1 }" << "vector { 4, 4, 4 }";
    QTest::newRow("vect+vect2") << "vector { 1, 2, 3 }+vector { 3, 2, sin(pi/2) }" << "vector { 4, 4, 4 }";
    QTest::newRow("vect*scalar") << "vector { 1, 2, 3 }*3" << "vector { 3, 6, 9 }";
    QTest::newRow("scalar*vect") << "3*vector { 1, 2, 3 }" << "vector { 3, 6, 9 }";
    
    QTest::newRow("sum") << "sum(vector {x,x,x} : x=1..99)" << "vector { 4950, 4950, 4950 }";
    QTest::newRow("product") << "product(vector {x,x,x} : x=1..5)" << "vector { 120, 120, 120 }";
    
    QTest::newRow("selector1+vector") << "selector(1, vector{1,2,3})" << "1";
    QTest::newRow("selector2+vector") << "selector(2, vector{1,2,3})" << "2";
    QTest::newRow("selector3+vector") << "selector(3, vector{1,2,3})" << "3";
    
    QTest::newRow("selector1+list") << "selector(1, list{1,2,3})" << "1";
    QTest::newRow("selector2+list") << "selector(2, list{1,2,3})" << "2";
    QTest::newRow("selector3+list") << "selector(3, union(list{1,2}, list{3}))" << "3";
    QTest::newRow("union") << "union(list{1,2}, list{3})" << "list { 1, 2, 3 }";
}

void AnalitzaTest::testCrash_data()
{
    QTest::addColumn<QString>("expression");
    
    QTest::newRow("undefined variable") << "x";
    QTest::newRow("selector overflow") << "selector(9, vector{1,2})";
    QTest::newRow("selector underflow") << "selector(0, vector{1,2})";
    QTest::newRow("simple piecewise") << "piecewise { pi=0? 3, eq(pi, pi)?33 }";
    QTest::newRow("oscarmartinez piecewise") << "piecewise { gt(x,23)?a }";
    QTest::newRow("vector+ovf") << "selector(2, vector{x})";
    QTest::newRow("wrong func") << "xsin(x)";
    QTest::newRow("scalarprod") << "scalarproduct(vector{0}, vector{x,0})";
    QTest::newRow("power") << "list{}**2";
    QTest::newRow("unary-nested-error") << "-(2/0)";
}

void AnalitzaTest::testCrash()
{
    QFETCH(QString, expression);
    Expression e(expression, Expression::isMathML(expression));
    QVERIFY(e.isCorrect());
    
    a->setExpression(e);
    a->evaluate();
    a->calculate();
    
    //We don't want it to crash, so we try to
    for(int i=0; i<expression.size(); i++)
    {
        QString aux=expression.left(i);
        QString aux1=expression.right(i);
        
        Expression e1(aux, false);
        Expression e2(aux, true);
        
        Expression e3(aux1, false);
        Expression e4(aux1, true);
    }
}

void AnalitzaTest::testOperators_data()
{
    QTest::addColumn<int>("i");
    
    for(int i=Operator::none+1; i<Operator::nOfOps; i++) {
        QTest::newRow( Operator::words[i] ) << i;
    }
}

void AnalitzaTest::testOperators()
{
    QFETCH(int, i);
    Operator o(static_cast<Operator::OperatorType>(i));
    QVERIFY(o.nparams()>=-1);
    if(!o.isCorrect())
        qDebug() << o.toString();
    QVERIFY(o.isCorrect());
    QCOMPARE(static_cast<Operator::OperatorType>(i), o.operatorType());
    QCOMPARE(Operator::toOperatorType(o.toString()), o.operatorType());
    
    if(o.operatorType()==Operator::function)
        return;
    
//     QVERIFY(!Analitza::Operations::infer(o.operatorType()).isEmpty() || !Analitza::Operations::inferUnary(o.operatorType()).isEmpty());
    
    Vector* v=new Vector(3);
    v->appendBranch(new Cn(0.));
    v->appendBranch(new Cn(1.));
    v->appendBranch(new Cn(2.));
    
    QList<Object*> values=QList<Object*>()    << new Cn(0.)
                                            << new Cn(0.5)
                                            << new Cn(1.)
                                            << new Cn(-1.)
                                            << new Cn(-.5)
                                            << new Ci(QStringLiteral("x"))
                                            << v; //lets try to make it crash
    QList<int> params;
    if(o.nparams()<0)
        params /*<< 0 << 1 << 2*/ << 3;
    else
        params << o.nparams();
    
#ifdef Q_CC_GNU
    #warning improve the test for bounded operations
#endif    
    if(o.operatorType()==Operator::sum || o.operatorType()==Operator::product) 
        return;
    
    foreach(Object* obj, values) {
        foreach(int paramCnt, params) {
            Analitza::Apply* apply=new Analitza::Apply;
            apply->appendBranch(new Operator(o));
            for(; paramCnt>0; paramCnt--)  {
                apply->appendBranch(obj->copy());
            }
            
            if(o.isBounded()) {
                Container *bvar=new Container(Container::bvar);
                apply->appendBranch(bvar);
                
                QList<Object*> bvarValues=QList<Object*>() << new Ci(QStringLiteral("x"));
                foreach(Object* obvar, bvarValues) {
                    Analitza::Apply* cc=(Analitza::Apply*) apply->copy();
                    Container* bvar=(Container*) cc->bvarCi().at(0);
                    bvar->appendBranch(obvar->copy());
                    
                    Expression e1(cc);
                    a->setExpression(e1);
                    
                    a->calculate();
                    a->evaluate();
                    a->derivative(QStringLiteral("x"));
                }
                qDeleteAll(bvarValues);
            } else {
                Expression e(apply);
                a->setExpression(e);
                a->calculate();
                a->evaluate();
                a->derivative(QStringLiteral("x"));
            }
        }
    }
    qDeleteAll(values);
    
    QList<double> diffValues = QList<double>() << 0. << 0.5 << -0.5 << 1. << -1.;
    QString bvar('x');
    foreach(double v, diffValues) {
        foreach(int paramCnt, params) {
            Analitza::Apply *diffApply=new Analitza::Apply;
            diffApply->appendBranch(new Operator(Operator::diff));
            Container* diffBVar=new Container(Container::bvar);
            diffBVar->appendBranch(new Ci(bvar));
            diffApply->appendBranch(diffBVar);
            
            Analitza::Apply* apply=new Analitza::Apply;
            apply->appendBranch(new Operator(o));
            diffApply->appendBranch(apply);
            
            for(; paramCnt>0; paramCnt--)
                apply->appendBranch(new Ci(bvar));
            
            Expression e(diffApply);
            a->setExpression(e);
            a->calculate();
            a->evaluate();
            a->simplify();
            a->derivative(QStringLiteral("x"));
            
            Cn* vv = new Cn(v);
            QVector<Object*> stack;
            stack += vv;
            a->derivative(stack);
            delete vv;
        }
    }
}


