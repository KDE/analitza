/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#include "plotsdictionariesmanager.h"
#include "dictionariesmodel.h"
#include "planecurve.h"
#include "spacecurve.h"
#include "surface.h"
#include "dictionaryitem.h"
#include <analitza/expression.h>
#include <analitza/container.h>
#include <KStandardDirs>
#include <KLocale>
#include <QFile>
#include <QDomDocument>

PlotsDictionariesManager::PlotsDictionariesManager(QObject* parent)
: QObject(parent)
, m_model(0)
{
    m_model = new PlotsDictionariesModel(this);

    createDictionary("Dictionary1", "libanalitza/data/plots/es/basic_curves.plots");
    createDictionary("Conics", "libanalitza/data/plots/es/conics.plots");
    createDictionary("Polar", "libanalitza/data/plots/es/polar.plots");
}

PlotsDictionariesManager::~PlotsDictionariesManager()
{
    qDeleteAll(m_collections);
    m_collections.clear();
}

QStringList PlotsDictionariesManager::availableDictionaries() const
{
    return QStringList();
}

PlotsDictionariesModel* PlotsDictionariesManager::model() const
{
    return m_model;
}

void PlotsDictionariesManager::createDictionary(const QString& title, const QString& file)
{
    m_collections.append(new DictionaryItem(DimAll));
    m_collections.last()->setTitle(title);
    QString localurl = KStandardDirs::locate("data", file);

    QFile device(localurl);

    if (device.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&device);
        
        while(!stream.atEnd()) {
            QString line = stream.readLine();
            
            if (line.isEmpty())
                continue;
            
            Analitza::Expression expression(line);
            
            if (!expression.isCorrect()) {
                qDebug() << "wrong expression: " << line ;
                continue;
            }
            
            if (PlaneCurve::canDraw(expression).isEmpty())
            {
                PlaneCurve *plot = new PlaneCurve(expression);
                plot->setSpace(m_collections.last());
                
                m_model->addPlot(plot);
            }
        }
    }
}
