/*************************************************************************************
*  Copyright (C) 2012 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "plotsfactory.h"
#include <analitzaplot/functiongraph.h>
#include "private/functiongraphfactory.h"
#include <analitza/analyzer.h>
#include <analitza/variables.h>
#include <QStringList>
#include <QCoreApplication>

using namespace Analitza;

Q_GLOBAL_STATIC(PlotsFactory, factoryInstance)

PlotsFactory::PlotsFactory()
    : m_vars(new Variables)
{}

PlotsFactory::~PlotsFactory()
{
}

PlotsFactory* PlotsFactory::self()
{
    return factoryInstance;
}

PlotBuilder PlotsFactory::requestPlot(const Analitza::Expression& testexp, Dimension dim, const QSharedPointer<Variables> &vars) const
{
    QStringList errs;
    
    if(!testexp.isCorrect() || testexp.toString().isEmpty()) {
        errs << QCoreApplication::tr("The expression is not correct");
        PlotBuilder b;
        b.m_errors = errs;
        return b;
    }
    
    Analitza::Expression exp(testexp);
    if(exp.isDeclaration())
        exp = exp.declarationValue();
    
    if(exp.isEquation())
        exp = exp.equationToFunction();
    
    Analitza::Analyzer a(vars ? vars : m_vars);
    a.setExpression(exp);
    a.setExpression(a.dependenciesToLambda());

    QString id;
    if(a.isCorrect()) {
        QString expectedid = FunctionGraphFactory::self()->trait(a.expression(), a.type(), dim);
        if(FunctionGraphFactory::self()->contains(expectedid)) {
            id = expectedid;
        } else if (!expectedid.isEmpty())
            errs << QCoreApplication::tr("Function type '%1' not recognized").arg(expectedid);
        else
            errs << QCoreApplication::tr("Function '%1' not recognized").arg(a.expression().toString());
    } else {
        errs << a.errors();
    }
    
    Q_ASSERT(!errs.isEmpty() || !id.isEmpty());
    
    PlotBuilder b;
    b.m_errors = errs;
    b.m_id = id;
    b.m_expression = a.expression();
    b.m_display = testexp.toString();
    b.m_vars = vars;
    return b;
}

QStringList PlotsFactory::examples(Dimensions s) const
{
    QStringList examples;
    if(s & Dim1D) examples += FunctionGraphFactory::self()->examples(Dim1D);
    if(s & Dim2D) examples += FunctionGraphFactory::self()->examples(Dim2D);
    if(s & Dim3D) examples += FunctionGraphFactory::self()->examples(Dim3D);
    return examples;
}

//------------------------------------------ PlotBuilder

PlotBuilder::PlotBuilder()
    : m_vars(nullptr)
{}

bool PlotBuilder::canDraw() const
{
    return m_errors.isEmpty() && !m_id.isEmpty();
}

FunctionGraph* PlotBuilder::create(const QColor& color, const QString& name) const
{
    FunctionGraph* it = FunctionGraphFactory::self()->buildItem(m_id, m_expression, m_vars);
    it->setColor(color);
    it->setName(name);
    it->setDisplay(m_display);
    return it;
}

Analitza::Expression PlotBuilder::expression() const
{
    return m_expression;
}

QString PlotBuilder::display() const
{
    return m_display;
}
