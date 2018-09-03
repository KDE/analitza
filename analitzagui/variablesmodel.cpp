/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "variablesmodel.h"
#include <analitza/variables.h>
#include <analitza/expression.h>
#include <analitza/value.h>
#include <analitza/analitzautils.h>
#include <QDebug>
#include <QCoreApplication>

using namespace Analitza;

VariablesModel::VariablesModel(const QSharedPointer<Variables>& v, QObject *parent)
    : QAbstractTableModel(parent), m_vars(v), m_editable(true)
{}

VariablesModel::VariablesModel(QObject* parent)
    : QAbstractTableModel(parent), m_vars(nullptr), m_editable(true)
{}

void VariablesModel::setVariables(const QSharedPointer<Variables> &v)
{
    m_vars = v;
}

QVariant VariablesModel::data(const QModelIndex & index, int role) const
{
    QVariant ret;
    if(role==Qt::DisplayRole) {
        switch(index.column()) {
            case 0:
                ret=m_vars->keys()[index.row()];
                break;
            case 1:
                return data(index.sibling(index.row(), 0), Qt::ToolTipRole);
        }
    } else if(role==Qt::ToolTipRole && index.column()==0) {
        QString key = m_vars->keys()[index.row()];
        if(m_vars->value(key)->type()==Object::value) {
            Cn* v=static_cast<Cn*>(m_vars->value(key));
            ret=v->value();
        } else
            ret=m_vars->value(key)->toString();
    } else if(role==Qt::WhatsThisRole && index.column()==0) {
        ret = QStringLiteral("%1 := %2").arg(index.sibling(index.row(), 0).data(Qt::DisplayRole).toString(),
                                             index.sibling(index.row(), 1).data(Qt::DisplayRole).toString());
    }
    return ret;
}

bool VariablesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(role!=Qt::EditRole || !value.isValid())
        return false;
    
    if(index.column()==1) { //Changing values
        QString name=data(index.sibling(index.row(), 0)).toString();
        m_vars->modify(name, AnalitzaUtils::variantToExpression(value));
        emit dataChanged(index, index);
        return true;
    } else if(index.column()==0) {
        QString name=data(index.sibling(index.row(), 0)).toString();
        m_vars->rename(name, value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant VariablesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal) {
        switch(section) {
            case 0:
                ret=QCoreApplication::translate("@title:column", "Name");
                break;
            case 1:
                ret=QCoreApplication::translate("@title:column", "Value");
                break;
        }
    }
    return ret;
}

int VariablesModel::rowCount(const QModelIndex &idx) const
{
    if(!m_vars || idx.isValid())
        return 0;
    else
        return m_vars->count();
}

QFlags< Qt::ItemFlag > VariablesModel::flags(const QModelIndex& index) const
{
    QFlags< Qt::ItemFlag > ret = QAbstractItemModel::flags(index);
    if(index.column()==1 && m_editable)
        ret |= Qt::ItemIsEditable;
    return ret;
}

void VariablesModel::updateInformation()
{
    beginResetModel();
    endResetModel();
}

void VariablesModel::insertVariable(const QString& name, const Expression& value)
{
    beginResetModel();
    m_vars->modify(name, value);
    endResetModel();
}


