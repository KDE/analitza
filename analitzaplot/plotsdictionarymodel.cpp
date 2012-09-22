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

#include "plotsdictionarymodel.h"
#include "dictionariesmodel.h"
#include <analitza/expression.h>
#include <KLocale>
#include <KStandardDirs>
#include <analitzaplot/planecurve.h>
#include <QDomDocument>
#include <QFile>
#include "analitzaplot/dictionaryitem.h"

Q_DECLARE_METATYPE(PlotItem*);

PlotsDictionaryModel::PlotsDictionaryModel(QObject* parent)
    : PlotsModel(parent)
{
    setHeaderData(2, Qt::Horizontal, i18nc("@title:column", "Collection"), Qt::DisplayRole);
    
    createDictionary("Dictionary1", "libanalitza/data/plots/es/basic_curves.plots");
    createDictionary("Conics", "libanalitza/data/plots/es/conics.plots");
    createDictionary("Polar", "libanalitza/data/plots/es/polar.plots");
}

PlotsDictionaryModel::~PlotsDictionaryModel()
{}

QVariant PlotsDictionaryModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || index.row()>=rowCount())
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch(index.column()) 
            {
                case 2: {
                    PlotItem* tmpcurve = index.data(PlotRole).value<PlotItem*>();
                    DictionaryItem* space = tmpcurve->space();
                    if (space)
                        return space->title();
                    break;
                }  
            }
            break;
        case Qt::DecorationRole:
            if(index.column()==2)
                return QVariant();
            break;
        case Qt::CheckStateRole:
            return QVariant();
    }

    return PlotsModel::data(index, role);
}

int PlotsDictionaryModel::columnCount(const QModelIndex& ) const
{
    return 3;
}

void PlotsDictionaryModel::createDictionary(const QString& title, const QString& file)
{
    DictionaryItem* di = new DictionaryItem(DimAll);
    di->setTitle(title);
    m_collections.append(di);
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
                addPlot(plot);
            }
        }
    }
}
