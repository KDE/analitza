/*************************************************************************************
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

#include "surfacetest.h"
#include "analitzaplot/surface.h"
#include <plotsfactory.h>
#include "analitza/expression.h"
#include "analitza/variables.h"
#include <QTest>
#include <cmath>

using namespace std;
using namespace Analitza;

QTEST_MAIN( SurfaceTest )

SurfaceTest::SurfaceTest(QObject *parent)
    : QObject(parent)
{
    m_vars=new Analitza::Variables;
}

SurfaceTest::~SurfaceTest()
{
    delete m_vars;
}

void SurfaceTest::initTestCase()
{}

void SurfaceTest::cleanupTestCase()
{}

void SurfaceTest::testCorrect_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("sphere-cartcoords") << "x*x+y*y+z*z=6";
    QTest::newRow("sphere-sphecoords") << "(t,p)->6";
}

void SurfaceTest::testCorrect()
{
    QFETCH(QString, input);

    QVERIFY(PlotsFactory::self()->requestPlot(Expression(input), Dim3D).canDraw());
}

void SurfaceTest::testIncorrect_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("hiper-surf") << "x*x+y*y+z*z=w";
    QTest::newRow("wrong-inf") << "(x,y)->x/0";
    QTest::newRow("wrong-nan-1stcomp") << "(u,v)->{u/0 - v/0, 5*u, -7*v}";
    QTest::newRow("wrong-dimension-uv") << "(u,v)->{u, v, 5*u, -7*v}";
}

void SurfaceTest::testIncorrect()
{
    QFETCH(QString, input);

    PlotBuilder rp = PlotsFactory::self()->requestPlot(Expression(input), Dim3D);
    if(rp.canDraw()) {
        FunctionGraph* f = rp.create(Qt::red, QStringLiteral("lala"));
        Surface* surface = dynamic_cast<Surface*>(f);
        QVERIFY(surface);
        
        surface->update(QVector3D(-1,-1,-1), QVector3D(1,1,1));
        QVERIFY(!f->isCorrect() || surface->vertices().isEmpty());
    }
}

//TODO
// typedef QPair<double, double> IntervalValue;
// Q_DECLARE_METATYPE(IntervalValue)
// 
// typedef QPair<Analitza::Expression, Analitza::Expression> IntervalExpression;
// Q_DECLARE_METATYPE(IntervalExpression)
// 
// void SurfaceTest::testParamIntervals_data()
// {
//     QTest::addColumn<QString>("input");
//     QTest::addColumn<QString>("param");
//     QTest::addColumn<IntervalValue>("interval_value");
//     QTest::addColumn<IntervalExpression>("interval_expression");
// 
//     QTest::newRow("simple_interval_vals") << "x->x*x" << "x" <<
//                                           qMakePair(-7.0, 5.0) << qMakePair(Analitza::Expression("a+b-4"), Analitza::Expression("16"));
// 
//     QTest::newRow("implicit_curve_1_interval_vals") << "x*x+y*y=8" << "y" <<
//             qMakePair(-9.0+2, 15.0) << qMakePair(Analitza::Expression("-abs(a*b)"), Analitza::Expression("cos(0)*a*a"));
// }
// 
// void SurfaceTest::testParamIntervals()
// {
//     QFETCH(QString, input);
//     QFETCH(QString, param);
//     QFETCH(IntervalValue, interval_value);
//     QFETCH(IntervalExpression, interval_expression);
// 
//     m_vars->modify("a", -4.0);
//     m_vars->modify("b", -9.5);
// 
//     PlotItem* plot = PlotsFactory::self()->requestPlot(Expression(input), Dim2D).create(Qt::red, "hola", m_vars);
//     PlaneCurve* f3 = dynamic_cast<PlaneCurve*>(plot);
//     QVERIFY(f3->isCorrect());
// 
//     QVERIFY(f3->setInterval(param, interval_value.first, interval_value.second));
//     QCOMPARE(f3->interval(param).first, -7.0);
// 
//     //Interval as expression
//     QVERIFY(f3->setInterval(param, interval_expression.first, interval_expression.second));
//     QCOMPARE(f3->interval(param, true).second.toString(), QStringLiteral("16"));
//     
//     delete m_vars->take("a");
//     delete m_vars->take("b");
// }


