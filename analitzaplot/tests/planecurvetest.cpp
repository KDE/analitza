/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "planecurvetest.h"
#include "analitzaplot/planecurve.h"
#include <plotsfactory.h>
#include "analitza/expression.h"
#include "analitza/variables.h"
#include <qtest_kde.h>
#include <cmath>

using namespace std;
using namespace Analitza;

QTEST_KDEMAIN_CORE( PlaneCurveTest )

PlaneCurveTest::PlaneCurveTest(QObject *parent)
    : QObject(parent)
{
    m_vars=new Analitza::Variables;
}

PlaneCurveTest::~PlaneCurveTest()
{
    delete m_vars;
}

void PlaneCurveTest::initTestCase()
{}

void PlaneCurveTest::cleanupTestCase()
{}

void PlaneCurveTest::testCorrect_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("fx-diag-line") << "x->x";
    QTest::newRow("fy-diag-line") << "y->y";
    QTest::newRow("vector-diag-line") << "t->vector{t,t}";
    QTest::newRow("simple-algebraic") << "x*x+y*y=3";
}

void PlaneCurveTest::testCorrect()
{
    QFETCH(QString, input);

    QVERIFY(PlotsFactory::self()->requestPlot(Expression(input), Dim2D).canDraw());
}

void PlaneCurveTest::testIncorrect_data()
{
    QTest::addColumn<QString>("input");

    //QTest::newRow("empty function") << ""; // se elimina este test porque no se podran crear graficos sin una expresion correcta o con una vacia
    QTest::newRow("undefined var") << "x:=w";
    QTest::newRow("parametric-wrongvector") << "t->vector{3}";
    QTest::newRow("wrong-dimension") << "vector{2,3}";
    QTest::newRow("wrong-dimension-y") << "y->vector{2,3}";
    QTest::newRow("wrong-dimension-q") << "q->vector{2,3}";
    QTest::newRow("wrong-parametric") << "t->v";
    QTest::newRow("wrong-variable") << "x->x(x)";
    QTest::newRow("wrong-call") << "(x+1)(x+2)";
    QTest::newRow("wrong-inf") << "y->y/0";
    QTest::newRow("wrong-nan") << "x/0 + y/0 = 89";
    QTest::newRow("collision") << "(x,y)->5=x*y";

//     QTest::newRow("implicit.notindomain") << "(x,y)->3-sin(x)*sin(y)";

//     QTest::newRow("not a function") << "t";
}

void PlaneCurveTest::testIncorrect()
{
    QFETCH(QString, input);

    PlotBuilder rp = PlotsFactory::self()->requestPlot(Expression(input), Dim2D);
    if(rp.canDraw()) {
        FunctionGraph* f = rp.create(Qt::red, "lala");
        PlaneCurve* curve = dynamic_cast<PlaneCurve*>(f);
        QVERIFY(curve);
        
        curve->update(QRectF(-5,-5,10,10));
        QVERIFY(!f->isCorrect() || curve->points().isEmpty());
    }
}

void PlaneCurveTest::testJumps_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("jumps");

    QTest::newRow("tanx") << "x->tan x" << 6;
    QTest::newRow("divx") << "x->1/x" << 1;
}

void PlaneCurveTest::testJumps()
{
    QFETCH(QString, input);
    QFETCH(int, jumps);

    PlotItem* plot = PlotsFactory::self()->requestPlot(Expression(input), Dim2D).create(Qt::red, "hola");
    PlaneCurve* f3 = dynamic_cast<PlaneCurve*>(plot);
    QVERIFY(f3->isCorrect());
    f3->update(QRect(-10, 10, 20, -20));
    QVERIFY(f3->isCorrect());

    f3->image(QPointF(1,1));

    QCOMPARE(f3->jumps().count(), jumps);
}

typedef QPair<double, double> IntervalValue;
Q_DECLARE_METATYPE(IntervalValue)

typedef QPair<Analitza::Expression, Analitza::Expression> IntervalExpression;
Q_DECLARE_METATYPE(IntervalExpression)

void PlaneCurveTest::testParamIntervals_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("param");
    QTest::addColumn<IntervalValue>("interval_value");
    QTest::addColumn<IntervalExpression>("interval_expression");

    QTest::newRow("simple_interval_vals") << "x->x*x" << "x" <<
                                          qMakePair(-7.0, 5.0) << qMakePair(Analitza::Expression("a+b-4"), Analitza::Expression("16"));

    QTest::newRow("implicit_curve_1_interval_vals") << "x*x+y*y=8" << "y" <<
            qMakePair(-9.0+2, 15.0) << qMakePair(Analitza::Expression("-abs(a*b)"), Analitza::Expression("cos(0)*a*a"));
}

void PlaneCurveTest::testParamIntervals()
{
    QFETCH(QString, input);
    QFETCH(QString, param);
    QFETCH(IntervalValue, interval_value);
    QFETCH(IntervalExpression, interval_expression);

    m_vars->modify("a", -4.0);
    m_vars->modify("b", -9.5);

    PlotItem* plot = PlotsFactory::self()->requestPlot(Expression(input), Dim2D, m_vars).create(Qt::red, "hola");
    PlaneCurve* f3 = dynamic_cast<PlaneCurve*>(plot);
    QVERIFY(f3->isCorrect());

    QVERIFY(f3->setInterval(param, interval_value.first, interval_value.second));
    QCOMPARE(f3->interval(param).first, -7.0);

    //Interval as expression
    QVERIFY(f3->setInterval(param, interval_expression.first, interval_expression.second));
    QCOMPARE(f3->interval(param, true).second.toString(), QString("16"));
    
    delete m_vars->take("a");
    delete m_vars->take("b");
}

#include "planecurvetest.moc"
