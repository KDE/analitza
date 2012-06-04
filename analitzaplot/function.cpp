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

#include "functionimpl.h"
#include "functionfactory.h"

namespace Keomath
{
	//esta clase se debe user si osi con su graph asociado ni no hay uno y se quier evauluar usar analyzer ...
Function::Function(const Analitza::Expression &expression,
					Analitza::Variables *variables,
                    const FunctionGraphDescription &graphDescription,
                    bool hasImplicitExpression,
                    const QString &name)
    : m_analyzer(variables), m_name (name), m_valueCount(-1), m_imageType(InvalidValued)
{
    m_id = QUuid::createUuid();


    if (!expression.isCorrect())
    {
        m_errors << i18n ("The expression is not correct");
        return;
    }

    m_analyzer.setExpression (expression);
    m_analyzer.setExpression (m_analyzer.dependenciesToLambda());

    bool foundExpected = false;
    int expectedIndex = 0;

    for (int i = 0; i < FunctionGraphFactory::self()->functionGraphConstructors.size(); ++i)
    {
        if ( (m_analyzer.type().canReduceTo(FunctionGraphFactory::self()->functionGraphExpressionTypes[i])) &&
                (graphDescription.dimension == FunctionGraphFactory::self()->functionGraphDimensions[i]) &&
                (graphDescription.coordinateSystem == FunctionGraphFactory::self()->functionGraphCoordinateSystems[i]) &&
                (hasImplicitExpression == FunctionGraphFactory::self()->functionGraphImplicitExpressionFlags[i]))
        {
            foundExpected = true;
            expectedIndex = i;

            break;
        }
    }

    if (!foundExpected)
        m_errors << i18n ("Function type not recognized");
    else
        if (!m_analyzer.isCorrect())
            m_errors << m_analyzer.errors();
        else
        {
            m_functionGraph = FunctionGraphFactory::self()->create (expectedIndex);

            m_graphDescription = graphDescription;
            m_hasImplicitExpression = hasImplicitExpression;
            m_color = Qt::darkCyan;

            m_argumentCount = m_analyzer.expression().parameters().size();

            for (int i = 0; i < m_argumentCount; ++i)
            {
                m_arguments.append (new Analitza::Cn);
                m_runStack.append (m_arguments.last());

				m_domain.append(qMakePair(-1.0, 1.0));
            }

            m_analyzer.setStack (m_runStack);

            if (m_analyzer.expression().lambdaBody().isVector())
            {
                m_imageType = VectorValued;
                m_valueCount = static_cast<Analitza::Vector *> (m_analyzer.expression().lambdaBody().tree())->size();
            }
            else
            {
                m_imageType = RealValued;
                m_valueCount = 1; //single valued function
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

void Function::setDomain (const IntervalList &domain)
{

}

VectorXd Function::evaluate (const VectorXd &args)
{
    Q_ASSERT (args.size() == m_argumentCount);

    for (int i = 0; i < m_argumentCount; ++i)
        m_arguments[i]->setValue (args[i]);

    VectorXd ret;

    switch (m_imageType)
    {
        case RealValued:
            ret = VectorXd(1);
			ret(0) = m_analyzer.calculateLambda().toReal().value();
            break;

        case VectorValued:
            break;
    }

    return ret;
}

QStringList Function::errors() const
{
    return m_errors;
}

bool Function::isCorrect() const
{
    return false;

}



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
