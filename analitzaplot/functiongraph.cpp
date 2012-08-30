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

#include <QUuid>

#include "analitza/analyzer.h"
#include "analitza/variable.h"
#include <analitza/variables.h>
#include <analitza/localize.h>

#include "private/abstractfunctiongraph.h"

#include "private/functiongraphfactory.h"

FunctionGraph::FunctionGraph(const Analitza::Expression &functionExpression, Dimension spacedim, const QString &n, const QColor &col, Analitza::Variables *vars)
    : PlotItem(n, col)
    , m_functionGraph(0)
{
    setName(n);
    setColor(col);
    QString id;
    
    Q_ASSERT(canDraw(functionExpression, spacedim, m_errors, id));

    m_functionGraph = FunctionGraphFactory::self()->build(id, functionExpression, vars);
    m_functionGraph->setInternalId(id);
    Q_ASSERT(m_functionGraph);
}

FunctionGraph::~FunctionGraph()
{
    delete m_functionGraph;
}

const QString FunctionGraph::typeName() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->typeName();
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

QStringList FunctionGraph::examples() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->examples();
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
    if(m_functionGraph) {
        err += m_functionGraph->errors();
    }
    return err;
}

bool FunctionGraph::isCorrect() const
{
    Q_ASSERT(m_functionGraph);

    return m_errors.isEmpty() && m_functionGraph && m_functionGraph->isCorrect();
}

bool FunctionGraph::isAutoUpdate() const
{
    Q_ASSERT(m_functionGraph);

    return m_functionGraph->isAutoUpdate();
}

void FunctionGraph::setAutoUpdate(bool b)
{
    Q_ASSERT(m_functionGraph);

    m_functionGraph->setAutoUpdate(b);
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

QStringList FunctionGraph::parameters() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->parameters();
}

bool FunctionGraph::canDraw(const Analitza::Expression &functionExpression, Dimension spacedim)
{
    QStringList nonusederr;
    QString nonusedid;    
    return canDraw(functionExpression, spacedim, nonusederr, nonusedid);
}

bool FunctionGraph::canDraw(const Analitza::Expression &functionExpression, Dimension spacedim, QStringList &errs)
{
    QString nonusedid;    
    return canDraw(functionExpression, spacedim, errs, nonusedid);
}

bool FunctionGraph::setExpression(const Analitza::Expression& functionExpression, Dimension spacedim)
{
    QString id;
    Analitza::Expression expr;
    
    if (!canDraw(functionExpression, spacedim, m_errors, id)) return false;

    QMap<QString, AbstractFunctionGraph::RealInterval > argumentIntervals = m_functionGraph->m_argumentIntervals;

    //NOTE antes de borrar el analyzer de backend se debe almacenar que variables obtuvo cuando fue construido
    //caso contrario se generara un nuevo backend con variables independientes
    Analitza::Variables *vars = m_functionGraph->variables();
    delete m_functionGraph;

    m_functionGraph = static_cast<AbstractFunctionGraph*>(FunctionGraphFactory::self()->build(id,functionExpression, vars));
    m_functionGraph->setInternalId(id);
    
    m_functionGraph->m_argumentIntervals = argumentIntervals;
    
    emitDataChanged();
    
    return true;
}

bool FunctionGraph::canDraw(const Analitza::Expression& testexp, Dimension spacedim, QStringList& errs, QString& id)
{
    errs.clear();
    id = QString();
    
    if(!testexp.isCorrect() || testexp.toString().isEmpty())
    {
        errs << i18n("The expression is not correct");
        
        return false;
    }
    
    Analitza::Analyzer *a = new Analitza::Analyzer;
    
    QString expectedid;

    a->setExpression(testexp);
    
    if (testexp.isEquation())
    {
        a->setExpression(a->expression().equationToFunction());
        a->setExpression(a->dependenciesToLambda());
    }
    else if (testexp.isLambda())
        a->setExpression(a->dependenciesToLambda());
    else
    {
        //pues no es ni ecuacion ni lambda
        errs << i18n("The expression is not correct");

        delete a;
        
        return false;
    }
        
    Q_ASSERT(errs.isEmpty());
    
    QStringList bvars = a->expression().bvarList();
    
    if (testexp.isEquation())
        a->setExpression(testexp);
    
    expectedid = FunctionGraphFactory::self()->trait(a->expression(), spacedim);
    
    if(!FunctionGraphFactory::self()->contains(expectedid))
        errs << i18n("Function type not recognized");
    else if(!a->isCorrect())
        errs << a->errors();
    else 
    {
        Analitza::ExpressionType expected=FunctionGraphFactory::self()->expressionType(expectedid);
        Analitza::ExpressionType actual=a->type();

        if(!actual.canReduceTo(expected)) 
        {
            delete a;
            
            errs << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
            
            return false;
        }
    }
    
    delete a;
    
    if (errs.empty())
        id = expectedid;
    
    return errs.empty();
}
