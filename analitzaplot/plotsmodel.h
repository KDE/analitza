/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef PLOTSMODEL_H
#define PLOTSMODEL_H

#include <QAbstractListModel>

#include "analitzaplot/analitzaplotexport.h"

class PlotItem;

class ANALITZAPLOT_EXPORT PlotsModel : public QAbstractListModel
{
Q_OBJECT
public:
    enum Roles {
        DimensionRole = Qt::UserRole+1,
        PlotRole
    };
    
    PlotsModel(QObject * parent = 0);
    virtual ~PlotsModel();

    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole); // only title and check=visible
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

    void addPlot(PlotItem *it);
    void emitChanged(PlotItem* it);

private:
    QList<PlotItem*> m_items;
};

#endif // FUNCTIONGRAPHMODEL_H
