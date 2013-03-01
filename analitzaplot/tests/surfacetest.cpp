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
#include <qtest_kde.h>
#include <cmath>

using namespace std;
using namespace Analitza;

QTEST_KDEMAIN_CORE( SurfaceTest )

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
        FunctionGraph* f = rp.create(Qt::red, "lala");
        SurfaceOld* surface = dynamic_cast<SurfaceOld*>(f);
        QVERIFY(surface);
        
        surface->update(QVector3D(-1,-1,-1), QVector3D(1,1,1));
        QVERIFY(!f->isCorrect() || surface->vertices().isEmpty());
    }
}

#include "surfacetest.moc"
