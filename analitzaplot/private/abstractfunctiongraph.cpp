
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

#include "abstractfunctiongraph.h"
#include "functiongraphfactory.h"

#include "analitza/variable.h"

AbstractFunctionGraph::AbstractFunctionGraph(const Analitza::Expression& e, Analitza::Variables* v)
: AbstractMappingGraph(), analyzer(new Analitza::Analyzer(v))
{
    //TODO intervalparameter tiene que ser const ... no es necesario que no lo sea ... siempre es preferible que algo sea inmutable :)
//     m_intervalsAnalizer = new Analitza::Analyzer(v);
    
        analyzer->setExpression(e);
    analyzer->simplify();
    analyzer->flushErrors();
    
    foreach (const Analitza::Ci *var, analyzer->expression().parameters())
    {
        m_argumentValues[var->name()] = new Analitza::Cn;
        
        //WARNING FIX magic numbers
        EndPoint min(-5.0);
        EndPoint max(8.0);
        
        m_argumentIntervals[var->name()] = RealInterval(min, max);
    }

    analyzer->setStack(m_argumentValues.values().toVector());
}

AbstractFunctionGraph::AbstractFunctionGraph(const Analitza::Expression& e)
: AbstractMappingGraph(), analyzer(new Analitza::Analyzer)
{
    //TODO intervalparameter tiene que ser const ... no es necesario que no lo sea ... siempre es preferible que algo sea inmutable :)
//     m_intervalsAnalizer = new Analitza::Analyzer(v);
    
        analyzer->setExpression(e);
    analyzer->simplify();
    analyzer->flushErrors();
    
    foreach (const Analitza::Ci *var, analyzer->expression().parameters())
    {
        m_argumentValues[var->name()] = new Analitza::Cn;
        
        //WARNING FIX magic numbers
        EndPoint min(-5.0);
        EndPoint max(8.0);
        
        m_argumentIntervals[var->name()] = RealInterval(min, max);
    }

    analyzer->setStack(m_argumentValues.values().toVector());
}

AbstractFunctionGraph::~AbstractFunctionGraph()
{
    qDeleteAll(m_argumentValues.begin(), m_argumentValues.end());
    m_argumentValues.clear();

    delete analyzer;
}

int AbstractFunctionGraph::spaceDimension() const
{
    return FunctionGraphFactory::self()->spaceDimension(analyzer->type());
}

Analitza::Variables *AbstractFunctionGraph::variables() const 
{ 
    return analyzer->variables(); 
}

void AbstractFunctionGraph::setVariables(Analitza::Variables* variables)
{
    Q_ASSERT(variables);
    
    Analitza::Expression exp = analyzer->expression();
    
    delete analyzer;
    
    analyzer = new Analitza::Analyzer(variables);
    analyzer->setExpression(exp);
    analyzer->setStack(m_argumentValues.values().toVector());
}

const Analitza::Expression& AbstractFunctionGraph::expression() const
{
    return analyzer->expression();
}


QPair<Analitza::Expression, Analitza::Expression> AbstractFunctionGraph::interval(const QString &argname, bool evaluate) const
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    QPair<Analitza::Expression, Analitza::Expression> ret;
    
    if (evaluate)
    {
        //NOTE heap ?
        Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());

        ret.first = m_argumentIntervals[argname].lowEndPoint().value(intervalsAnalizer);
        ret.second = m_argumentIntervals[argname].highEndPoint().value(intervalsAnalizer);
        
        delete intervalsAnalizer;
    }
    else
    {
        ret.first = m_argumentIntervals[argname].lowEndPoint().value();
        ret.second = m_argumentIntervals[argname].highEndPoint().value();
    }
    
    return ret;
}

bool AbstractFunctionGraph::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());

    double min_val = m_argumentIntervals[argname].lowEndPoint().value(intervalsAnalizer).toReal().value();
    double max_val = m_argumentIntervals[argname].highEndPoint().value(intervalsAnalizer).toReal().value();
        
    delete intervalsAnalizer;

    if (max_val < min_val)
        return false;
    
    m_argumentIntervals[argname] = RealInterval(EndPoint(min), EndPoint(max));
    
    return true;
}

QPair<double, double> AbstractFunctionGraph::interval(const QString &argname) const
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    QPair<double, double> ret;
    
    Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());
    
    ret.first = m_argumentIntervals[argname].lowEndPoint().value(intervalsAnalizer).toReal().value();
    ret.second = m_argumentIntervals[argname].highEndPoint().value(intervalsAnalizer).toReal().value();
    
    delete intervalsAnalizer;
    
    return ret;
}

bool AbstractFunctionGraph::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(m_argumentIntervals.contains(argname));
    
    if (max < min)
        return false;
    
    m_argumentIntervals[argname] = RealInterval(EndPoint(min), EndPoint(max));
    
    return true;
}