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
#include <analitza/expression.h>
#include <KStandardDirs>
#include <KLocalizedString>
#include <analitzaplot/planecurve.h>
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
            if(index.column()==2)
                return m_plotTitles.value(data(index, PlotRole).value<PlotItem*>());
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
    QString localurl = KStandardDirs::locate("data", file);

    QFile device(localurl);

    if (device.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&device);
        
        QString line;
        while(!stream.atEnd()) {
            line += stream.readLine();
            
            if (Analitza::Expression::isCompleteExpression(line)) {
                Analitza::Expression expression(line);
                
                QStringList errors = PlaneCurve::canDraw(expression);
                
                if (errors.isEmpty()) {
                    PlaneCurve *plot = new PlaneCurve(expression);
                    m_plotTitles.insert(plot, title);
                    addPlot(plot);
                    line.clear();
                } else {
                    qDebug() << "Couldn't add " << line << " because of errors: " << errors.join(", ") << "@" << file;
                    break;
                }
            }
        }
    }
}
