/*************************************************************************************
 *  Copyright (C) 2012 by Aleix Pol Gonzalez <aleixpol@kde.org>                      *
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

#include "plotsmodeltest.h"

#include "analitzaplot/planecurve.h"
#include "analitzaplot/plotsmodel.h"

#include "analitza/expression.h"
#include "analitza/variables.h"
#include <analitza/analyzer.h>
#include <analitza/value.h>
#include <qtest_kde.h>
#include <cmath>

using namespace std;
using Analitza::Expression;

QTEST_KDEMAIN_CORE( PlotsModelTest )

PlotsModelTest::PlotsModelTest(QObject *parent)
    : QObject(parent)
{
    m_model = new PlotsModel(this);
}

PlotsModelTest::~PlotsModelTest()
{}

void PlotsModelTest::testAppend_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("x->flat") << "x->1";
    QTest::newRow("x->x") << "x->x";
    QTest::newRow("x->and") << "x->piecewise { and(x>-1, x<1) ? 1, ?0 }";
    QTest::newRow("x->abs") << "x->abs(x)";
    QTest::newRow("x->addition") << "x->2+x";
    QTest::newRow("x->minus") << "x->x-2";
    QTest::newRow("x->log") << "x->log x";
    QTest::newRow("x->tan") << "x->tan x";
    QTest::newRow("x->sqrt") << "x->root(x, 2)";
    QTest::newRow("x->factorof") << "x->factorof(x,x)";
    QTest::newRow("x->sum") << "x->sum(t : t=0..3)";
    QTest::newRow("x->piece") << "x->piecewise { gt(x,0) ? selector(1, vector{x, 1/x}),"
                              "? selector(2, vector{x, 1/x} ) }";
    QTest::newRow("x->diff1") << "x->(diff(x:x))(x)";
    QTest::newRow("x->diffx") << "x->(diff(x^2:x))(x)";
    QTest::newRow("y->flat") << "y->1";
    QTest::newRow("y->trigonometric") << "y->sin y";
    QTest::newRow("polar->scalar") << "q->2";
    QTest::newRow("polar->function") << "q->sin q";
    QTest::newRow("polar->hard") << "q->ceiling(q/(2*pi))";
    QTest::newRow("polar->strange") << "q->q/q";

    QTest::newRow("parametric") << "t->vector{t,t**2}";
    QTest::newRow("parametric1") << "t->vector{16*sin(t)^3, abs(t)^0.3*root(t,2)}";
    QTest::newRow("implicit") << "x+y=9";
}

void PlotsModelTest::testAppend()
{
    QFETCH(QString, input);

    PlaneCurve* item = new PlaneCurve(Expression(input), "Hola", Qt::yellow);;
    m_model->addPlot(item);
    QVERIFY(item->isCorrect());
    
    item->update(QRectF(-5,5,10,10));
    if(!item->isCorrect())
        qDebug() << "error" << item->errors();
}
