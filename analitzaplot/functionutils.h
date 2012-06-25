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


#ifndef ANALITZAPLOT_FUNCTIONUTILS
#define ANALITZAPLOT_FUNCTIONUTILS

#include "analitzaplotexport.h"
#include <analitza/expression.h>
#include <analitza/analyzer.h>
#include <analitza/value.h>

#include <cmath>
#include <limits>


#include <QPair>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QLineF>
#include <qsharedpointer.h>

using std::acos;
using std::atan;
using std::fabs;
using std::cos;
using std::sin;
using std::sqrt;

const double PI = std::acos(-1.0);

namespace Analitza
{
class Expression;
}

enum DrawingPrecision { LowPrecision = 1, MediumPrecision, HighPrecision};
enum CoordinateSystem { Cartesian = 1, Polar, Cylindrical, Spherical };


//2 private class not used on public api

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
    double value(Analitza::Analyzer *analyzer) const
    { 
        Q_ASSERT(analyzer);

        analyzer->setExpression(m_expressionValue);
        
        //TODO checks
        if (!m_isInfinite)
//             if (m_analyzer->isCorrect())
            return analyzer->calculate().toReal().value();
        
        return std::numeric_limits<double>::infinity();
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
    
    const Analitza::Expression & expression() const { return m_expressionValue; }

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


/*
//TODO endpoints to expressions
class EndPoint
{
public:
    EndPoint(double value = 0.0) : m_isInfinite(false), m_value(value) {}

    bool isInfinite() const { return m_isInfinite; }
    void setInfinite(bool infinite) { m_isInfinite = infinite; }
    double value() const { Q_ASSERT(m_isInfinite); return m_value; }
    void setValue(double value) { Q_ASSERT(m_isInfinite); m_value = value; }

private:

    bool m_isInfinite;
    double m_value;
};

class RealInterval
{
public:
    RealInterval(const EndPoint &lowEndPoint = EndPoint(-1.0), 
                 const EndPoint &highEndPoint = EndPoint(1.0))
    : m_lowEndPoint(lowEndPoint), m_highEndPoint(highEndPoint) { }

    EndPoint lowEndPoint() const { return m_lowEndPoint; }
    EndPoint highEndPoint() const { return m_highEndPoint; }
    void setEndPoints(const EndPoint &lowEndPoint, EndPoint &highEndPoint)
    {
        m_lowEndPoint = lowEndPoint;
        m_highEndPoint = highEndPoint;
    }

private:
    EndPoint m_lowEndPoint;
    EndPoint m_highEndPoint;
};*/


static bool isSimilar(double a, double b, double diff = .0000001)
{
    return fabs(a-b) < diff;
}

static bool traverse(double p1, double p2, double next)
{
    static const double delta=3;
    double diff=p2-p1, diff2=next-p2;
    bool ret=false;

    if(diff>0 && diff2<-delta)
        ret=true;
    else if(diff<0 && diff2>delta)
        ret=true;

    return ret;
}

static QLineF slopeToLine(const double &der)
{
    double arcder = atan(der);
    const double len=6.*der;
    QPointF from, to;
    from.setX(len*cos(arcder));
    from.setY(len*sin(arcder));

    to.setX(-len*cos(arcder));
    to.setY(-len*sin(arcder));
    return QLineF(from, to);
}

static QLineF mirrorXY(const QLineF& l)
{
    return QLineF(l.y1(), l.x1(), l.y2(), l.x2());
}

#endif // ANALITZAPLOT_FUNCTIONUTILS
