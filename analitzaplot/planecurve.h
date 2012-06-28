/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef ANALITZAPLOT_FUNCTION_H
#define ANALITZAPLOT_FUNCTION_H

#include "analitza/expression.h"
#include "mathutils.h"

#include <QMap>
#include <QColor>

namespace Analitza
{
class Variables;
}

class AbstractPlaneCurve;
class AbstractSurface;

///
#include "private/curve.h"
///

class ANALITZAPLOT_EXPORT PlaneCurve : public Curve 
{
public:
    PlaneCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    PlaneCurve(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~PlaneCurve();

    static bool canDraw(const Analitza::Expression &functionExpression);
    //with stringlist is used in model for add a item ... de otra manera se crearia una instancia solo para verrificar que sea valido
    static bool canDraw(const Analitza::Expression &functionExpression, QStringList &errors);

    bool reset(const Analitza::Expression &functionExpression);
    void setVariables(Analitza::Variables *variables);

    //MappingGraph
    const QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    QStringList examples() const;
    int spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
    
    QPair<double, double> interval(const QString &argname) const;
    bool setInterval(const QString &argname, double min, double max);
    
    QStringList parameters() const;

    //Curve
    QVector<int> jumps() const;

    //Own
    const QVector<QPointF> & points() const;
    void update(const QRect& viewport);
    QPair<QPointF, QString> image(const QPointF &mousepos); // calculate the image of the curve based on the mouse postion 
    QLineF tangent(const QPointF &mousepos);// calculate the tangent to the curve based on the mouse postion 

protected:
    PlaneCurve() {}
    PlaneCurve(const PlaneCurve &other) {}

private:
    Analitza::Variables *m_varsModule;
    AbstractPlaneCurve *m_planeCurve;

    QStringList m_errors;
};


///


///

// class ANALITZAPLOT_EXPORT SpaceCurve : public Curve 
// {
//     //3D
// //torsion,   
//     //curvature, length of arc, curvature
//     
// 
// };

/// sacar khipu  mockups 
//fines de mes khipu ui!!


///

#endif // ANALITZAPLOT_FUNCTION_H
