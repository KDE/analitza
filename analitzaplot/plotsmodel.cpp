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

#include "plotsfactory.h"
#include "plotitem.h"

#include "analitza/analitzautils.h"
#include <analitza/variables.h>
#include <analitzaplot/functiongraph.h>

#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QCoreApplication>
#include <QRandomGenerator>

using namespace Analitza;

PlotsModel::PlotsModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_resolution(500)
    , m_namingCount(0)
{}

PlotsModel::~PlotsModel()
{
    clear();
}

QHash<int, QByteArray> PlotsModel::roleNames() const
{
    auto ret = QAbstractListModel::roleNames();
    ret.insert(DescriptionRole, "description");
    return ret;
}

void PlotsModel::clear()
{
    if (!m_items.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.count()-1);
        qDeleteAll(m_items);
        m_items.clear();
        endRemoveRows();
    }
}

Qt::ItemFlags PlotsModel::flags(const QModelIndex & index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
    else
        return Qt::NoItemFlags;
}

QVariant PlotsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal) {
        switch(section) 
        {
            case 0: return QCoreApplication::translate("@title:column", "Name");
            case 1: return QCoreApplication::translate("@title:column", "Plot");
        }
    }
    
    return QAbstractListModel::headerData(section, orientation, role);
}

QVariant PlotsModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid() || index.row()>=m_items.count())
        return QVariant();

    PlotItem *tmpcurve = m_items.at(index.row());
    
    switch(role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch(index.column()) 
            {
                case 0: return tmpcurve->name();
                case 1: return tmpcurve->display();
            }
            break;
        case Qt::DecorationRole:
            if(index.column()==0)
            {
                QPixmap ico(16, 16);
                ico.fill(tmpcurve->color());
                return QIcon(ico);
            } 
            else 
                return QIcon::fromTheme(tmpcurve->iconName());
        case Qt::ToolTipRole:
            return tmpcurve->name();
        case Qt::StatusTipRole:
            return tmpcurve->typeName();
        case Qt::CheckStateRole:
            if(index.column()==0) 
                return tmpcurve->isVisible() ? Qt::Checked : Qt::Unchecked;
            break;
        case DimensionRole:
            return int(tmpcurve->spaceDimension());
        case PlotRole:
            return QVariant::fromValue<PlotItem*>(tmpcurve);
        case DescriptionRole:
            return tmpcurve->display();
    }

    
    return QVariant();
}

bool PlotsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    switch(role)
    {
        case Qt::EditRole:
            switch(index.column()) 
            {
                case 0: {
                    //FIXME: actually I think the name should be stored in the model instead of plotItem
                    //if there was another plot with that name, it shouldn't be accepted
                    QString newName = value.toString();
                    if(!newName.isEmpty()) {
                        m_items[index.row()]->setName(newName);
                        emit dataChanged(index, index);
                    }
                    return !newName.isEmpty();
                }
                case 1: {
                    Analitza::Expression valexp = AnalitzaUtils::variantToExpression(value);
                    PlotItem* it = m_items[index.row()];

                    PlotBuilder plot = PlotsFactory::self()->requestPlot(valexp, it->spaceDimension());
                    if (plot.canDraw()) {
                        if (m_items[index.row()]->expression() != valexp) {
                            QColor color=it->color();
                            QString name=it->name();
                            delete m_items[index.row()];
                            m_items[index.row()] = plot.create(color, name);
                        }
                        
                        emit dataChanged(index, index);
                        
                        return true;
                    }
                    return false;
                }
            } //fallthrough
        case Qt::CheckStateRole:
            m_items[index.row()]->setVisible(value.toBool());
            return true;
        case Qt::DecorationRole:
            m_items[index.row()]->setColor(value.value<QColor>());
            return true;
    }

    return false;
}

int PlotsModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return m_items.size();
}

int PlotsModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}


bool PlotsModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_ASSERT(row<m_items.size());
    if(parent.isValid())
        return false;
    
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for (int i = 0; i < count; ++i) {
        delete m_items.takeAt(row);
    }
    endRemoveRows();
    
    return true;
}

void PlotsModel::addPlot(PlotItem* it)
{
    Q_ASSERT(it);

    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    it->setModel(this);
    m_items.append(it);

    if(FunctionGraph* g=dynamic_cast<FunctionGraph*>(it))
        g->setResolution(m_resolution);
    endInsertRows();

    m_namingCount++;
}

void PlotsModel::updatePlot(int row, PlotItem* it)
{
    it->setModel(this);
    delete m_items[row];
    m_items[row]=it;
    
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx);
}

void PlotsModel::emitChanged(PlotItem* it)
{
    int row = m_items.indexOf(it);
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx);
}

QModelIndex PlotsModel::indexForName(const QString& name)
{
    const int rows = rowCount();
    for(int i=0; i<rows; i++) {
        QModelIndex idx = index(i);
        if(idx.data().toString()==name)
            return idx;
    }
    return QModelIndex();
}

QString PlotsModel::freeId() const
{
    return 'f'+QString::number(m_namingCount);
}

void PlotsModel::setResolution(int res)
{
    m_resolution = res;
    for(int i=0; i<rowCount(); i++) {
        FunctionGraph* g = dynamic_cast<FunctionGraph*>(m_items[i]);
        if(g)
            g->setResolution(res);
    }
}

static QColor randomFunctionColor() { return QColor::fromHsv(QRandomGenerator::global()->bounded(255), 255, 225); }

QStringList PlotsModel::addFunction(const QString& expression, Dimension dim, const QSharedPointer<Analitza::Variables>& vars)
{
    Analitza::Expression e(expression, Analitza::Expression::isMathML(expression));

    QString fname;
    do {
        fname = freeId();
    } while(vars && vars->contains(fname));
    QColor fcolor = randomFunctionColor();

    QStringList err;
    PlotBuilder req = PlotsFactory::self()->requestPlot(e, dim, vars);
    if(req.canDraw()) {
        auto it = req.create(fcolor, fname);

        if(it->isCorrect())
            addPlot(it);
        else {
            err = it->errors();
            delete it;
        }
    }

    return err;
}

bool Analitza::PlotsModel::canAddFunction(const QString& expression, int _dim, const QSharedPointer<Analitza::Variables>& vars)
{
    const Analitza::Dimension dim = static_cast<Analitza::Dimension>(_dim);
    Analitza::Expression e(expression, Analitza::Expression::isMathML(expression));
    PlotBuilder req = PlotsFactory::self()->requestPlot(e, dim, vars);
    return req.canDraw();
}
