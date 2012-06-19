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


#ifndef ANALITZAPLOT_FUNCTIONGRAPH_H
#define ANALITZAPLOT_FUNCTIONGRAPH_H

#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "function.h"

namespace Analitza
{
class Variables;
class Analyzer;
class Object;
class Cn;
class Expression;
}

class ANALITZAPLOT_EXPORT AbstractMappingGraph //strategy pattern for curves and surfaces
{
public:
    explicit AbstractMappingGraph(const Analitza::Expression& e, Analitza::Variables* v);
    AbstractMappingGraph(const AbstractMappingGraph& fi);
    virtual ~AbstractMappingGraph();

    virtual const QString typeName() const = 0;
    const Analitza::Expression &expression() const;
    virtual QString iconName() const = 0;
    virtual QStringList examples() const = 0;

    virtual int spaceDimension() const = 0;
    virtual CoordinateSystem coordinateSystem() const = 0;
    DrawingPrecision drawingPrecision() const { return m_drawingPrecision; }
    void setDrawingPrecision(DrawingPrecision precision) { m_drawingPrecision = precision; }

    virtual QStringList errors() const = 0;
    virtual bool isCorrect() const = 0;
    virtual AbstractMappingGraph * copy() = 0;

protected:
    Analitza::Analyzer analyzer;
    Analitza::Cn* arg(const QString &argname) { return dynamic_cast<Analitza::Cn*>(m_argumentValues[argname]); }

private:
    DrawingPrecision m_drawingPrecision;

    QMap<QString, Analitza::Object*> m_argumentValues;
};

////



///

// plugins must expose this static members

// QString TypeName 
// Analitza::ExpressionType ExpressionType
// int SpaceDimension
// CoordinateSystem CoordinateSystem
// QStringList Arguments
// QString IconName
// QStringList Examples
// bool IsImplicit()
// bool IsParametric()

///

//si tiene 2 vars es implicit si tiene 2 vars y una de ellos es r es polar ... caracterizacion
class ANALITZAPLOT_EXPORT AbstractPlaneCurve : public AbstractMappingGraph //strategy pattern for curves
{
public:
    explicit AbstractPlaneCurve(const Analitza::Expression& e, Analitza::Variables* v);
    AbstractPlaneCurve(const AbstractPlaneCurve& fi);
    virtual ~AbstractPlaneCurve();

    //FunctionGraph
    RealInterval argumentInterval(const QString &argname) const;
    void setArgumentInverval(const QString &argname, const RealInterval &interval);
    QStringList arguments() const;
    virtual void update(const QList<RealInterval> viewport) = 0;

    //Curve
    virtual double arcLength() const = 0;
    virtual bool isClosed() const = 0;
    virtual double area() const = 0;
    virtual QPair<bool, double> isParallelTo(const Curve &othercurve) = 0;
    QList<int> jumps() const;

    //Own
    const QVector<QVector2D> & points() const;
    virtual QPair<QVector2D, QString> calc(const QPointF &mousepos) = 0;
    virtual QLineF derivative(const QPointF &mousepos) const = 0;

    bool isImplicit() const { return m_isImplicit; }
    bool isParametric() const { return m_isParametric; }
    bool isAlgebraic() const;

protected:
    bool addPoint(const QVector2D& p);
    void setJump(int jump);

private:
    QMap< QString, RealInterval > m_argumentIntervals;
    QVector<QVector2D> m_points;
    QList<int> m_jumps;
    bool m_isImplicit;
    bool m_isAlgebraic;
    bool m_isParametric;
};

#endif // ANALITZAPLOT_FUNCTIONGRAPH_H
