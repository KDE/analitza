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

    DictionaryItem *space = new DictionaryItem(DimAll);
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
//                 plot->setSpace(space);
                
                m_model->addPlot(plot);
            }
        }
        while (!line.isNull());
    }

    
//     ///TODDDOO
//     
//     space = dicts->addSpace(DimAll, "Hola2", "holadescrp2");
//     localurl = KStandardDirs::locate("data", QString("libanalitza/data/plots/es/conics.plots"));
// 
//     QFile device2(localurl);
// 
//     if (device2.open(QFile::ReadOnly | QFile::Text))
//     {
//         QTextStream stream(&device2);
//         
//         QString line;
//         
//         do 
//         {
//             line = stream.readLine();
//             
//             if (line.isEmpty())
//                 continue;
//             
//             Analitza::Expression expression(line);
//             
//             if (!expression.isCorrect())
//                 continue;
//             //TODO :)
//             
//             if (PlaneCurve::canDraw(expression))
//             {
//                 PlaneCurve *plot = new PlaneCurve(expression);
//                 plot->setSpace(space);
//                 
//                 plots->addPlot(plot);
//             }
//         }
//         while (!line.isNull());
//     }    
//     
//     ///TODDDOO
//     
//     space = dicts->addSpace(DimAll, "hola3", "polarrde");
//     localurl = KStandardDirs::locate("data", QString("libanalitza/data/plots/es/polar.plots"));
// 
//     QFile device3(localurl);
// 
//     if (device3.open(QFile::ReadOnly | QFile::Text))
//     {
//         QTextStream stream(&device3);
//         
//         QString line;
//         
//         do 
//         {
//             line = stream.readLine();
//             
//             if (line.isEmpty())
//                 continue;
//             
//             Analitza::Expression expression(line);
//             
//             if (!expression.isCorrect())
//                 continue;
//             //TODO :)
//             
//             if (PlaneCurve::canDraw(expression))
//             {
//                 PlaneCurve *plot = new PlaneCurve(expression);
//                 plot->setSpace(space);
//                 
//                 plots->addPlot(plot);
//             }
//         }
//         while (!line.isNull());
//     }    
}

PlotsDictionariesManager::~PlotsDictionariesManager()
{

}

QStringList PlotsDictionariesManager::availableDictionaries() const
{
    return QStringList();
}

PlotsDictionariesModel* PlotsDictionariesManager::model() const
{
    return m_model;
}


