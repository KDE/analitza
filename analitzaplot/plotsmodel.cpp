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
#include <KLocalizedString>

#include <QDebug>
#include <KIcon>
#include <analitzaplot/functiongraph.h>

Q_DECLARE_METATYPE(PlotItem*);

PlotsModel::PlotsModel(QObject* parent)
    : QAbstractListModel(parent)
{}

PlotsModel::~PlotsModel()
{
    clear();
}

void PlotsModel::clear()
{
	qDeleteAll(m_items);
    m_items.clear();
}

Qt::ItemFlags PlotsModel::flags(const QModelIndex & index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
    else
        return 0;
}

QVariant PlotsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal) {
        switch(section) 
        {
            case 0: return i18nc("@title:column", "Name");
            case 1: return i18nc("@title:column", "Plot");
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
                case 1: return tmpcurve->expression().toString();
            }
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
            return qVariantFromValue<PlotItem*>(tmpcurve);
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
                            delete m_items[index.row()];
                            m_items[index.row()] = plot.create(it->color(), it->name());
                        }
                        
                        emit dataChanged(index, index);
                        
                        return true;
                    }
                    return false;
                }
            }
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

    endInsertRows();
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
	//TODO: figure out a better way
	return "f"+QString::number(qrand()*rowCount());
}
