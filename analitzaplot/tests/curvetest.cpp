/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#include "curvetest.h"
#include "analitzaplot/curve.h"
#include "analitza/expression.h"
#include "analitza/variables.h"
#include <analitza/builtinmethods.h>
#include <analitza/analyzer.h>
#include <qtest_kde.h>
#include <cmath>
#include <QColor>

using namespace std;
using namespace Analitza;

QTEST_KDEMAIN_CORE( PlaneCurveTest )

PlaneCurveTest::PlaneCurveTest(QObject *parent)
    : QObject(parent)
{
}

PlaneCurveTest::~PlaneCurveTest()
{
}

class ODE : public FunctionDefinition
{
    virtual Expression operator()(const QList< Expression >& args)
    {
        return Expression("sin(12)");
    }
};

void PlaneCurveTest::initTestCase()
{
    m_vars = new Analitza::Variables;
}

void PlaneCurveTest::cleanupTestCase()
{
    delete m_vars;
}

void PlaneCurveTest::testCopyCompare_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QColor>("color");
    QTest::addColumn<bool>("visible");
    
    QTest::newRow("growth-function") << "x->exp(x)" << "upupup" << QColor(Qt::red) << true;
    QTest::newRow("basic-implicit") << "x*x+y*y=6" << "circle" << QColor(Qt::blue) << false;
}

void PlaneCurveTest::testCopyCompare()
{
    QFETCH(QString, expression);
    QFETCH(QString, name);
    QFETCH(QColor, color);
    QFETCH(bool, visible);
    
    Curve curve1(Expression(expression), m_vars);
    curve1.setName(name);
    curve1.setColor(color);
    curve1.setVisible(false);
    
    Curve curve2(curve1);
    Curve curve3 = curve2;
    Curve curve4;
    
    QVERIFY(!curve4.isValid()); // curve4 is a invalid empty/null curve (null shape message in errors)
    
    curve4 = curve3;
    
    QVERIFY(curve4.isValid());
    QCOMPARE(curve1, curve4);
}

void PlaneCurveTest::testCorrectExpressions_data()
{
    QTest::addColumn<QString>("expression");
    
    QTest::newRow("fx-diag-line") << "x->x";
    QTest::newRow("fy-diag-line") << "y->y";
    QTest::newRow("vector-diag-line") << "t->vector{t,t}";
    QTest::newRow("simple-algebraic") << "x*x + y*y = 3";
    QTest::newRow("complex-implicit") << "abs(x)*sin(y)*x -y*y = x+y";
    QTest::newRow("vector-3d") << "t->vector{sin(t), cos(t), t}";
    QTest::newRow("ode-integral-curve-exp") << "x->list{1, y, 0, 1}"; // y'=y
    QTest::newRow("ode-kmplot-example-cos") << "x->list{2, -y, 0, 1, 0}"; // y''=-y
    QTest::newRow("ode-high-order-vars") << "x->list{6, -d2y + 3x - 1, 0, 1, 0, 0, 0, 0, 0}"; // y(6)=-y''+3x-1
}

void PlaneCurveTest::testCorrectExpressions()
{
    QFETCH(QString, expression);
    
    Curve curve(Expression(expression), m_vars);
    curve.createGeometry();
    
    QVERIFY(curve.isValid());
}

void PlaneCurveTest::testIncorrect_data()
{
    QTest::addColumn<QString>("expression");

    QTest::newRow("empty-expression") << "";
    QTest::newRow("undefined-var") << "x:=w";
    QTest::newRow("parametric-wrongvector") << "t->vector{3}";
    QTest::newRow("wrong-dimension") << "vector{2,3}";
    QTest::newRow("wrong-dimension-y") << "y->vector{2,3}";
    QTest::newRow("wrong-dimension-q") << "q->vector{2,3}";
    QTest::newRow("wrong-parametric") << "t->v";
    QTest::newRow("wrong-variable") << "x->x(x)";
    QTest::newRow("wrong-call") << "(x+1)(x+2)";
    QTest::newRow("not-a-function") << "t";
    QTest::newRow("not-a-2d-function") << "(x,y)->3-sin(x)*sin(y)";
    QTest::newRow("collision") << "(x,y)->5=x*y";
    QTest::newRow("bad-ode-empty") << "x->list{}";
    QTest::newRow("bad-ode-few-args") << "x->list{y,1}";
    QTest::newRow("bad-ode-order-not-int") << "x->list{45.98,y,0,1}";
    QTest::newRow("bad-ode-wrong-initconds") << "x->list{1,y,x,list{}}";
//     QTest::newRow("bad-ode-wrong-vars") << "x->list{t+v,1,0,1}";//TODO
    
    //TODO here need to implement createGeometry
//     QTest::newRow("wrong-inf") << "y->y/0";TODO
//     QTest::newRow("wrong-nan") << "x/0 + y/0 = 89";TODO
}

void PlaneCurveTest::testIncorrect()
{
    QFETCH(QString, expression);
    
    Curve curve(Expression(expression), m_vars);
    curve.createGeometry();
    
    QVERIFY(!curve.isValid());
}

#include "curvetest.moc"
