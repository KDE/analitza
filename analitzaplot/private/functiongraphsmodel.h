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

#include "analitzaplot/analitzaplotexport.h"
#include <analitzaplot/mathutils.h>

class Surface;
class SpaceCurve;
class PlaneCurve;

class VisualItem;

namespace Analitza
{
class Variables;
class Expression;
}

class ANALITZAPLOT_EXPORT VisualItemsModel : public QAbstractListModel
{
friend class VisualItem;    
    
Q_OBJECT
    
public:
    VisualItemsModel(QObject * parent = 0);
    VisualItemsModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~VisualItemsModel();
    
    void setVariables(Analitza::Variables *v); // set variables for all this items this not emit setdata signal
    
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    PlaneCurve * addPlaneCurve(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));
    SpaceCurve * addSpaceCurve(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));
    Surface * addSurface(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));

    //index-item
    QMap<int, PlaneCurve *> planeCurves() const;
    QMap<int, SpaceCurve *> spaceCurves() const;
    QMap<int, Surface *> surfaces() const;

    VisualItem * item(int curveIndex) const;
    void removeItem(int curveIndex);

private:
    template<typename VisualItemType>
    VisualItemType * addItem(const Analitza::Expression &functionExpression, const QString &name = QString(), const QColor& col = QColor(Qt::yellow));

    template<typename VisualItemType>
    QMap<int, VisualItemType *> items() const;

    Analitza::Variables *m_variables;
    QList<VisualItem*> m_items;
    bool m_itemCanCallModelRemoveItem; // just a lock para evitar que el item llame recursivamente a removeItem
};

#endif // FUNCTIONGRAPHMODEL_H
