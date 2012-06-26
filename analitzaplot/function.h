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
    virtual const QString typeName() const = 0;
    virtual const Analitza::Expression & expression() const = 0;

    QString name() const { return m_name; }
    void setName(const QString &newName) { m_name = newName; }
    virtual QString iconName() const = 0;
    QColor color() const { return m_color; }
    void setColor(const QColor& newColor) { m_color = newColor; }
    virtual QStringList examples() const = 0;

    virtual int spaceDimension() const = 0; // dim of the space where the item can be drawn ... IS NOT the variety dimension
    virtual CoordinateSystem coordinateSystem() const = 0;
    bool isVisible() const { return m_graphVisible; }
    void setVisible(bool f) { m_graphVisible = f; }

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

    //if evaluate true then result of expressiones will be strings of the value
    //if evaluate is false then the expressions will not evaluate
    virtual QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const = 0;
    virtual void setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max) = 0;

    //2 convenience methods to work with doubles instead of Expression->Cn->value ... see above
    virtual QPair<double, double> interval(const QString &argname) const = 0;
    virtual void setInterval(const QString &argname, double min, double max) = 0;

    virtual QStringList parameters() const = 0;
    
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
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    void setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
    
    QPair<double, double> interval(const QString &argname) const;
    void setInterval(const QString &argname, double min, double max);
    
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
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    void setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);

    QPair<double, double> interval(const QString &argname) const;
    void setInterval(const QString &argname, double min, double max);
    
    QStringList parameters() const;

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
