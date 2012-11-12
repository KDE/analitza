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

namespace Analitza
{
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
    void updatePlot(int row, PlotItem* it);
    
    //WARNING @apol this method is for private communication with plotitem
    // why is in public api? sometime the use of friends are good
    // and this context collention/item is a good case when the use of friend is a good thing
    void emitChanged(PlotItem* it);
    
    //DEPRECATED @apol Why this method? why the client needs to know the freeid?
    //model can contain same names for plots, id are used only for internal tasks
    // like persintence and so on
    /**
      * @returns an identifier that's not present in the model 
      * @warning Deprecated since 4.11.
      */
#ifndef KDE_NO_DEPRECATED
    QString KDE_DEPRECATED freeId() const;
#endif

    //DEPRECATED @apol this kind of queries are done by proxies no by the model
    // so this method is like a hack for this class ... we need to follow the model->proxy
    // architecture by qt
    /**
      * @warning Deprecated since 4.11.
      */
#ifndef KDE_NO_DEPRECATED
    QModelIndex KDE_DEPRECATED indexForName(const QString& name);
#endif
    
    Q_SCRIPTABLE void clear();

    //DEPRECATED @apol Why this method? this class is a model class it doesn't have to do 
    //nothing with graphics/rendering tasks, also each algorithm will resolve the best 
    //resolution value
    /**
      * @warning Deprecated since 4.11.
      */
#ifndef KDE_NO_DEPRECATED
    void KDE_DEPRECATED setResolution(int res);
#endif
    
private:
    QList<PlotItem*> m_items;
    int m_resolution;
};

}

#endif // PLOTSMODEL_H
