/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef ANALITZAPLOT_FUNCTIONSMODEL_H
#define ANALITZAPLOT_FUNCTIONSMODEL_H

#include <QAbstractListModel>

#include "function.h"

class ANALITZAPLOT_EXPORT FunctionGraphModel : public QAbstractListModel
{
Q_OBJECT
    
public:
    enum PlaneCurveDataRole 
    {
        //roles that will show in a view, also works for editing job except iconname
        ExpressionRole = Qt::UserRole, //Variant->QString
        NameRole, //Variant->QString
        ColorRole, //Variant->QColor
        IconNameRole, //Variant->QString

        //roles for editing job
        ArgumentsRole, //Variant->QList<QVariant> ... List: ... QString(argname), double min, double max ...
        DrawingPrecisionRole, //Variant->int
        VisibleRole, //Variant->bool

        //roles for deliver extra data: examples, etc ... read-only roles / except plotstyle
        ExamplesRoles, //Variant->QStringList
        SpaceDimensionRole, //Variant->int
        CoordinateSystemRole, //Variant->int
        ErrorsRole, //Variant->QStringList
    };

    
    FunctionGraphModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~FunctionGraphModel();
    
    //QAbstractItemModel
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;

protected:
    Analitza::Variables *variablesModule;
};

class ANALITZAPLOT_EXPORT PlaneCurveModel : public FunctionGraphModel
{
Q_OBJECT

public:
    
    PlaneCurveModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~PlaneCurveModel();
    
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    void addItem(const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    void removeItem(int row);
    const PlaneCurve * item(int row) const;
    
    //planecurve methods
    void update(int row, const QRect& viewport); //emit setdata signal
    QPair<QPointF, QString> calcItem(int row, const QPointF &mousepos);
    QLineF derivativeItem(int row, const QPointF &mousepos) const;
    QVariantMap additionalPropertiesForItem(int row);
    QVector<QVariantMap> additionalInformationForItem(int row, const QVector<MappingGraph*> &others);

private:
    QVector<PlaneCurve*> m_items;
};

#endif // ANALITZAPLOT_FUNCTIONSMODEL_H
