/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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

#include "functiongraph.h"

#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variable.h"

AbstractMappingGraph::AbstractMappingGraph(const Analitza::Expression& e, Analitza::Variables* v)
: analyzer(v)
{
    analyzer.setExpression(e);
    analyzer.simplify();
    analyzer.flushErrors();
}

AbstractMappingGraph::~AbstractMappingGraph()
{
}

const Analitza::Expression& AbstractMappingGraph::expression() const
{
    return analyzer.expression();
}

///

AbstractFunctionGraph::AbstractFunctionGraph(const Analitza::Expression& e, Analitza::Variables* v)
: AbstractMappingGraph(e,v)
{
    m_intervalsAnalizer = new Analitza::Analyzer(v);
    
    foreach (const Analitza::Ci *var, analyzer.expression().parameters())
    {
        m_argumentValues[var->name()] = new Analitza::Cn;
        
        //WARNING FIX magic numbers
        EndPoint min(-5.0);
        EndPoint max(8.0);
        
        m_argumentIntervals[var->name()] = RealInterval(min, max);
    }

    analyzer.setStack(m_argumentValues.values().toVector());
}

AbstractFunctionGraph::~AbstractFunctionGraph()
{
    qDeleteAll(m_argumentValues.begin(), m_argumentValues.end());
    m_argumentValues.clear();

    delete m_intervalsAnalizer;
}


QPair<double, double> AbstractFunctionGraph::intervalValues(const QString &argname)
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    QPair<double, double> ret;
    
    ret.first = m_argumentIntervals[argname].lowEndPoint().value(m_intervalsAnalizer);
    ret.second = m_argumentIntervals[argname].highEndPoint().value(m_intervalsAnalizer);
    
    return ret;
}

void AbstractFunctionGraph::setIntervalValues(const QString &argname, const QPair<double, double> &interval)
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    m_argumentIntervals[argname] = RealInterval(EndPoint(interval.first), EndPoint(interval.second));
}

QPair<Analitza::Expression, Analitza::Expression> AbstractFunctionGraph::intervalExpressionValues(const QString &argname) const
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    QPair<Analitza::Expression, Analitza::Expression> ret;
    
    ret.first = m_argumentIntervals[argname].lowEndPoint().expression();
    ret.second = m_argumentIntervals[argname].highEndPoint().expression();
    
    return ret;
}

void AbstractFunctionGraph::setIntervalExpressionValues(const QString &argname, const QPair<Analitza::Expression, Analitza::Expression> &interval)
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    m_argumentIntervals[argname] = RealInterval(EndPoint(interval.first), EndPoint(interval.second));
}


//TODO borrar siguiente iteracion
// RealInterval AbstractFunctionGraph::argumentInterval(const QString &argname) const
// {
//     
//     
//     return m_argumentIntervals[argname];
// }
// 
// void AbstractFunctionGraph::setArgumentInverval(const QString &argname, const RealInterval &interval)
// {
//     Q_ASSERT(m_argumentIntervals.contains(argname));
//     
//     m_argumentIntervals[argname] = interval;
//     
//     //NOTE aqui se le pasa el analitza a los intervals
//     m_argumentIntervals[argname].lowEndPoint().setAnalitza(m_intervalsAnalizer);
//     m_argumentIntervals[argname].highEndPoint().setAnalitza(m_intervalsAnalizer);
// }

///

AbstractPlaneCurve::AbstractPlaneCurve(const Analitza::Expression& e, Analitza::Variables* v)
: AbstractFunctionGraph(e, v)
{

}

AbstractPlaneCurve::~AbstractPlaneCurve()
{

}

QVector< int > AbstractPlaneCurve::jumps() const
{
    return m_jumps;
}

    //Own
const QVector<QPointF> & AbstractPlaneCurve::points() const
{
//     qDebug() << m_points.size();
    return m_points;
}


#include <cmath>
using std::atan2;


bool AbstractPlaneCurve::addPoint(const QPointF& p)
{
    int count=m_points.count();
    if(count<2) {
        m_points.append(p);
        return false;
    }
    
    double angle1=std::atan2(m_points[count-1].y()-m_points[count-2].y(), m_points[count-1].x()-m_points[count-2].x());
    double angle2=std::atan2(p.y()-m_points[count-1].y(), p.x()-m_points[count-1].x());
    
    bool append=!isSimilar(angle1, angle2);
    if(append)
        m_points.append(p);
    else
        m_points.last()=p;
        
    return append;
}

void AbstractPlaneCurve::setJump(int jump)
{
    
}

void AbstractPlaneCurve::clearPoints()
{
    m_points.clear();
}

void AbstractPlaneCurve::clearJumps()
{
    m_jumps.clear();
}

///



AbstractSurface::AbstractSurface(const Analitza::Expression& e, Analitza::Variables* v)
: AbstractFunctionGraph(e, v)
{

}

AbstractSurface::~AbstractSurface()
{

}

const QVector< int >& AbstractSurface::indexes() const
{
return QVector<int>();
}

const QVector< QVector3D >& AbstractSurface::points() const
{
return QVector<QVector3D>();
}

    
