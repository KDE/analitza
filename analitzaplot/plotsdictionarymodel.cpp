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
#include <QDomDocument>
#include "analitzaplot/dictionaryitem.h"
PlotsDictionariesModel::PlotsDictionariesModel(QObject* parent)
    : PlotsModel(parent)
{
}

PlotsDictionariesModel::~PlotsDictionariesModel()
{
}

QVariant PlotsDictionariesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal) 
    {
        switch(section) 
        {
            case 2: return i18nc("@title:column", "Collection");
        }
    }
        
    return PlotsModel::headerData(section, orientation, role);
}

QVariant PlotsDictionariesModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || index.row()>=rowCount())
        return QVariant();

    int var=index.row();

    PlotItem *tmpcurve = plot(var);
    
    switch(role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch(index.column()) 
            {
                case 2:
                {
                    if (tmpcurve->space())
                    {
                        return tmpcurve->space()->title();
                    }
                }
                    break;
            }
        case Qt::DecorationRole:
        {
            if(index.column()==2)
                return QVariant();
        }
        break;
        case Qt::CheckStateRole:
            return QVariant();
    }

    return PlotsModel::data(index, role);
}

int PlotsDictionariesModel::columnCount(const QModelIndex& parent) const
{
    return 3;
}



