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

    Curve curve(Expression(input), m_vars);
    curve.createGeometry();
    
    QVERIFY(curve.isValid());
}

void PlaneCurveTest::testIncorrect_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("empty expression") << "";
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

    Curve curve(Expression(input), m_vars);
    curve.createGeometry();
    
    QVERIFY(!curve.isValid());
}

#include "curvetest.moc"
