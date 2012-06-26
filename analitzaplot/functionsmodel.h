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
    //clear next iter ... elimino los roles en favor de metodos porque 
    //los roles hacen que se deba hacer un typecast muy largo para cada campo 
    //el uso de un puntero de solo lectura es mas rapido, flexible y directo ... 
    // ademas el codigo es mas elengate que los largos typecast :)
    //see childs::item(row)
    //TODO remove next iter
//     enum PlaneCurveDataRole 
//     {
//         //roles that will show in a view, also works for editing job except iconname
//         ExpressionRole = Qt::UserRole, //Variant->QString
//         NameRole, //Variant->QString
//         ColorRole, //Variant->QColor
//         IconNameRole, //Variant->QString
// 
//         //roles for editing job
//         ArgumentValuesRole, //Variant->QList<QVariant> ... List: ... QString(argname), double min, double max ...
//         ArgumentExpressionRole, 
//         DrawingPrecisionRole, //Variant->int
//         VisibleRole, //Variant->bool
// 
//         //roles for deliver extra data: examples, etc ... read-only roles / except plotstyle
//         ExamplesRoles, //Variant->QStringList
//         SpaceDimensionRole, //Variant->int
//         CoordinateSystemRole, //Variant->int
//         ErrorsRole, //Variant->QStringList
//     };

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

    void addCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    void removeCurve(int curveIndex);

    //planecurve getters
    const PlaneCurve * curve(int curveIndex) const; //read only pointer the data CAN NOT be changed (is a good thing :) )... use this instead of roles ... razon: el uso de roles hace que el cliente deba hacer casts largos
    
    //planecurve setters and calculation/evaluation methods  .. don't forget to emit setdata signal' ... ninguno de estos metodos tiene cont al final
    bool setCurve(int curveIndex, const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
//     QPair<Analitza::Expression, Analitza::Expression> curveParameterInterval(int curveIndex, const QString &argname, bool evaluate) ;
    void setCurveParameterInterval(int curveIndex, const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
//     QPair<double, double> curveParameterInterval(int curveIndex, const QString &argname);
    void setCurveParameterInterval(int curveIndex, const QString &argname, double min, double max);
    void updateCurve(int curveIndex, const QRect& viewport); //emit setdata signal
    QPair<QPointF, QString> curveImage(int curveIndex, const QPointF &mousepos); // image of curve
    QLineF curveTangent(int curveIndex, const QPointF &mousepos); //tangent to curve

    //TODO out of scope: no se desarrollara en este gsoc ... solo queda la idea ... que es buena por cierto :)
//     QVariantMap additionalPropertiesForItem(int row);
//     QVector<QVariantMap> additionalInformationForItem(int row, const QVector<MappingGraph*> &others);

private:
    QList<PlaneCurve*> m_items;
};

#endif // ANALITZAPLOT_FUNCTIONSMODEL_H
