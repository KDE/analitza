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

#include "functiongraph.h"

#include "private/abstractfunctiongraph.h"
#include "private/functiongraphfactory.h"

using namespace Analitza;

FunctionGraph::FunctionGraph(AbstractFunctionGraph *g)
    : PlotItem(QStringLiteral("123123213123"), Qt::black)
    , m_functionGraph(g)
{
    Q_ASSERT(m_functionGraph);
}

FunctionGraph::~FunctionGraph()
{
    delete m_functionGraph;
}

QString FunctionGraph::typeName() const
{
    Q_ASSERT(m_functionGraph);
    return m_functionGraph->typeName();
}

QString FunctionGraph::display() const
{
    return m_display;
}

void FunctionGraph::setDisplay(const QString& display)
{
    m_display = display;
}

const Analitza::Expression& FunctionGraph::expression() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->expression();
}

Analitza::Variables* FunctionGraph::variables() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->variables();
}

QString FunctionGraph::iconName() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->iconName();
}

Dimension FunctionGraph::spaceDimension() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->spaceDimension();
}

CoordinateSystem FunctionGraph::coordinateSystem() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->coordinateSystem();
}

QStringList FunctionGraph::errors() const
{
    Q_ASSERT(m_functionGraph);
    
    QStringList err(m_errors);
    err += m_functionGraph->errors();
    return err;
}

bool FunctionGraph::isCorrect() const
{
    Q_ASSERT(m_functionGraph);

    return m_errors.isEmpty() && m_functionGraph && m_functionGraph->isCorrect();
}

QPair<Analitza::Expression, Analitza::Expression> FunctionGraph::interval(const QString &argname, bool evaluate) const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->interval(argname, evaluate);
}

bool FunctionGraph::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(m_functionGraph);
    
    bool ret = m_functionGraph->setInterval(argname, min, max);
    
    if (ret)
        emitDataChanged();
    
    return ret;
}

QPair<double, double> FunctionGraph::interval(const QString &argname) const
{
    Q_ASSERT(m_functionGraph);

    return m_functionGraph->interval(argname);
}

bool FunctionGraph::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(m_functionGraph);
    
    bool ret = m_functionGraph->setInterval(argname, min, max);

    if (ret)
        emitDataChanged();
    
    return ret;
}

void FunctionGraph::clearIntervals()
{
    m_functionGraph->clearIntervals();
}

bool FunctionGraph::hasIntervals() const
{
    return m_functionGraph->hasIntervals();
}

QStringList FunctionGraph::parameters() const
{
    Q_ASSERT(m_functionGraph);
    return m_functionGraph->parameters();
}

void FunctionGraph::setResolution(int resolution)
{
    Q_ASSERT(m_functionGraph);
    return m_functionGraph->setResolution(resolution);
}
