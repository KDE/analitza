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

#ifndef FUNCTIONGRAPHMODEL_H__2d__
#define FUNCTIONGRAPHMODEL_H__2d__

#include <QAbstractListModel>
#include <QColor>
#include <QSortFilterProxyModel>

#include "analitzaplot/analitzaplotexport.h"

class Surface;
class SpaceCurve;
class PlaneCurve;

class PlotItem;

namespace Analitza
{
class Variables;
class Expression;
}

class ANALITZAPLOT_EXPORT PlotsModel : public QAbstractListModel
{
friend class PlotItem;    
    
Q_OBJECT
//     if(item->type()==CurveType)
public:
    PlotsModel(QObject * parent = 0, Analitza::Variables *v = 0);
    virtual ~PlotsModel();
    
    Analitza::Variables * variables() const;
    void setVariables(Analitza::Variables *v); // set variables for all this items this not emit setdata signal
    
    Qt::ItemFlags flags(const QModelIndex & index) const;
    Qt::DropActions supportedDropActions() const;

    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    //si es tru entonces los items pueden ocultar por medio de checks en la vista
    bool isCheckable () const { return m_isCheckable; }
    void setCheckable ( bool b) { m_isCheckable = b; }

    PlaneCurve * addPlaneCurve(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));
    SpaceCurve * addSpaceCurve(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));
    Surface * addSurface(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));

    //index-item
    QMap<int, PlaneCurve *> planeCurves() const;
    QMap<int, SpaceCurve *> spaceCurves() const;
    QMap<int, Surface *> surfaces() const;

    PlotItem * item(int curveIndex) const;
    void removeItem(int curveIndex);

private:
    //WARNING no poner nada encima de los templates causan un bug al momento de detruir #qtbug
    template<typename VisualItemType>
    VisualItemType * addItem(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));

    template<typename VisualItemType>
    QMap<int, VisualItemType *> items() const;

    Analitza::Variables *m_variables;
    QList<PlotItem*> m_items;
    bool m_itemCanCallModelRemoveItem; // just a lock para evitar que el item llame recursivamente a removeItem
    bool m_isCheckable;
};


class ANALITZAPLOT_EXPORT PlotsFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        PlotsFilterProxyModel(QObject *parent = 0);
        virtual ~PlotsFilterProxyModel();

        int filterSpaceDimension() const { return m_dimension; }
        void setFilterSpaceDimension(int dimension);

        //functiontype ... if the item is a functiongraph TODO

    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    private:
        int m_dimension; // 2 => 2D, 3 => 3D, -1 => ALL
};



#endif // FUNCTIONGRAPHMODEL_H
