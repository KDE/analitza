
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
#include <analitza/variables.h>

using namespace Analitza;

// combobox For 3 or fewer options, use a set of radio buttons. 
// no combo para dict For more than 10 options, use a list. 
// get new plots 
// 
// Make the list control large enough that it can show at least four items at a time without scrolling. 
// For lists of ten or more items, increase this minimum size as appropriate. 

AbstractFunctionGraph::AbstractFunctionGraph(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v)
    : AbstractMappingGraph()
    , m_resolution(200)
    , m_e(e)
    , m_varsmod(v ? new Variables(*v) : new Variables)
{
    analyzer = new Analitza::Analyzer(m_varsmod);

    Q_ASSERT(!m_e.isEquation());
    
    analyzer->setExpression(m_e);
    analyzer->simplify();

    QStack<Analitza::Object*> stack;
    foreach(const QString& var, analyzer->expression().bvarList()) {
        Analitza::Cn* val = new Analitza::Cn;
        stack.push(val);
        m_argumentValues.insert(var, val);
    }
    analyzer->setStack(stack);
}

AbstractFunctionGraph::~AbstractFunctionGraph()
{
    qDeleteAll(m_argumentValues);
    delete analyzer;
    delete m_varsmod;
}

Dimension AbstractFunctionGraph::spaceDimension() const
{
    return FunctionGraphFactory::self()->spaceDimension(m_internalId);
}

Analitza::Variables *AbstractFunctionGraph::variables() const 
{ 
    return m_varsmod;
}

void AbstractFunctionGraph::setVariables(Analitza::Variables* variables)
{
    Q_ASSERT(variables);
    
    Analitza::Expression exp = analyzer->expression();
    QVector<Analitza::Object*> prevStack = analyzer->runStack();
    delete analyzer;
    delete m_varsmod;
    
    analyzer = new Analitza::Analyzer(variables);
    analyzer->setExpression(exp);
    analyzer->setStack(prevStack);
    
    m_varsmod = variables;
}

const Analitza::Expression& AbstractFunctionGraph::expression() const
{
    return m_e;
}

QPair<Analitza::Expression, Analitza::Expression> AbstractFunctionGraph::interval(const QString &argname, bool evaluate) const
{
    Q_ASSERT(analyzer->expression().bvarList().contains(argname));
    
    QPair<Analitza::Expression, Analitza::Expression> ret;
    
    if (evaluate) {
        Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());

        ret.first = m_argumentIntervals[argname].lowEndPoint().value(intervalsAnalizer);
        ret.second = m_argumentIntervals[argname].highEndPoint().value(intervalsAnalizer);
        
        delete intervalsAnalizer; 
        
    } else {
        ret.first = m_argumentIntervals[argname].lowEndPoint().value();
        ret.second = m_argumentIntervals[argname].highEndPoint().value();
    }
    
    return ret;
}

bool AbstractFunctionGraph::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(analyzer->expression().bvarList().contains(argname));
    
    if (min == Expression(Analitza::Cn("-inf")) && 
        max == Expression(Analitza::Cn("inf")))
    {
        m_argumentIntervals[argname] = RealInterval(EndPoint(min), EndPoint(max));        
    }
    
    Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());
    intervalsAnalizer->setExpression(min);
    double min_val = intervalsAnalizer->calculate().toReal().value();
    intervalsAnalizer->setExpression(max);
    double max_val = intervalsAnalizer->calculate().toReal().value();
    
    delete intervalsAnalizer;
    
    if (max_val < min_val)
        return false;
    
    m_argumentIntervals[argname] = RealInterval(EndPoint(min), EndPoint(max));
    
    return true;
}

QPair<double, double> AbstractFunctionGraph::interval(const QString &argname) const
{
    Q_ASSERT(analyzer->expression().bvarList().contains(argname));
    
    QPair<double, double> ret;
    
    Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());
    ret.first = m_argumentIntervals[argname].lowEndPoint().value(intervalsAnalizer).toReal().value();
    ret.second = m_argumentIntervals[argname].highEndPoint().value(intervalsAnalizer).toReal().value();
    
    delete intervalsAnalizer;

    return ret;
}

void AbstractFunctionGraph::clearIntervals()
{
    m_argumentIntervals.clear();
}

bool AbstractFunctionGraph::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(analyzer->expression().bvarList().contains(argname));
    
    if (max < min)
        return false;

    m_argumentIntervals[argname] = RealInterval(EndPoint(min), EndPoint(max));

    return true;
}

Analitza::Cn* AbstractFunctionGraph::arg(const QString& argname)
{
    return m_argumentValues[argname];
}

bool AbstractFunctionGraph::hasIntervals() const
{
    return !m_argumentIntervals.isEmpty();
}

bool AbstractFunctionGraph::isCorrect() const
{
    return m_errors.isEmpty() && analyzer->isCorrect();
}

QStringList AbstractFunctionGraph::errors() const
{
    QStringList ret = m_errors+analyzer->errors();
    ret.removeDuplicates();
    return ret;
}
