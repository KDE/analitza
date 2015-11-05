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

#include "plottingenums.h"
#include "analitzaplot/analitzaplotexport.h"

namespace Analitza
{
class PlotItem;
class Variables;

/**
 * \class PlotsModel
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Collection of many instances of PlotItem.
 *
 * This class contains all plots that will be rendered by plotters. Also it 
 * allows to show the plots in Qt views suchs QListView or QTreeView.
 */

class ANALITZAPLOT_EXPORT PlotsModel : public QAbstractListModel
{
Q_OBJECT
friend class PlotItem;
public:
    enum Roles {
        DimensionRole = Qt::UserRole+1,
        PlotRole,
        DescriptionRole
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
    QHash<int, QByteArray> roleNames() const override;

    void addPlot(PlotItem *it);
    void updatePlot(int row, PlotItem* it);

    /** @returns an identifier that's not present in the model */
    QString freeId() const;

    QModelIndex indexForName(const QString& name);
    Q_SCRIPTABLE void clear();

    void setResolution(int res);

    /**
     * Helper method to help easily add elements to the model
     */
    QStringList addFunction(const QString& expression, Analitza::Dimension dim, Analitza::Variables* vars);

protected:
    void emitChanged(PlotItem* it);

private:
    QList<PlotItem*> m_items;
    int m_resolution;
    int m_namingCount;
};

}

#endif // PLOTSMODEL_H
