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
#include "functionutils.h"

#include <QMap>
#include <QColor>

namespace Analitza
{
class Variables;
}

class AbstractPlaneCurve;
class AbstractSurface;

//*entity* concept here (not a value concept, so no copy constructor,etc)... defines a hierarchy
//it should use with pointers to pass in mthods
//reference semantics when dealing with objects that will be use polymorphically: again,MappingGraphdefines a hierarchy
// ... if is a pointer then also because of performance reasons
class ANALITZAPLOT_EXPORT MappingGraph
{
public:
    explicit MappingGraph(const QString &name, const QColor& col);
    virtual ~MappingGraph();
    
    
// redefine ... no todo es setexpression ... redefine involucra expresio + coordsys (surf) o optra convinacion
    const QString id() const { return m_id; }
    virtual const QString typeName() const = 0; // curve, linear op, isosurface etc localized
    virtual const Analitza::Expression & expression() const = 0; // why pure abstract: couse graphpres go to functionimpl

    QString name() const { return m_name; }
    void setName(const QString &newName) { m_name = newName; }
    virtual QString iconName() const = 0;
    QColor color() const { return m_color; }
    void setColor(const QColor& newColor) { m_color = newColor; }
    virtual QStringList examples() const = 0;

    virtual int spaceDimension() const = 0; // dim of the space where the item can be drawn ... IS NOT the variety dimension
    virtual CoordinateSystem coordinateSystem() const = 0;
    virtual DrawingPrecision drawingPrecision() const = 0;
    virtual void setDrawingPrecision(DrawingPrecision precs) = 0; // why pure abstract: couse graphpres go to functionimpl
    PlotStyle plotStyle() { return m_plotStyle; }
    void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; }
    bool isVisible() const { return m_graphVisible; }
    void setVisible(bool f) { m_graphVisible = f; }

    virtual QStringList errors() const = 0;
    virtual bool isCorrect() const = 0;

    //no sense, see above : *entity* concept ... use id() == other.id() instead
//     bool operator == (const MappingGraph& f) const { return m_id == f.m_id; }

protected:
    MappingGraph() {}
    MappingGraph(const MappingGraph &other) {}

    //no sense, see above : *entity* concept 
//     void setId(const QString id) { m_id = id; }

private: //TODO pimpl idiom here?


    QString m_id; // from a QUuid

    //gui
    QString m_name;
    QColor m_color;

    //graphDescription    
    PlotStyle m_plotStyle;
    bool m_graphVisible;
};

//TODO balls
//TODO Planar graphs
//TODO operator or transf

class ANALITZAPLOT_EXPORT FunctionGraph : public MappingGraph
{
public:
    FunctionGraph(const QString &name, const QColor& col) : MappingGraph(name, col) {}
    
    virtual RealInterval argumentInterval(const QString &argname) const = 0;
    virtual void setArgumentInverval(const QString &argname, const RealInterval &interval) = 0;
    virtual QStringList arguments() const = 0;
    
protected:
    FunctionGraph() {}
    FunctionGraph(const FunctionGraph &other) {}
};

//TODO class curve surface curve

class ANALITZAPLOT_EXPORT Curve : public FunctionGraph
{
public:
    Curve(const QString &name, const QColor& col) : FunctionGraph(name, col) {}
    virtual ~Curve() {}

    //Own
    //TODO ... (very :p) hard Numerical analysis
    virtual double arcLength() const = 0;
    virtual bool isClosed() const = 0;
    virtual double area() const = 0; //only if is closed
    virtual QPair<bool /*yes or not*/, double /*offset*/> isParallelTo(const Curve &othercurve) = 0; // offset, either positive or negative, in the direction of the curve's normal
    virtual QList<int> jumps() const = 0;

protected:
    Curve() {}
    Curve(const Curve &other) {}
};

//algebraic, parametric curves (//equans of this curves are functiones callet vector valued functions (with one real param) 
//... aka vectorialcurves)), polar curves etc
class ANALITZAPLOT_EXPORT PlaneCurve : public Curve 
{
//2D
    //curvature, length of arc, etc curvature
//parametricform ... implicit->parametric etc
public:
    explicit PlaneCurve(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~PlaneCurve();

    bool canReset(const Analitza::Expression &functionExpression) const;
    bool reset(const Analitza::Expression &functionExpression);
    
    //MappingGraph
    const QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    QStringList examples() const;
    int spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    DrawingPrecision drawingPrecision() const;
    void setDrawingPrecision(DrawingPrecision precision); 
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    RealInterval argumentInterval(const QString &argname) const;
    void setArgumentInverval(const QString &argname, const RealInterval &interval);
    QStringList arguments() const;

    //Curve
    double arcLength() const;
    bool isClosed() const;
    double area() const;
    QPair<bool, double> isParallelTo(const Curve &othercurve);
    QList<int> jumps() const;

    //Own
    const QVector<QVector2D> & points() const;
    void update(const QRect& viewport);
    QPair<QVector2D, QString> calc(const QPointF &mousepos);
    QLineF derivative(const QPointF &mousepos) const;

    bool isImplicit() const;
    bool isParametric() const;
    bool isAlgebraic() const; // implicit plus only polynomails analitza work :)

    //no sense, see above MappingGraph : *entity* concept 
//     PlaneCurve operator = (const PlaneCurve &curve); // copy all:members, "id" and funcimpl instance
    
    
    //TODO gsoc
    ///Region under curve (relative to zxis polar axis ... sistema of referencia = basis = 0 + vectors)
    ///parametricform ... -> expresion
    ///toparamform ->void
    /// can be parameterized ... work for analitza :)

protected:
    PlaneCurve() {}
    PlaneCurve(const PlaneCurve &other) {}

private:
    
    Analitza::Variables *m_varsModule;
    AbstractPlaneCurve *m_planeCurve;

    QStringList m_errors;
};

///

// class ANALITZAPLOT_EXPORT SpaceCurve : public Curve 
// {
//     //3D
// //torsion,   
//     //curvature, length of arc, curvature
//     
// 
// };


///


class ANALITZAPLOT_EXPORT Surface : public FunctionGraph 
{
public:
    explicit Surface(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~Surface();

    bool canReset(const Analitza::Expression &functionExpression) const;
    bool reset(const Analitza::Expression &functionExpression);
    
    //MappingGraph
    const QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    QStringList examples() const;
    int spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    DrawingPrecision drawingPrecision() const;
    void setDrawingPrecision(DrawingPrecision precision); 
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    RealInterval argumentInterval(const QString &argname) const;
    void setArgumentInverval(const QString &argname, const RealInterval &interval);
    QStringList arguments() const;

    //Own
    const QList<int> & indexes() const;
    const QList<QVector3D> & points() const;
    void update(/*frustum*/);

protected:
    Surface() {}
    Surface(const PlaneCurve &other) {}

private:
    Analitza::Variables *m_varsModule;
    AbstractSurface *m_surface;

    QStringList m_errors;
};




#endif // ANALITZAPLOT_FUNCTION_H
