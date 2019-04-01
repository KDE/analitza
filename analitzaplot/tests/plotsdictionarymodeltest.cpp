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

#include "plotsdictionarymodeltest.h"
#include <QTest>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <analitzaplot/plotitem.h>
#include <analitzaplot/plotsmodel.h>
#include <analitzaplot/plotsdictionarymodel.h>

using namespace Analitza;

QTEST_MAIN( PlotsDictionaryModelTest )

PlotsDictionaryModelTest::PlotsDictionaryModelTest(QObject *parent)
    : QObject(parent)
{}

PlotsDictionaryModelTest::~PlotsDictionaryModelTest()
{}

void PlotsDictionaryModelTest::testDictionaries()
{
    PlotsDictionaryModel m;
    m.clear(); //we don't want the installed, we want the ones in the source directory
    QFileInfo f(QFINDTESTDATA("../data/plots/3Ds.plots"));
    QDir d(f.dir());
    foreach(const QString& f, d.entryList(QStringList(QLatin1String("*.plots")), QDir::Files)) {
        m.createDictionary(d.filePath(f));
    }
    
    QVERIFY(m.rowCount()>0);
    PlotsModel* plot = m.plotModel();
    
    for(int i=0; i<m.rowCount(); i++) {
        QModelIndex idx = m.index(i, 0);
        QVariant v=idx.data(Qt::DisplayRole);
        QVERIFY(v.isValid());
        QVERIFY(v.type()==QVariant::String);
        QVERIFY(!v.toString().isEmpty());
        QVERIFY(!idx.data(PlotsDictionaryModel::ExpressionRole).toString().isEmpty());
        
        m.setCurrentRow(i);
        QCOMPARE(plot->rowCount(), 1);
        
        QModelIndex plotIdx = plot->index(0,0);
        PlotItem* item = plotIdx.data(PlotsModel::PlotRole).value<PlotItem*>();
        QVERIFY(item);
        QCOMPARE(idx.data(Qt::DisplayRole).toString(), item->name());
    }
}

