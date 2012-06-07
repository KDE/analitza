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

namespace Keomath
{


Function::Function(const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem, bool isImplicit, const QString &name)
: m_name(name), m_graph(0)
{
    setExpression(functionExpression, coordinateSystem, isImplicit);
}

Function::Function (const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem, Analitza::Variables *variables, bool isImplicit, const QString &name)
: m_analyzer(variables), m_name (name), m_graph(0)
{
setExpression(functionExpression, coordinateSystem, isImplicit);
}

Function::Function(const Function &f)
: m_name(f.m_name)
, m_analyzer(f.m_analyzer)
, m_runStack(f.m_runStack)
, m_isImplicit(f.m_isImplicit)
, m_type(f.m_type)
, m_arguments(f.m_arguments)
, m_graphDimension(f.m_graphDimension)
, m_coordinateSystem(f.m_coordinateSystem)
, m_graphPrecision(f.m_graphPrecision)
, m_graphType(f.m_graphType)
, m_graphColor(f.m_graphColor)
, m_graphVisible(f.m_graphVisible)
, m_graph(0)
, m_errors(f.errors())
{
    if (f.m_graph)
        m_graph = f.m_graph->copy();
}

void Function::setExpression (const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem, bool isImplicitExpression)
{
    if (!functionExpression.isCorrect())
    {
        m_errors << i18n ("The expression is not correct");
    }

    m_analyzer.setExpression (functionExpression);
    m_analyzer.setExpression (m_analyzer.dependenciesToLambda());

    bool foundExpected = false;
    int expectedIndex = 0;

    for (int i = 0; i < FunctionGraphFactory::self()->constructors.size(); ++i)
    {
        if ( (m_analyzer.type().canReduceTo(FunctionGraphFactory::self()->expressionTypes[i])) &&
             (m_analyzer.expression().bvarList() == FunctionGraphFactory::self()->argumentNames[i]) &&
                (coordinateSystem == FunctionGraphFactory::self()->coordinateSystems[i]) &&
                (isImplicitExpression == FunctionGraphFactory::self()->implicitFlags[i]))
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
        if (!m_analyzer.isCorrect())
        {
            m_errors << m_analyzer.errors();
        }
        else
        {
            bool isDrawable = true;

            if (m_analyzer.expression().lambdaBody().isVector())
            {
                m_type = VectorValued;

                if (dynamic_cast<Analitza::Vector*>(m_analyzer.expression().lambdaBody().tree())->size() > 3)
                    isDrawable = false;
            }
            else
            {
                m_type = RealValued;

                if (m_analyzer.expression().parameters().size() > 3) // maximo dibujable: superficies de nivel
                    isDrawable = false;
            }

            if (!isDrawable)
                m_errors << i18n ("Function can not be drawn");
            else
            {
                m_coordinateSystem = coordinateSystem;
                m_graph = FunctionGraphFactory::self()->create(expectedIndex);

                m_isImplicit = isImplicitExpression;

                for (int i = 0; i < m_analyzer.expression().parameters().size(); ++i)
                {
                    ArgumentInfo arg;
                    arg.interval = qMakePair(-1.0, 1.0);
                    arg.value = new Analitza::Cn;

                    m_arguments[m_analyzer.expression().parameters()[i]->name()] = arg;
                    m_runStack.append(arg.value);
                }

                m_iconName = FunctionGraphFactory::self()->iconNames[expectedIndex];
                m_examples = FunctionGraphFactory::self()->examples[expectedIndex];

                m_analyzer.setStack (m_runStack);

                m_graphColor = Qt::darkCyan;
                m_graphVisible = true;
            }
        }
}

Function::~Function()
{

}

const Analitza::Expression &Function::expression() const
{
    return m_analyzer.expression();
}

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
}


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

// Keomath::Function Function::operator=(const Keomath::Function& f)
// {
//     if(&f!=this)
//     {
//         delete m_graph;
//
//         if(f.m_graph)
//         {
//             m_graph=f.m_graph->copy();
// //          m_function=copy(f.m_function);
//             Q_ASSERT(m_graph);
//         }
//         else
//             m_graph=0;
//
//         m_analyzer=Analitza::Analyzer(m_analyzer).m_expression;
//         m_show=f.m_show;
//         m_pen=f.m_pen;
//         m_name=f.m_name;
//         m_err=f.m_err;
//     }
//     return *this;
// }

// bool Function::operator== (const Function &f) const
// {
//
// }


}


Function::Function()
    : m_solver (0)
    , m_show (false)
    , m_lineWidth (0.0)
{
}

Function::Function (const QString &name, const Analitza::Expression &newFunc, Analitza::Variables *v,
                    const QPen &pen, double uplimit, double downlimit)
    : /*m_pen(pen), */m_name (name)
    , m_solver (0)
    , m_show (true)
    , m_color (Qt::green)
    , m_drawingType (Solid)
    , m_lineWidth (1)
{
    Analitza::Expression expression = newFunc;
    Analitza::Variables *variables = v;
    int dimension = 2;
    RealInterval::List domain = RealInterval::List() << RealInterval (downlimit, uplimit);

    m_id = QUuid::createUuid();
    m_dateTime = KDateTime::currentLocalDateTime();

    if (expression.isCorrect())
    {
        Analitza::Analyzer a (variables);
        a.setExpression (expression);
        a.setExpression (a.dependenciesToLambda());

        if (dimension == 2)
        {
            QStringList bvars = a.expression().bvarList();


            if (!FunctionFactory::self()->containsFor2D (bvars))
            {
                m_errors << i18n ("Function type not recognized");
            }
            else
            {
                bool correct = true;
                Analitza::ExpressionType expected = FunctionFactory::self()->typeFor2D (bvars);
                Analitza::ExpressionType actual = a.type();

                correct &= actual.canReduceTo (expected);

                if (correct)
                {
                    m_solver = static_cast<FunctionImpl2D *> (FunctionFactory::self()->item2D (bvars, a.expression(), variables));
                    m_solver->setDomain (domain);

                }
                else
                    m_errors << i18n ("Function type not correct for Functions depending on %1", bvars.join (i18n (", ")));
            }
        }
        else
            if (dimension == 3)
            {
                QStringList bvars = a.expression().bvarList();

                if (!FunctionFactory::self()->containsFor3D (bvars))
                {
                    m_errors << i18n ("Function type not recognized");

                    qDebug() << "dentro de funcion " << "pero q dbialosss" << bvars << m_errors;

                }
                else
                {
                    bool correct = true;

                    Analitza::ExpressionType expected = FunctionFactory::self()->typeFor3D (bvars);
                    Analitza::ExpressionType actual = a.type();

                    correct &= actual.canReduceTo (expected);

                    if (correct)
                    {
                        m_solver = static_cast<FunctionImpl3D *> (FunctionFactory::self()->item3D (bvars, a.expression(), variables));
                        m_solver->setDomain (domain);

                    }
                    else
                        m_errors << i18n ("Function type not correct for Functions depending on %1", bvars.join (i18n (", ")));
                }
            }

    }
    else
    {
        m_errors << i18n ("The expression is not correct");
    }
}

Function::Function (const Analitza::Expression &expression, int dimension, const RealInterval::List &domain,
                    Analitza::Variables *variables, const QString &name, const QColor &color)
    : m_solver (0)
    , m_show (true)
    , m_color (color)
    , m_name (name)
    , m_drawingType (Solid)
    , m_lineWidth (0.5)
{


    m_id = QUuid::createUuid();
    m_dateTime = KDateTime::currentLocalDateTime();

    if (expression.isCorrect())
    {
        Analitza::Analyzer a (variables);
        a.setExpression (expression);
        a.setExpression (a.dependenciesToLambda());

        if (dimension == 2)
        {
            QStringList bvars = a.expression().bvarList();


            if (!FunctionFactory::self()->containsFor2D (bvars))
            {
                m_errors << i18n ("Function type not recognized");
            }
            else
            {
                bool correct = true;
                Analitza::ExpressionType expected = FunctionFactory::self()->typeFor2D (bvars);
                Analitza::ExpressionType actual = a.type();

                correct &= actual.canReduceTo (expected);

                if (correct)
                {
                    m_solver = static_cast<FunctionImpl2D *> (FunctionFactory::self()->item2D (bvars, a.expression(), variables));
                    m_solver->setDomain (domain);

                }
                else
                    m_errors << i18n ("Function type not correct for Functions depending on %1", bvars.join (i18n (", ")));
            }
        }
        else
            if (dimension == 3)
            {
                QStringList bvars = a.expression().bvarList();

                if (!FunctionFactory::self()->containsFor3D (bvars))
                {
                    m_errors << i18n ("Function type not recognized");

                    qDebug() << "dentro de funcion " << "pero q dbialosss" << bvars << m_errors;

                }
                else
                {
                    bool correct = true;

                    Analitza::ExpressionType expected = FunctionFactory::self()->typeFor3D (bvars);
                    Analitza::ExpressionType actual = a.type();

                    correct &= actual.canReduceTo (expected);

                    if (correct)
                    {
                        m_solver = static_cast<FunctionImpl3D *> (FunctionFactory::self()->item3D (bvars, a.expression(), variables));
                        m_solver->setDomain (domain);

                    }
                    else
                        m_errors << i18n ("Function type not correct for Functions depending on %1", bvars.join (i18n (", ")));
                }
            }

    }
    else
    {
        m_errors << i18n ("The expression is not correct");
    }
}

Function::Function (const Function &function)
    : m_show (function.m_show)
    , m_color (function.m_color)
    , m_name (function.m_name)
    , m_errors (function.m_errors)
    , m_lineWidth (function.m_lineWidth)
    , m_dateTime (function.dateTime())
    , m_drawingType (function.m_drawingType)
    , m_spaceId (function.m_spaceId)
    , m_id (function.m_id)
{
    if (function.m_solver)
        m_solver = function.m_solver->copy();
}

Function::~Function()
{
    if (m_solver)
        delete m_solver;

//   foreach (Analitza::Object* obj, m_domain)
//       delete obj;
}

QPair< QPointF, QString > Function::calc (const QPointF &dp)
{
    //TODO gsoc
//     Q_ASSERT(m_function);
//     return m_function->calc(dp);

    return QPair<QPointF, QString> (QPointF(), QString());
}

void Function::update_points (const QRect &viewport)
{
    //TODO gsoc
}

const Analitza::Expression &Function::expression() const
{
    return m_solver->lambda();
}

int Function::dimension() const
{
    return m_solver->dimension();
}

Function::Axe Function::axeType() const
{
    return m_solver->axeType();
}

RealInterval::List Function::domain() const
{
    return m_solver->domain();
}

void Function::setDomain (const RealInterval::List &domain)
{
    m_solver->setDomain (domain);
}

int Function::resolution() const
{
    return m_solver->resolution();
}

void Function::setResolution (int resolution)
{
    m_solver->setResolution (resolution);
}

bool Function::isShown() const
{
    return m_show && m_solver && m_solver->isCorrect();
}

QStringList Function::errors() const
{
    QStringList err (m_errors);

    if (m_solver)
    {
        err += m_solver->errors();

    }

    return err;
}

bool Function::isCorrect() const
{
    return m_solver && m_errors.isEmpty() && m_solver->isCorrect();
}

Function Function::operator = (const Function &f)
{
    if (&f != this)
    {
        delete m_solver;

        if (f.m_solver)
        {
            m_solver = f.m_solver->copy();

            Q_ASSERT (m_solver);
        }
        else
            m_solver = 0;

        m_show = f.m_show;
        m_color = f.m_color;
        m_name = f.m_name;
        m_errors = f.m_errors;

        m_lineWidth = f.m_lineWidth;
    }

    return *this;
}
