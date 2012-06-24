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


class EndPoint
{
public:
    EndPoint() {}
    
    EndPoint(double value, Analitza::Variables *varmod)
    {
        m_analyzer = QSharedPointer<Analitza::Analyzer>(new Analitza::Analyzer(varmod));
        
        setValue(value);
    }

    EndPoint(const Analitza::Expression &expression, Analitza::Variables *varmod)
    {
        m_analyzer = QSharedPointer<Analitza::Analyzer>(new Analitza::Analyzer(varmod));

        if (!setValue(expression))
            setValue(0.0);
    }

    EndPoint(const EndPoint &other) : m_analyzer(other.m_analyzer) { }

    bool isInfinite() const 
    {
        return m_analyzer->expression().toString() == QString("-inf") || m_analyzer->expression().toString() == QString("inf");
    }
    
    double value() 
    { 
        //TODO no magic numbers
        if (m_analyzer->expression().toString() == QString("-inf"))
            return -1000000;
        
        if (m_analyzer->expression().toString() == QString("inf"))
            return 1000000;
        
        return m_analyzer->calculate().toReal().value(); 
    }
    
    void setValue(double value) { m_analyzer->setExpression(Analitza::Expression(Analitza::Cn(value))); }
    
    bool setValue(const Analitza::Expression &expression)
    {
        if (!expression.isCorrect())
            return false;
        else
        {
            m_analyzer->setExpression(expression);

            if (expression.toString().isEmpty() || expression.toString() == QString("-inf"))
                m_analyzer->setExpression(Analitza::Expression("-inf"));

            if (expression.toString().isEmpty() || expression.toString() == QString("inf"))
                m_analyzer->setExpression(Analitza::Expression("inf"));
         
            return true;
        }
    }
    
    const Analitza::Expression & expression() const { return m_analyzer->expression(); }

    bool operator==(const EndPoint &other) const { return m_analyzer->expression() == other.m_analyzer->expression(); }
    EndPoint operator=(const EndPoint& other) 
    {
        m_analyzer = other.m_analyzer;
        
        return *this;
    }

private:

    
    QSharedPointer<Analitza::Analyzer> m_analyzer;
};

class RealInterval
{
public:
    RealInterval() {}
    RealInterval(const EndPoint &lowEndPoint, const EndPoint &highEndPoint) : m_lowEndPoint(lowEndPoint), m_highEndPoint(highEndPoint) {}
    RealInterval(const RealInterval &other) : m_lowEndPoint(other.m_lowEndPoint) {}

    EndPoint lowEndPoint() { return m_lowEndPoint; }
    EndPoint highEndPoint() { return m_highEndPoint; }
    
    void setEndPoints(const EndPoint &lowEndPoint, EndPoint &highEndPoint)
    {
        m_lowEndPoint = lowEndPoint;
        m_highEndPoint = highEndPoint;
    }

    bool operator==(const RealInterval &other) const 
    {
        return (m_lowEndPoint == other.m_lowEndPoint) && (m_highEndPoint == other.m_highEndPoint);  
    }

    RealInterval operator=(const RealInterval& other) 
    {
        m_lowEndPoint = other.m_lowEndPoint;
        m_highEndPoint = other.m_lowEndPoint;
        
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
