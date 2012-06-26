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



#define TYPE_NAME(name) \
const QString typeName() const { return TypeName(); } \
static QString TypeName() { return QString(name); } 

#define EXPRESSION_TYPE(name) \
static Analitza::ExpressionType ExpressionType() { return Analitza::ExpressionType(name); }

#define COORDDINATE_SYSTEM(name) \
CoordinateSystem coordinateSystem() const { return CoordSystem(); } \
static CoordinateSystem CoordSystem() { return name; }

//TODO validaciones strim etc 
#define ARGUMENTS(name) \
QStringList arguments() const { return Arguments(); } \
static QStringList Arguments() { return QString(name).split(","); }

#define ICON_NAME(name) \
QString iconName() const { return IconName(); } \
static QString IconName() { return QString(name); } 

#define EXAMPLES(name) \
QStringList examples() const { return Examples(); } \
static QStringList Examples() { return QString(name).split(","); }


//NOTE
//para construir estos backends/abstract solo re necesitaa que su ctor tenga exp y varmod ... 
//en la funcion y en el metodo factory id es donde se requiere que tenga mas detalle ademas de exp y varsmod
// por ejemolo coordsys en el caso de surf

class ANALITZAPLOT_EXPORT AbstractMappingGraph //strategy pattern
{
public:
    explicit AbstractMappingGraph(const Analitza::Expression& e, Analitza::Variables* v);
    virtual ~AbstractMappingGraph();

    virtual const QString typeName() const = 0;
    const Analitza::Expression &expression() const;
    virtual QString iconName() const = 0;
    virtual QStringList examples() const = 0;

    virtual int spaceDimension() const = 0;
    virtual CoordinateSystem coordinateSystem() const = 0;
    DrawingPrecision drawingPrecision() const { return m_drawingPrecision; }
    void setDrawingPrecision(DrawingPrecision precision) { m_drawingPrecision = precision; }

//     virtual QVariantMap additionalProperties() = 0;
//     virtual QVector<QVariantMap> additionalInformation(const QVector<MappingGraph*> &others) = 0;
    
    QStringList errors() const { return m_errors; }
    bool isCorrect() const { return m_errors.isEmpty() && analyzer.isCorrect(); }

protected:
    AbstractMappingGraph() {}
    AbstractMappingGraph(const AbstractMappingGraph& other) {}
    
    void appendError(const QString &error) { m_errors.append(error); }
    void flushErrors() { m_errors.clear(); }
    
    //WARNING see if errorCount is necesary ...
    int errorCount() const { return m_errors.count(); } // if some method throws many erros perhaps the user (child-class) want to stop something
    
    Analitza::Analyzer analyzer;
    
private:
    QStringList m_errors;

    DrawingPrecision m_drawingPrecision;
};

///

class ANALITZAPLOT_EXPORT AbstractFunctionGraph : public AbstractMappingGraph
{
public:
    explicit AbstractFunctionGraph(const Analitza::Expression& e, Analitza::Variables* v);
    virtual ~AbstractFunctionGraph();

    //FunctionGraph
    //no lleva const porque se calcularan valores con m_argumentIntervals
    QPair<double, double> intervalValues(const QString &argname);
    void setIntervalValues(const QString &argname, const QPair<double, double> &interval);
    
    QPair<Analitza::Expression, Analitza::Expression> intervalExpressionValues(const QString &argname) const;
    void setIntervalExpressionValues(const QString &argname, const QPair<Analitza::Expression, Analitza::Expression> &interval);
    
    virtual QStringList arguments() const = 0;

protected:
    AbstractFunctionGraph() {}
    AbstractFunctionGraph(const AbstractFunctionGraph& other);
    
    Analitza::Cn* arg(const QString &argname) { return dynamic_cast<Analitza::Cn*>(m_argumentValues[argname]); }

private:
    QMap<QString, Analitza::Object*> m_argumentValues;
    QMap<QString, RealInterval > m_argumentIntervals;
    Analitza::Analyzer* m_intervalsAnalizer; //solo es necesario 1?
};

///

class ANALITZAPLOT_EXPORT AbstractPlaneCurve : public AbstractFunctionGraph 
{
public:
    explicit AbstractPlaneCurve(const Analitza::Expression& e, Analitza::Variables* v);
    virtual ~AbstractPlaneCurve();

    //AbstractMappingGraph
    int spaceDimension() const { return 2; }
    
    //Curve ... los expongo como publicos tanto para planecurve como para los backend (
    //para los backends por un tema de performance y flexibilidad) 
    // al final en planecurve todo estara expuesto consistentemente 
    QVector<QPointF> points;
    QVector<int> jumps;
    
    //Own
    virtual void update(const QRect& viewport) = 0;
    virtual QPair<QPointF, QString> calc(const QPointF &mousepos) = 0;
    virtual QLineF derivative(const QPointF &mousepos) const = 0;

protected:
    AbstractPlaneCurve() {}
    AbstractPlaneCurve(const AbstractPlaneCurve& other) {}
    
    bool addPoint(const QPointF& p);


};

///

class ANALITZAPLOT_EXPORT AbstractSurface : public AbstractFunctionGraph //strategy pattern for curves
{
public:
    explicit AbstractSurface(const Analitza::Expression& e, Analitza::Variables* v);
    virtual ~AbstractSurface();

    //AbstractMappingGraph
    int spaceDimension() const { return 3; }
    
    //Own
    virtual void update(/*frumtum*/) = 0;
    const QVector<int> & indexes() const;
    const QVector<QVector3D> & points() const;
    
protected:
    AbstractSurface() {}
    AbstractSurface(const AbstractSurface& other) {}
    
//     bool addPoint(const QVector2D& p);
//     void setJump(int jump);
//     void clearPoints();
//     void clearJumps();

private:
    QMap< QString, RealInterval > m_argumentIntervals;
    QVector<int> m_indexes;
    QVector<QVector3D> m_points;
};


#endif // ANALITZAPLOT_FUNCTIONGRAPH_H
