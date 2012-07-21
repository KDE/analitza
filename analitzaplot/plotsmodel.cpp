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

#include "plotsmodel.h"

#include <surface.h>
#include <planecurve.h>
#include <spacecurve.h>


#include "analitza/analyzer.h"
#include "analitza/variables.h"
#include "analitza/localize.h"

#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QIcon>
#include <KIcon>
// #include <kcategorizedsortfilterproxymodel.h>

PlotsModel::PlotsModel(QObject* parent, Analitza::Variables *v): QAbstractListModel(parent)
    , m_variables(v), m_itemCanCallModelRemoveItem(true), m_isCheckable(true)
{
}

PlotsModel::~PlotsModel()
{
    qDeleteAll(m_items);
    m_items.clear();
}

Analitza::Variables * PlotsModel::variables() const
{
    return m_variables;
}

void PlotsModel::setVariables(Analitza::Variables* v)
{
    m_variables = v;
    for(int i = 0; i < m_items.size(); ++i)
        m_items[i]->setVariables(v);
}

Qt::ItemFlags PlotsModel::flags(const QModelIndex & index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return 0;
}

Qt::DropActions PlotsModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}

QVariant PlotsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal) {
        switch(section) {
        case 0:
            ret=i18nc("@title:column", "Plot");
            break;
        }
    }
    return ret;
}

QVariant PlotsModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid() || index.row()>=m_items.count())
        return QVariant();

    int var=index.row();

    PlotItem *tmpcurve = m_items.at(var);
    
    switch(role)
    {
    case Qt::DisplayRole:
        return tmpcurve->expression().toString();
    case Qt::DecorationRole:
    {
//         QPixmap ico(32, 32);
//         ico.fill(tmpcurve->color());
//         return  QIcon(ico);
        return KIcon(tmpcurve->iconName());
    }
    case Qt::ToolTipRole:
        return tmpcurve->name();
    case Qt::StatusTipRole:
        return tmpcurve->typeName();
        

    }

    if (role == Qt::CheckStateRole && m_isCheckable)
        return tmpcurve->isVisible()?Qt::Checked:Qt::Unchecked;
    
    
    return QVariant();
}

bool PlotsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
// //     if(role==Shown) {
// //         bool isshown=value.toBool();
// //         funclist[idx.row()].setShown(isshown);
// //         
// //         QModelIndex idx1=index(idx.row(), 0), idxEnd=index(idx.row(), columnCount()-1);
// //         emit dataChanged(idx1, idxEnd);
// //     }
//     return false;

    if (index.isValid() && role == Qt::CheckStateRole && m_isCheckable) 
    {
        m_items[index.row()]->setVisible(value.toBool());
        
//         qDebug() << m_items[index.row()]->name() << " is now " << m_items[index.row()]->isVisible();
         emit dataChanged(index, index);
         return true;
     }
     return QAbstractListModel::setData(index,value,role);
}

int PlotsModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return m_items.size();
}

PlaneCurve * PlotsModel::addPlaneCurve(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
    return addItem<PlaneCurve>(functionExpression, name, col);
}

SpaceCurve* PlotsModel::addSpaceCurve(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
    return addItem<SpaceCurve>(functionExpression, name, col);
}

Surface* PlotsModel::addSurface(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
    return addItem<Surface>(functionExpression, name, col);
}

QMap< int,PlaneCurve* > PlotsModel::planeCurves() const
{
    return items<PlaneCurve>();
}

QMap< int, SpaceCurve* > PlotsModel::spaceCurves() const
{
    return items<SpaceCurve>();
}

QMap< int, Surface* > PlotsModel::surfaces() const
{
    return items<Surface>();
}

PlotItem* PlotsModel::item(int curveIndex) const
{
    Q_ASSERT(curveIndex<m_items.count());

    return m_items[curveIndex];
}

void PlotsModel::removeItem(int row)
{
    Q_ASSERT(row<m_items.size());

    beginRemoveRows(QModelIndex(), row, row);

    PlotItem *tmpcurve = m_items[row];

    m_itemCanCallModelRemoveItem = false;

    if (!tmpcurve->m_inDestructorSoDontDeleteMe)
    {
        delete tmpcurve;
        tmpcurve = 0;
    }

    m_itemCanCallModelRemoveItem = true;

    m_items.removeAt(row);

    endRemoveRows();
}


template<typename VisualItemType>
VisualItemType* PlotsModel::addItem(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
    VisualItemType * ret = 0;

    //no se permiten items invalidos
    if (VisualItemType::canDraw(functionExpression))
    {
        beginInsertRows (QModelIndex(), m_items.count(), m_items.count());

        ret = new VisualItemType(functionExpression, name, col);
        ret->setModel(this);
        m_items.append(ret);

        endInsertRows();

        return ret;
    }

    return ret;
}

template<typename VisualItemType>
QMap< int, VisualItemType* > PlotsModel::items() const
{
    QMap< int, VisualItemType* > ret;

    //TODO create a TYPE system for speed
    for (int i = 0; i < m_items.size(); ++i)
    {
        VisualItemType * ci = dynamic_cast<VisualItemType *>(m_items[i]);
        if (ci)
            ret[i] = ci;
    }

    return ret;
}


//
// bool FunctionGraphsModel::addItem(const Analitza::Expression& functionExpression,int spaceDimension,const QString& name, const QColor& col, QStringList &errors)
// {
//     //no se permiten items invalidos
//     if (FunctionGraph::canDraw(functionExpression,spaceDimension, errors))
//     {
//         beginInsertRows (QModelIndex(), items.count(), items.count());
//
//         //TODO
//         items.append(new FunctionGraph(functionExpression,spaceDimension, /*variablesModule, */ name, col));
//
//        this->
//
//         endInsertRows();
//
//         return true;
//     }
//
//     return false;
// }
//NOTE en hijos
/*
bool FunctionGraphsModel::setItemExpression(int curveIndex, const Analitza::Expression& functionExpression,int spaceDimension)
{
    Q_ASSERT(curveIndex<items.count());


    if (FunctionGraph::canDraw(functionExpression,spaceDimension))
    {
        if (items[curveIndex]->reset(functionExpression,spaceDimension))
        {

            emit dataChanged(index(curveIndex), index(curveIndex));

            return true;

        }
    }

    return false;
}*/

///


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
