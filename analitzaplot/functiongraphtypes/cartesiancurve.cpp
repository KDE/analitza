/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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


#include "functiongraph.h"
#include "functiongraphfactory.h"

#include <QRectF>
#include "analitza/value.h"
#include "analitza/variable.h"


class ANALITZAPLOT_EXPORT CartesianCurveY : public AbstractPlaneCurve
{
public:
    CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables);
	~CartesianCurveY()
	{
	}


    //MappingGraph
    const QString typeName() const { return TypeName(); }
    QString iconName() const { return IconName(); }
    QStringList examples() const { return Examples(); }
    int spaceDimension() const { return SpaceDimension(); }
    CoordinateSystem coordinateSystem() const { return CoordSystem(); }
    QStringList errors() const { return m_errors; }
    bool isCorrect() const { return false; }
    AbstractMappingGraph * copy();

    //FunctionGraph
    QStringList arguments() const { return Arguments(); }
    void update(const QList<RealInterval> viewport);

    //Curve
    double arcLength() const;
    bool isClosed() const { return false; }
    double area() const;
    QPair<bool, double> isParallelTo(const Curve &othercurve);

    //Own
    QPair<QVector2D, QString> calc(const QPointF &mousepos);
    QLineF derivative(const QPointF &mousepos) const;

    bool isImplicit() const { return IsImplicit(); }
    bool isParametric() const { return IsParametric(); }

    //factory registration
    static QString TypeName() { return QString("CartesianCurveY"); }
    static Analitza::ExpressionType ExpressionType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value));
    }
    static int SpaceDimension() { return 2; }
    static CoordinateSystem CoordSystem() { return Cartesian; }
    static QStringList Arguments() { return QStringList() << "x"; }
    static QString IconName() { return QString(); }
    static QStringList Examples() { return QStringList() << "x*x+x" << "x-2*x*x"; }
    static bool IsImplicit() { return false; }
    static bool IsParametric() { return false; }

private:
QStringList m_errors;
};

CartesianCurveY::CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables)
:AbstractPlaneCurve(functionExpression, variables)
{
}


AbstractMappingGraph * CartesianCurveY::copy()
{
	return 0;
}


//FunctionGraph

void CartesianCurveY::update(const QList<RealInterval> viewport)
{
    
}

//Curve


double CartesianCurveY::arcLength() const
{
    return 0;
}

double CartesianCurveY::area() const
{
    return 0;
}

QPair<bool, double> CartesianCurveY::isParallelTo(const Curve &othercurve)
{
    return qMakePair(false, 0.0);
}


//Own
QPair<QVector2D, QString> CartesianCurveY::calc(const QPointF &mousepos)
{
    return qMakePair(QVector2D(), QString());
}

QLineF CartesianCurveY::derivative(const QPointF &mousepos) const
{
    return QLineF();
}






REGISTER_PLANECURVE(CartesianCurveY)

