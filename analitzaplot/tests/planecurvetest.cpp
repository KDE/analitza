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
#include "analitza/expression.h"
#include "analitza/variables.h"
#include <qtest_kde.h>
#include <QPen>
#include <cmath>

using namespace std;
using Analitza::Expression;

QTEST_KDEMAIN_CORE( FunctionTest )

static const int resolution=3000;

FunctionTest::FunctionTest(QObject *parent)
    : QObject(parent)
{
    m_vars=new Analitza::Variables;
}

FunctionTest::~FunctionTest()
{
    delete m_vars;
}

void FunctionTest::initTestCase()
{}

void FunctionTest::cleanupTestCase()
{}

// void FunctionTest::testCopy_data()
// {
//     QTest::addColumn<QString>("input");
//     
//     QTest::newRow("x->flat") << "x->1";
//     QTest::newRow("x->x") << "x->x";
//     QTest::newRow("x->and") << "x->piecewise { and(gt(x,-1), lt(x,1)) ? 1, ?0 }";
//     QTest::newRow("x->abs") << "x->abs(x)";
//     QTest::newRow("x->addition") << "x->2+x";
//     QTest::newRow("x->minus") << "x->x-2";
//     QTest::newRow("x->log") << "x->log x";
//     QTest::newRow("x->tan") << "x->tan x";
//     QTest::newRow("x->sqrt") << "x->root(x, 2)";
//     QTest::newRow("x->factorof") << "x->factorof(x,x)";
//     QTest::newRow("x->sum") << "x->sum(t : t=0..3)";
//     QTest::newRow("x->piece") << "x->piecewise { gt(x,0) ? selector(1, vector{x, 1/x}),"
//                                     "? selector(2, vector{x, 1/x} ) }";
//     QTest::newRow("x->diff1") << "x->(diff(x:x))(x)";
//     QTest::newRow("x->diffx") << "x->(diff(x^2:x))(x)";
//     QTest::newRow("y->flat") << "y->1";
//     QTest::newRow("y->trigonometric") << "y->sin y";
//     QTest::newRow("polar->scalar") << "q->2";
//     QTest::newRow("polar->function") << "q->sin q";
//     QTest::newRow("polar->hard") << "q->ceiling(q/(2*pi))";
//     QTest::newRow("polar->strange") << "q->q/q";
//     
//     QTest::newRow("parametric") << "t->vector{t,t**2}";
//     QTest::newRow("parametric1") << "t->vector{16*sin(t)^3, abs(t)^0.3*root(t,2)}";
//     QTest::newRow("implicit") << "(x,y)->x+y";
// }

// void FunctionTest::testCopy()
// {
//     QFETCH(QString, input);
//     Expression exp(input, false);
//     
//     PlaneCurve f(exp, m_vars, "hola", Qt::red/*, 0,0*/);
//     if(!f.isCorrect()) qDebug() << "error:" << f.errors();
//     QVERIFY(f.isCorrect());
// //     f.setResolution(resolution); // Resolution/DrawingPrecision removed
//     if(!f.isCorrect()) qDebug() << "error:" << f.errors();
//     QVERIFY(f.isCorrect());
//     PlaneCurve f2(f.expression(), m_vars, f.name(), f.color());
//     if(!f2.isCorrect()) qDebug()<< "error" << f2.errors();
//     QVERIFY(f2.isCorrect());
//     
//     PlaneCurve f3(f2.expression(), m_vars, f2.name(), f2.color());
//     QVERIFY(f3.isCorrect());
//     f3.image(QPointF(1,1));
//     if(!f3.isCorrect()) qDebug() << "error" << f3.errors();
//     QVERIFY(f3.isCorrect());
//     QRectF viewp(QPoint(-12, 10), QPoint(12, -10));
//     f3.update(viewp.toRect());
//     
//     QVERIFY(f3.points().size()>1);
// //     QVERIFY(f3.points().size()<=int(f3.resolution())); // Resolution/DrawingPrecision removed
//     f3.update(viewp.toRect());
//     
// //  bool found=false;
// //  foreach(const QPointF& pt, f3.points()) {
// //      if(viewp.contains(pt)) {
// //          found=true;
// //          break;
// //      }
// //  }
// //  QVERIFY(found);
//     
//     QCOMPARE(f3.expression().toString(), exp.toString());
//     
//     int ant=-1;
//     foreach(int pos, f3.jumps()) {
//         QVERIFY(pos>ant);
//         ant=pos;
//     }
//     
//     f3.tangent(QPointF(0., 0.));
//     f3.tangent(QPointF(1., 0.));
//     f3.image(QPointF(0., 0.));
//     f3.image(QPointF(1., 0.));
//     f3.image(QPointF(109., 20.));
// }

void FunctionTest::testCorrect_data()
{
    QTest::addColumn<QString>("input");
    
    //QTest::newRow("empty function") << ""; // se elimina este test porque no se podran crear graficos sin una expresion correcta o con una vacia
    QTest::newRow("undefined var") << "x:=w";
    QTest::newRow("parametric-novector") << "t->3";
    QTest::newRow("parametric-wrongvector") << "t->vector{3}";
    QTest::newRow("wrong-dimension") << "vector{2,3}";
    QTest::newRow("wrong-dimension-y") << "y->vector{2,3}";
    QTest::newRow("wrong-dimension-q") << "q->vector{2,3}";
    QTest::newRow("wrong-parametric") << "t->v";
    QTest::newRow("wrong-variable") << "x->x(x)";
    QTest::newRow("wrong-call") << "(x+1)(x+2)";
    
    QTest::newRow("implicit.notindomain") << "(x,y)->3-sin(x)*sin(y)";
    
    QTest::newRow("not a function") << "t";
}

void FunctionTest::testCorrect()
{
    QFETCH(QString, input);
    
    if(PlaneCurve::canDraw(Analitza::Expression(input))) {
        PlaneCurve f3(Expression(input, false), m_vars, "hola", Qt::red/*, 0,0*/);

//         f3.setResolution(resolution); // Resolution/DrawingPrecision removed
        f3.image(QPointF(1,1));

        //solo se tienen graficos con expresiones correctas
//         if(f3.isCorrect())
//             f3.update(QRect(-10, 10, 10, -10));
        
        f3.update(QRect(-10, 10, 10, -10));
        
        if(f3.isCorrect())
        {
            QVERIFY(f3.points().size()>1);
            QVERIFY(f3.points().size()<=100);
        }
        
        QVERIFY(!f3.isCorrect());
    }
//     esle
//         QVERIFY(!f3.isCorrect());
}

void FunctionTest::testJumps_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("jumps");
    
    QTest::newRow("tanx") << "x->tan x" << 6;
    QTest::newRow("divx") << "x->1/x" << 1;
}

void FunctionTest::testJumps()
{
    QFETCH(QString, input);
    QFETCH(int, jumps);
    
    PlaneCurve f3(Expression(input, false), m_vars, "hola", Qt::red/*, 0,0*/);
    QVERIFY(f3.isCorrect());
    
//     f3.setResolution(resolution); // Resolution/DrawingPrecision removed
    f3.image(QPointF(1,1));
    QVERIFY(f3.isCorrect());
    f3.update(QRect(-10, 10, 20, -20));
    
    QVERIFY(f3.isCorrect());
    QCOMPARE(f3.jumps().count(), jumps);
}

typedef QPair<double, double> IntervalValue;
Q_DECLARE_METATYPE(IntervalValue)

typedef QPair<Analitza::Expression, Analitza::Expression> IntervalExpression;
Q_DECLARE_METATYPE(IntervalExpression)

void FunctionTest::testParamIntervals_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("param");
    QTest::addColumn<IntervalValue>("interval_value");
    QTest::addColumn<IntervalExpression>("interval_expression");
    
    QTest::newRow("simple_interval_vals") << "x->x*x" << "x" << 
        qMakePair(-7.0, 5.0) << qMakePair(Analitza::Expression("a+b-4"), Analitza::Expression("16"));
        
    QTest::newRow("implicit_curve_1_interval_vals") << "(x,y)->x*x+y*y-8" << "y" << 
        qMakePair(-9.0+2, 15.0) << qMakePair(Analitza::Expression("-abs(a*b)"), Analitza::Expression("cos(0)*a*a"));
}

void FunctionTest::testParamIntervals()
{
    QFETCH(QString, input);
    QFETCH(QString, param);
    QFETCH(IntervalValue, interval_value);
    QFETCH(IntervalExpression, interval_expression);

    m_vars->modify("a", -4.0);
    m_vars->modify("b", -9.5);
    
    PlaneCurve f3(Expression(input, false), m_vars, "hola", Qt::red/*, 0,0*/);
    QVERIFY(f3.isCorrect());
    
    f3.setInterval(param, interval_value.first, interval_value.second);
    QCOMPARE(f3.interval(param).first, -7.0);

    //Interval as expression
    f3.setInterval(param, interval_expression.first, interval_expression.second);

    QCOMPARE(f3.interval(param, true).second.toString(), QString("16"));
}

#include "planecurvetest.moc"
