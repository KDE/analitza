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

#include "plotsfilterproxymodel.h"

#include "plotitem.h"
#include "plotsmodel.h"


PlotsFilterProxyModel::PlotsFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_dimension(-1)
{
     setDynamicSortFilter(true);
}

PlotsFilterProxyModel::~PlotsFilterProxyModel()
{

}

void PlotsFilterProxyModel::setFilterSpaceDimension(int dimension)
{
    Q_ASSERT(dimension == 2 || dimension == 3);
    
    m_dimension = dimension;
    invalidateFilter();
}

bool PlotsFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_ASSERT(sourceModel());
    
    PlotItem *item = qobject_cast<PlotsModel *>(sourceModel())->item(sourceRow);
    
//     qDebug() << item->spaceDimension() << item->typeName() << item->name() <<  (item->spaceDimension() != m_dimension);
    
    if (item->spaceDimension() != m_dimension)
        return false;
    
    return true;
/*
    return item->name().contains(filterRegExp()) ||
        item->expression().toString().contains(filterRegExp());*/
}
