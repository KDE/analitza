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

class ANALITZAPLOT_EXPORT MappingGraph
{
public:
    explicit MappingGraph(const QString &name, const QColor& col);
    virtual ~MappingGraph();
    
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
    bool isVisible() const { return m_graphVisible; }
    void setVisible(bool f) { m_graphVisible = f; }

    //2D//curvature, length of arc, etc curvature
    //parametricform ... implicit->parametric etc
    //Own
    //TODO ... (very :p) hard Numerical analysis was on planecurve
//     virtual double arcLength() const = 0;
//     virtual bool isClosed() const = 0;
//     virtual double area() const = 0; //only if is closed
    //all avobe is replaced by additionalInformation a more inteligent approach to expose
    // information from the curve/surface/item ... 
    // the developer can put here what kind of aditional info will be calculated/showed
//     virtual QVariantMap additionalProperties() = 0;

    // additionalInformation = properties relative to others items such as:
    //curvas paralelas, intersec entre curvas/surfacesetc, mismos eigenvals, ...
    //exaples
    // was virtual QPair<bool /*yes or not*/, double /*offset*/> isParallelTo(const Curve &othercurve) = 0; // offset, either positive or negative, in the direction of the curve's normal
//     virtual QVariantMap additionalInformation(const QVector<MappingGraph*> &others) = 0; //key|feature-> vmap[graphid]->variant(data/result)

    virtual QStringList errors() const = 0;
    virtual bool isCorrect() const = 0;

protected:
    MappingGraph() {}
    MappingGraph(const MappingGraph &other) {}

private:
    QString m_id; // from a QUuid

    //gui
    QString m_name;
    QColor m_color;

    //graphDescription    
    bool m_graphVisible;
};

///

class ANALITZAPLOT_EXPORT FunctionGraph : public MappingGraph
{
public:
    FunctionGraph(const QString &name, const QColor& col) : MappingGraph(name, col) {}

    virtual QPair<double, double> intervalValues(const QString &argname) = 0;
    virtual void setIntervalValues(const QString &argname, const QPair<double, double> &interval) = 0;
    
    virtual QPair<Analitza::Expression, Analitza::Expression> intervalExpressionValues(const QString &argname) const = 0;
    virtual void setIntervalExpressionValues(const QString &argname, const QPair<Analitza::Expression, Analitza::Expression> &interval) = 0;

    virtual QStringList arguments() const = 0;
    
protected:
    FunctionGraph() {}
    FunctionGraph(const FunctionGraph &other) {}
};

///

class ANALITZAPLOT_EXPORT Curve : public FunctionGraph
{
public:
    Curve(const QString &name, const QColor& col) : FunctionGraph(name, col) {}
    virtual ~Curve() {}

    //own
    virtual QVector<int> jumps() const = 0;

protected:
    Curve() {}
    Curve(const Curve &other) {}
};

///

class ANALITZAPLOT_EXPORT PlaneCurve : public Curve 
{
public:
    explicit PlaneCurve(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~PlaneCurve();

    static bool canDraw(const Analitza::Expression &functionExpression);
    //with stringlist is used in model for add a item ... de otra manera se crearia una instancia solo para verrificar que sea valido
    static bool canDraw(const Analitza::Expression &functionExpression, QStringList &errors);

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
//     QVariantMap additionalProperties();
//     QVariantMap additionalInformation(const QVector< MappingGraph* >& others);
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    QPair<double, double> intervalValues(const QString &argname);
    void setIntervalValues(const QString &argname, const QPair<double, double> &interval);
    
    QPair<Analitza::Expression, Analitza::Expression> intervalExpressionValues(const QString &argname) const;
    void setIntervalExpressionValues(const QString &argname, const QPair<Analitza::Expression, Analitza::Expression> &interval);

    //RealInterval ni endpoint deben de ser publcos ... Solo deben usarse internamente
//     RealInterval argumentInterval(const QString &argname) const;
//     void setArgumentInverval(const QString &argname, const RealInterval &interval);
    
    QStringList arguments() const;

    //Curve
    QVector<int> jumps() const;

    //Own
    const QVector<QPointF> & points() const;
    void update(const QRect& viewport);
    QPair<QPointF, QString> calc(const QPointF &mousepos);
    QLineF derivative(const QPointF &mousepos) const;

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

/// sacar khipu  mockups 
//fines de mes khipu ui!!


///

class ANALITZAPLOT_EXPORT Surface : public FunctionGraph 
{
public:
    explicit Surface(const Analitza::Expression &functionExpression, CoordinateSystem coordsys, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~Surface();

    static bool canDraw(const Analitza::Expression &functionExpression, CoordinateSystem coordsys);
    //with stringlist is used in model for add a item ... de otra manera se crearia una instancia solo para verrificar que sea valido
    static bool canDraw(const Analitza::Expression &functionExpression, CoordinateSystem coordsys, QStringList &errors);
    bool reset(const Analitza::Expression &functionExpression, CoordinateSystem coordsys);
    
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
    QPair<double, double> intervalValues(const QString &argname);
    void setIntervalValues(const QString &argname, const QPair<double, double> &interval);
    
    QPair<Analitza::Expression, Analitza::Expression> intervalExpressionValues(const QString &argname) const;
    void setIntervalExpressionValues(const QString &argname, const QPair<Analitza::Expression, Analitza::Expression> &interval);

    QStringList arguments() const;

    //Own
    const QVector<int> & indexes() const;
    const QVector<QVector3D> & points() const;
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
