/*************************************************************************************
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

#ifndef ANALITZAPLOT_FUNCTION_H
#define ANALITZAPLOT_FUNCTION_H

#include "analitza/expression.h"
#include "functionutils.h"

#include <QMap>
#include <QColor>

namespace Analitza
{
class Variables;
}

class AbstractCurve;

// this class is a sort of a functionitem ...
// it can't use for plot curves/surfaces (is a just a data holder)
class ANALITZAPLOT_EXPORT MappingGraph
{
public:
    MappingGraph();
    MappingGraph(const MappingGraph &f);
    MappingGraph(const QString &name, const QColor& col);
    virtual ~MappingGraph();

    const QString id() const { return m_id; }
    virtual const QString type() const = 0; // curve, linear op, isosurface etc localized
    virtual const Analitza::Expression & expression() const = 0; // why pure abstract: couse graphpres go to functionimpl

    QString name() const { return m_name; }
    void setName(const QString &newName) { m_name = newName; }
    QString iconName() const { return m_iconName; }
    QColor color() const { return m_color; }
    void setColor(const QColor& newColor) { m_color = newColor; }
    QStringList examples() const { return m_examples; }

    virtual int dimension() const = 0;
    CoordinateSystem coordinateSystem() { return m_coordinateSystem; }
    virtual DrawingPrecision drawingPrecision()  = 0;
    virtual void setDrawingPrecision(DrawingPrecision precs) = 0; // why pure abstract: couse graphpres go to functionimpl
    PlotStyle plotStyle() { return m_plotStyle; }
    void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; }
    bool isVisible() const { return m_graphVisible; }
    void setVisible(bool f) { m_graphVisible = f; }

    virtual QStringList errors() const = 0;
    virtual bool isCorrect() const = 0;

    bool operator == (const MappingGraph& f) const { return m_id == f.m_id; }

protected:
    void setId(const QString id) { m_id = id; }

    //gui
    void setIconName(const QString &iconName) { m_iconName = iconName; }
    void setExamples(const QStringList &examples) { m_examples = examples; }

    //graphDescription
    void setDimension(int dimension) { m_dimension = dimension; }
    void setCoordinateSystem(CoordinateSystem coordsys) { m_coordinateSystem = coordsys; }

private: //TODO pimpl idiom here?
    QString m_id; // from a QUuid

    //gui
    QString m_name;
    QString m_iconName;
    QColor m_color;
    QStringList m_examples;

    //graphDescription    
    int m_dimension;
    CoordinateSystem m_coordinateSystem;
    PlotStyle m_plotStyle;
    bool m_graphVisible;
};

//TODO balls
//TODO Planar graphs
//TODO operator or transf

class ANALITZAPLOT_EXPORT FunctionGraph : public MappingGraph
{
public:
    FunctionGraph() : MappingGraph() {}
    FunctionGraph(const FunctionGraph &f) : MappingGraph(f) {} //interval abstrac y ademas deberia ir la data en el abscurve
    FunctionGraph(const QString &name, const QColor& col) : MappingGraph(name, col) {}
    
    virtual RealInterval argumentInterval(const QString &argname) const = 0;
    virtual void setArgumentInverval(const QString &argname, const RealInterval &interval) = 0;
    virtual QStringList arguments() const = 0;
    
    virtual void update(const QList<RealInterval> viewport) = 0;

private:
};

template<typename VectorType>
class ANALITZAPLOT_EXPORT Curve : public FunctionGraph
{
public:
    Curve();
    Curve(const Curve &f);
    Curve(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~Curve();

    //MappingGraph
    const QString type() const { return QString("if names"); } //TODO 
    const Analitza::Expression &expression() const;
    int dimension() const { return VectorType().length(); }
    DrawingPrecision drawingPrecision();
    void setDrawingPrecision(DrawingPrecision precision); 
    QStringList errors() const;
    bool isCorrect() const;
    
    //FunctionGraph
    QStringList arguments() const;
    void update(const QList<RealInterval> viewport);
    QPair<VectorType, QString> calc(const VectorType &dp);

    //Own
    bool isImplicit() const;
    const QVector<VectorType> & points() const;
    QList<int> jumps() const;
    LineSegment<VectorType> derivative(const VectorType &p) const;

    Curve<VectorType> operator = (const Curve<VectorType> &curve); // copy all:members, "id" and funcimpl instance

private:
    Analitza::Expression m_expression;

    DrawingPrecision m_drawingPrecision;

    AbstractCurve *m_curve;

    QStringList m_errors;
};

#endif // ANALITZAPLOT_FUNCTION_H
