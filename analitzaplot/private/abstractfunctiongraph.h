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

#include "abstractplotitem.h"
#include <analitza/expression.h>
#include <analitza/analyzer.h>
#include <analitza/value.h>
#include <QCoreApplication>
#include <QPair>

namespace Analitza {
    class Variables;
    class Analyzer;
}

#define TYPE_NAME(name) \
const QString typeName() const override { return QCoreApplication::tr(TypeName()); } \
static const char* TypeName() { return name; } 

#define EXPRESSION_TYPE(name) \
static Analitza::ExpressionType ExpressionType() { return Analitza::ExpressionType(name); }

#define COORDDINATE_SYSTEM(name) \
CoordinateSystem coordinateSystem() const override { return CoordSystem(); }\
static CoordinateSystem CoordSystem() { return name; }

#define PARAMETERS(name) \
QStringList parameters() const override { return Parameters(); } \
static QStringList Parameters() { return (name); }

#define ICON_NAME(name) \
QString iconName() const override { return IconName(); } \
static QString IconName() { return (name); } 

#define EXAMPLES(exs) \
static QStringList Examples() { return (exs); }

namespace Analitza {

class AbstractFunctionGraph : public AbstractMappingGraph
{
friend class FunctionGraphFactory;
public:
    explicit AbstractFunctionGraph(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v = {});
    ~AbstractFunctionGraph() override;
    
    Dimension spaceDimension() const override;

    Analitza::Variables *variables() const;
    void setVariables(Analitza::Variables *variables);

    const Analitza::Expression &expression() const;

    QStringList errors() const;
    bool isCorrect() const;
    
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    virtual bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
    
    QPair<double, double> interval(const QString &argname) const;
    virtual bool setInterval(const QString &argname, double min, double max);
    void clearIntervals();
    bool hasIntervals() const;
    
    virtual QStringList parameters() const = 0;

    void setResolution(int res) { m_resolution = res; }
    
protected:
    void appendError(const QString &error) { m_errors.append(error); }
    void flushErrors() { m_errors.clear(); }
    
    void setInternalId(const QString &iid) { m_internalId = iid; }
    
    Analitza::Cn* arg(const QString &argname);
    
    Analitza::Analyzer *analyzer;
    int m_resolution;
    
private:
    QString m_internalId;    
    
    Analitza::Expression m_e;
    Analitza::Variables *m_varsmod;

//BEGIN private types
class EndPoint
{
public:
    EndPoint() {}

    explicit EndPoint(double value)
    {
        setValue(value);
    }

    explicit EndPoint(const Analitza::Expression &expression)
    {
        if (!setValue(expression))
            setValue(0.0);
    }

    EndPoint(const EndPoint &other) : m_expressionValue(other.m_expressionValue)
    {
        
    }
    
    Analitza::Expression value(Analitza::Analyzer *analyzer) const
    { 
        analyzer->setExpression(m_expressionValue);
        return analyzer->calculate();
    }

    Analitza::Expression value() const
    { 
        return m_expressionValue;
    }
    
    void setValue(double value)
    { 
        m_expressionValue = Analitza::Expression(Analitza::Cn(value)); 
    }
    
    bool setValue(const Analitza::Expression &expression)
    {
        if (!expression.isCorrect())
            return false;
        else
        {
            m_expressionValue = expression;

            return true;
        }
        
        return false;
    }
    
    bool operator==(const EndPoint &other) const 
    { 
        return m_expressionValue == other.m_expressionValue;
    }
    
    EndPoint operator=(const EndPoint& other) 
    {
        m_expressionValue = other.m_expressionValue;
        
        return *this;
    }

private:
    Analitza::Expression m_expressionValue;
};

class RealInterval
{
public:
    RealInterval() {}
    RealInterval(const EndPoint &lEndPoint, const EndPoint &hEndPoint) : m_lowEndPoint(lEndPoint), m_highEndPoint(hEndPoint) {}
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

    QMap<QString, Analitza::Cn*> m_argumentValues;
    QMap<QString, RealInterval > m_argumentIntervals;
};

}

#endif // ABSTRACTFUNCTIONGRAPH_H
