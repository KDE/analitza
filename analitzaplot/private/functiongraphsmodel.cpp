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



#include "functiongraphsmodel.h"
#include "functiongraph.h"
#include <surface.h>
#include <planecurve.h>


#include "analitza/analyzer.h"
#include "analitza/variables.h"
#include "analitza/localize.h"

#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QIcon>

VisualItemsModel::VisualItemsModel(QObject* parent): QAbstractListModel(parent)
, m_variables(0), m_itemCanCallModelRemoveItem(true)
{

}


VisualItemsModel::VisualItemsModel(Analitza::Variables *v, QObject * parent)
    : QAbstractListModel(parent), m_variables(v), m_itemCanCallModelRemoveItem(true)
{
//     Q_ASSERT(v);

//     variablesModule = v;
}

VisualItemsModel::~VisualItemsModel()
{
    qDeleteAll(m_items);
    m_items.clear();
}

void VisualItemsModel::setVariables(Analitza::Variables* v)
{
    m_variables = v;
    for(int i = 0; i < m_items.size(); ++i)
        m_items[i]->setVariables(v);
}


int VisualItemsModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 2;
}

Qt::ItemFlags VisualItemsModel::flags(const QModelIndex & index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsTristate;
    else
        return 0;
}

bool VisualItemsModel::hasChildren(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return false;
}

QVariant VisualItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;

    if(role==Qt::DisplayRole && orientation==Qt::Horizontal)
    {
        switch(section) {
        case 0:
            ret=i18nc("@title:column", "Name");
            break;
        case 1:
            ret=i18nc("@title:column", "Function");
            break;
        }
    }
    return ret;
}


Qt::DropActions VisualItemsModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}

QVariant VisualItemsModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid() || index.row()>=m_items.count())
        return QVariant();

    int var=index.row();

    VisualItem *tmpcurve = m_items.at(var);

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0:
            return tmpcurve->name();
            break;
        case 1:
            return tmpcurve->expression().toString();
            break;
        }
        break;
    case Qt::DecorationRole:
        if(index.column()==0) {
            QPixmap ico(15, 15);
            ico.fill(tmpcurve->color());
            return  QIcon(ico);
        } else {
            return QIcon::fromTheme(tmpcurve->iconName());
        }
        break;
    }

    return QVariant();
}

int VisualItemsModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return m_items.size();
}

PlaneCurve * VisualItemsModel::addPlaneCurve(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
    PlaneCurve * ret = 0;
    
    //no se permiten items invalidos
    if (PlaneCurve::canDraw(functionExpression))
    {
        beginInsertRows (QModelIndex(), m_items.count(), m_items.count());

        ret = new PlaneCurve(functionExpression, name, col);
        ret->setModel(this);
        m_items.append(ret);
        
        endInsertRows();
        
        return ret;
    }
    
    return ret;
}

Surface* VisualItemsModel::addSurface(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
    Surface * ret = 0;
    
    //no se permiten items invalidos
    if (Surface::canDraw(functionExpression))
    {
        beginInsertRows (QModelIndex(), m_items.count(), m_items.count());

        ret = new Surface(functionExpression, name, col);
        ret->setModel(this);
        m_items.append(ret);
        
        endInsertRows();
        
        return ret;
    }
    
    return ret;
}

QMap< int,PlaneCurve* > VisualItemsModel::planeCurves() const
{
    QMap< int, PlaneCurve* > ret;
    
    //TODO create a TYPE system for speed
    for (int i = 0; i < m_items.size(); ++i)
    {
        PlaneCurve * ci = dynamic_cast<PlaneCurve *>(m_items[i]);
        if (ci)
            ret[i] = ci;
    }
    
    return ret;
}

QMap< int, Surface* > VisualItemsModel::surfaces() const
{
    QMap< int, Surface* > ret;
    
    //TODO create a TYPE system for speed
    for (int i = 0; i < m_items.size(); ++i)
    {
        Surface * ci = dynamic_cast<Surface *>(m_items[i]);
        if (ci)
            ret[i] = ci;
    }
    
    return ret;
}


VisualItem* VisualItemsModel::item(int curveIndex) const
{
    Q_ASSERT(curveIndex<m_items.count());

    return m_items[curveIndex];
}

void VisualItemsModel::removeItem(int row)
{
    Q_ASSERT(row<m_items.size());

     beginRemoveRows(QModelIndex(), row, row);
    
    VisualItem *tmpcurve = m_items[row];
    
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
