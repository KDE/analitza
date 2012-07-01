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

#include "planecurve.h"

#include "private/functiongraphsmodel.h"

class ANALITZAPLOT_EXPORT PlaneCurvesModel : public FunctionGraphsModel
{
Q_OBJECT

public:
    PlaneCurvesModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~PlaneCurvesModel();
    
    bool addCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    bool addCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor& col, QStringList &errors);

    //planecurve getters
    const PlaneCurve * curve(int curveIndex) const; //read only pointer the data CAN NOT be changed (is a good thing :) )... use this instead of roles ... razon: el uso de roles hace que el cliente deba hacer casts largos
    
    //planecurve setters and calculation/evaluation methods  .. don't forget to emit setdata signal' ... ninguno de estos metodos tiene cont al final
    bool setCurve(int curveIndex, const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    void updateCurve(int curveIndex, const QRect& viewport); //emit setdata signal
    QPair<QPointF, QString> curveImage(int curveIndex, const QPointF &mousepos); // image of curve
    QLineF curveTangent(int curveIndex, const QPointF &mousepos); //tangent to curve

};

#endif // ANALITZAPLOT_FUNCTIONSMODEL_H
