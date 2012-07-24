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

#ifndef PROXY_FUNCTIONGRAPHMODEL_H__2d__
#define PROXY_FUNCTIONGRAPHMODEL_H__2d__


#include <QSortFilterProxyModel>

#include "analitzaplotexport.h"

#include "private/utils/mathutils.h"

//si no se establece ningun filter2d entonces no hay filtro y muetra todos los items sin importar que dimension
//estoy es util para un vista (itemview) que quiera mostrar todo el modelo por ejemplo 
class ANALITZAPLOT_EXPORT PlotsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:

        PlotsProxyModel(QObject *parent = 0);
        virtual ~PlotsProxyModel();

        int filterSpaceDimension() const { return m_dimension; }
        void setFilterSpaceDimension(Dimension dimension);

        //functiontype ... if the item is a functiongraph TODO

    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

    private:
        Dimension m_dimension; // 2 => 2D, 3 => 3D, -1 => ALL
};

#endif 
