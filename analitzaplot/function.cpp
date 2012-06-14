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


#include "function.h"

#include <KDE/KLocalizedString>
#include <QPen>

#include "analitza/analyzer.h"
#include <analitza/value.h>
#include <analitza/vector.h>
#include <analitza/variable.h>

#include "functiongraph.h"
#include "functiongraphfactory.h"

Function::Function()
: m_graph(0), m_graphColor(Qt::black)
{
}

Function::Function(const Function &f)
: m_name(f.m_name)
, m_expression(f.m_expression)
, m_isImplicit(f.m_isImplicit)
, m_type(f.m_type)
, m_arguments(f.m_arguments)
, m_graphDimension(f.m_graphDimension)
, m_coordinateSystem(f.m_coordinateSystem)
, m_graphPrecision(f.m_graphPrecision)
, m_plotStyle(f.m_plotStyle)
, m_graphColor(f.m_graphColor)
, m_graphVisible(f.m_graphVisible)
, m_graph(0)
, m_errors(f.errors())
{
    if (f.m_graph)
        m_graph = f.m_graph->copy();
}

Function::Function (const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem, Analitza::Variables *variables, bool isImplicit, const QString &name)
: m_name (name), m_graph(0)
{
    if (!functionExpression.isCorrect())
    {
        m_errors << i18n ("The expression is not correct");
    }

    Analitza::Analyzer analyzer(variables);
    analyzer.setExpression (functionExpression);
    m_expression = analyzer.dependenciesToLambda();
    analyzer.setExpression (m_expression);

    bool foundExpected = false;
    int expectedIndex = 0;

    for (int i = 0; i < FunctionFactory::self()->constructors.size(); ++i)
    {
        if ( (analyzer.type().canReduceTo(FunctionFactory::self()->expressionTypes[i])) &&
             (m_expression.bvarList() == FunctionFactory::self()->argumentNames[i]) &&
                (coordinateSystem == FunctionFactory::self()->coordinateSystems[i]) &&
                (isImplicit == FunctionFactory::self()->implicitFlags[i]))
        {
            foundExpected = true;
            expectedIndex = i;

            break;
        }
    }

    if (!foundExpected)
    {
        m_errors << i18n ("Function type not recognized");
    }
    else
        if (!analyzer.isCorrect())
        {
            m_errors << analyzer.errors();
        }
        else
        {
//             bool isDrawable = true;

            if (analyzer.expression().lambdaBody().isVector())
            {
                m_type = VectorValued;

//                 if (dynamic_cast<Analitza::Vector*>(m_analyzer.expression().lambdaBody().tree())->size() > 3)
//                     isDrawable = false;
            }
            else
            {
                m_type = RealValued;

//                 if (m_analyzer.expression().parameters().size() > 3) // maximo dibujable: superficies de nivel
//                     isDrawable = false;
            }

//             if (!isDrawable)
//                 m_errors << i18n ("Function can not be drawn");
//             else
            {
                m_isImplicit = isImplicit;

                m_coordinateSystem = coordinateSystem;

                m_graph = FunctionFactory::self()->create(expectedIndex, m_expression, m_coordinateSystem, analyzer.variables(), m_isImplicit);


                m_iconName = FunctionFactory::self()->iconNames[expectedIndex];
                m_examples = FunctionFactory::self()->examples[expectedIndex];


                m_graphColor = Qt::darkCyan;
                m_graphVisible = true;
            }
        }
}

Function::~Function()
{
    delete m_graph;
}

const Analitza::Expression &Function::expression() const
{
    return m_expression;
}
/*
double Function::evaluateRealValue(double arg)
{
    Q_ASSERT (m_arguments.size() == 1);
//     Q_ASSERT (m_arguments[m_analyzer.expression().parameters()[0]->name()].value); TODO ?

    m_arguments[m_analyzer.expression().parameters()[0]->name()].value->setValue(arg);

    return m_analyzer.calculateLambda().toReal().value();
}

double Function::evaluateRealValue(double arg1, double arg2)
{
//     Q_ASSERT (args.size() == m_argumentCount);

    return 0;
}

double Function::evaluateRealValue(double arg1, double arg2, double arg3)
{
//     Q_ASSERT (args.size() == m_argumentCount);

    return 0;
}

QVector2D Function::evaluateVectorValue2(double arg)
{
//     Q_ASSERT (args.size() == m_argumentCount);

    return QVector2D();
}

QVector3D Function::evaluateVectorValue3(double arg)
{
//     Q_ASSERT (args.size() == m_argumentCount);

    return QVector3D();
}

QVector3D Function::evaluateVectorValue3(double arg1, double arg2)
{
//     Q_ASSERT (args.size() == m_argumentCount);

    return QVector3D();
}*/


void Function::updateGraphData()
{
    m_graph->updateGraphData(this);
}

const FunctionGraphData * Function::graphData() const
{
    return m_graph->data();
}

QStringList Function::errors() const
{
    return m_errors;
}

bool Function::isCorrect() const
{
    return false;

}

Function Function::operator=(const Function &f)
{
    if(&f!=this)
    {
        delete m_graph;

        if(f.m_graph)
        {
            m_graph=f.m_graph->copy();
//          m_function=copy(f.m_function);
            Q_ASSERT(m_graph);
        }
        else
            m_graph=0;

        //TODO check for mem corr

        m_expression=f.m_expression;


//         m_graphVisible=f.m_graphVisible;
//         m_pen=f.m_pen;
//         m_name=f.m_name;
//         m_err=f.m_err;
    }
    return *this;
}

bool Function::operator== (const Function &f) const
{
    return false;
}

