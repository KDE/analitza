
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

#ifndef _ABSTRACTFUNCTIONGRAPH_H
#define _ABSTRACTFUNCTIONGRAPH_H

#include "abstractmappinggraph.h"


#define TYPE_NAME(name) \
const QString typeName() const { return TypeName(); } \
static QString TypeName() { return QString(name); } 

#define EXPRESSION_TYPE(name) \
static Analitza::ExpressionType ExpressionType() { return Analitza::ExpressionType(name); }

#define COORDDINATE_SYSTEM(name) \
CoordinateSystem coordinateSystem() const { return CoordSystem(); } \
static CoordinateSystem CoordSystem() { return name; }

//TODO validaciones strim etc 
#define PARAMETERS(name) \
QStringList parameters() const { return Parameters(); } \
static QStringList Parameters() { return QString(name).split(","); }

#define ICON_NAME(name) \
QString iconName() const { return IconName(); } \
static QString IconName() { return QString(name); } 

#define EXAMPLES(name) \
QStringList examples() const { return Examples(); } \
static QStringList Examples() { return QString(name).split(","); }


class ANALITZAPLOT_EXPORT AbstractFunctionGraph : public AbstractMappingGraph
{
public:
    AbstractFunctionGraph(const Analitza::Expression& e, Analitza::Variables* v);
    AbstractFunctionGraph(const Analitza::Expression& e);
    virtual ~AbstractFunctionGraph();
    
    int spaceDimension() const;

    Analitza::Variables *variables() const;
    void setVariables(Analitza::Variables *variables);

    const Analitza::Expression &expression() const;

    QStringList errors() const { return m_errors; }
    bool isCorrect() const { return m_errors.isEmpty() && analyzer->isCorrect(); }

    //FunctionGraph
    //no lleva const porque se calcularan valores con m_argumentIntervals
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
    
    QPair<double, double> interval(const QString &argname) const;
    bool setInterval(const QString &argname, double min, double max);
    
    virtual QStringList parameters() const = 0;

protected:
    AbstractFunctionGraph() {}
    AbstractFunctionGraph(const AbstractFunctionGraph& other);
    
    void appendError(const QString &error) { m_errors.append(error); }
    void flushErrors() { m_errors.clear(); }
    
    //WARNING see if errorCount is necesary ...
    int errorCount() const { return m_errors.count(); } // if some method throws many erros perhaps the user (child-class) want to stop something
    
    Analitza::Analyzer *analyzer;
    
    Analitza::Cn* arg(const QString &argname) { return dynamic_cast<Analitza::Cn*>(m_argumentValues[argname]); }
    
private:

//BEGIN private types
class EndPoint
{
public:
    EndPoint() : m_isInfinite(false)  {}
    
    //el owner de *analitza sera functiongraph ... no tiene sentido usar usar esta calse fuera de functiongraph
    EndPoint(double value): m_isInfinite(false)
    {
        setValue(value);
    }

    EndPoint(const Analitza::Expression &expression) : m_isInfinite(false)
    {
        if (!setValue(expression))
            setValue(0.0);
    }

    EndPoint(const EndPoint &other) : m_expressionValue(other.m_expressionValue), m_isInfinite(other.m_isInfinite)
    {
        
    }

    bool isInfinite() const 
    {
        return m_isInfinite;
    }
    
    void setInfinite(bool infinite) { m_isInfinite = infinite; }
    
    //no cambiar el exp de analyzer en setvalue por el costo ... solo cuando se quier calcular el value
    //case evaluate = true
    Analitza::Expression value(Analitza::Analyzer *analyzer) const
    { 
            Q_ASSERT(analyzer);

            analyzer->setExpression(m_expressionValue);
        
            //TODO checks
            if (!m_isInfinite)
    //             if (m_analyzer->isCorrect())
                return analyzer->calculate();

//             return std::numeric_limits<double>::infinity();
              return Analitza::Expression(Analitza::Cn("inf")); //TODO
        
    }

    //case evaluate = false
    Analitza::Expression value() const
    { 
        return m_expressionValue;
    }
    
    
    //no cambiar el exp de analyzer en setvalue por el costo ... solo cuando se quier calcular el value
    void setValue(double value)
    { 
        m_expressionValue = Analitza::Expression(Analitza::Expression(Analitza::Cn(value))); 
    }
    
    bool setValue(const Analitza::Expression &expression)
    {
        if (!expression.isCorrect())
            return false;
        else
        {
            m_expressionValue = Analitza::Expression(expression);

            return true;
        }
        
        return false;
    }
    
    bool operator==(const EndPoint &other) const 
    { 
        return m_expressionValue == other.m_expressionValue && m_isInfinite == other.m_isInfinite;
    }
    
    EndPoint operator=(const EndPoint& other) 
    {
        m_expressionValue = other.m_expressionValue;
        m_isInfinite = other.m_isInfinite;
        
        return *this;
    }

private:
    Analitza::Expression m_expressionValue;
    bool m_isInfinite;
};

class RealInterval
{
public:
    RealInterval() {}
    RealInterval(const EndPoint &lEndPoint, const EndPoint &hEndPoint) : m_lowEndPoint(lEndPoint), m_highEndPoint(hEndPoint) { }
    RealInterval(const RealInterval &other) : m_lowEndPoint(other.m_lowEndPoint), m_highEndPoint(other.m_highEndPoint) {}

    EndPoint lowEndPoint() const { return m_lowEndPoint; }
    EndPoint highEndPoint() const { return m_highEndPoint; }
    
    void setEndPoints(const EndPoint &lEndPoint, EndPoint &hEndPoint)
    {
        m_lowEndPoint = lEndPoint;
        m_highEndPoint = hEndPoint;
    }

    bool operator==(const RealInterval &other) const 
    {
        return (m_lowEndPoint == other.m_lowEndPoint) && (m_highEndPoint == other.m_highEndPoint);  
    }

    RealInterval operator=(const RealInterval& other) 
    {
        m_lowEndPoint = other.m_lowEndPoint;
        m_highEndPoint = other.m_highEndPoint;
        
        return *this;
    }
    
private:
    EndPoint m_lowEndPoint;
    EndPoint m_highEndPoint;
};
//END private types
    QStringList m_errors;

    QMap<QString, Analitza::Object*> m_argumentValues;
    QMap<QString, RealInterval > m_argumentIntervals;
};


#endif // ABSTRACTFUNCTIONGRAPH_H
