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
#include <analitza/expression.h>
#include <analitza/container.h>
#include <KStandardDirs>
#include <KLocale>
#include <QFile>
#include <QDomDocument>


///

PlotsDictionariesManager::PlotsDictionariesManager(QObject* parent)
: QObject(parent)
, m_model(0)
{
    m_model = new PlotsDictionariesModel(this);

    //TODO memory leaks spaces ... solved: this::dtor
    m_collections.append(new DictionaryItem(DimAll));
    m_collections.last()->setTitle("Dictionary1");
    QString localurl = KStandardDirs::locate("data", QString("libanalitza/data/plots/es/basic_curves.plots"));

    QFile device(localurl);

    if (device.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&device);
        
        QString line;
        
        do 
        {
            line = stream.readLine();
            
            if (line.isEmpty())
                continue;
            
            Analitza::Expression expression(line);
            
            if (!expression.isCorrect())
                continue;
            //TODO :)
            
            if (PlaneCurve::canDraw(expression))
            {
                PlaneCurve *plot = new PlaneCurve(expression);
                plot->setSpace(m_collections.last());
                
                m_model->addPlot(plot);
            }
        }
        while (!line.isNull());
    }

    
//     ///TODDDOO
//     
    m_collections.append(new DictionaryItem(DimAll));
    localurl = KStandardDirs::locate("data", QString("libanalitza/data/plots/es/conics.plots"));
    m_collections.last()->setTitle("Conics");

    QFile device2(localurl);

    if (device2.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&device2);
        
        QString line;
        
        do 
        {
            line = stream.readLine();
            
            if (line.isEmpty())
                continue;
            
            Analitza::Expression expression(line);
            
            if (!expression.isCorrect())
                continue;
            //TODO :)
            
            if (PlaneCurve::canDraw(expression))
            {
                PlaneCurve *plot = new PlaneCurve(expression);
                plot->setSpace(m_collections.last());
                
                m_model->addPlot(plot);
            }
        }
        while (!line.isNull());
    }    
    
    
//     ///TODDDOO
//     
    m_collections.append(new DictionaryItem(DimAll));
    localurl = KStandardDirs::locate("data", QString("libanalitza/data/plots/es/polar.plots"));
    m_collections.last()->setTitle("Polar plots");

    QFile device3(localurl);

    if (device3.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&device3);
        
        QString line;
        
        do 
        {
            line = stream.readLine();
            
            if (line.isEmpty())
                continue;
            
            Analitza::Expression expression(line);
            
            if (!expression.isCorrect())
                continue;
            //TODO :)
            
            if (PlaneCurve::canDraw(expression))
            {
                PlaneCurve *plot = new PlaneCurve(expression);
                plot->setSpace(m_collections.last());
                
                m_model->addPlot(plot);
            }
        }
        while (!line.isNull());
    }    
}

PlotsDictionariesManager::~PlotsDictionariesManager()
{
    //NOTE since space is opt for a plot item we have to delete all associated spaces
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


