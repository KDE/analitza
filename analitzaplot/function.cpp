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

#include "functionimpl.h"
#include "functionfactory.h"

Function::Function()
    : m_solver(0)
    , m_show(false)
    , m_lineWidth(0.0)
{
}

Function::Function(const QString &name, const Analitza::Expression& newFunc, Analitza::Variables* v,
                   const QPen& pen, double uplimit, double downlimit)
    : /*m_pen(pen), */m_name(name)
    , m_solver(0)
    , m_show(true)
    , m_color(Qt::green)
    , m_drawingType(Solid)
    , m_lineWidth(1)
{
    Analitza::Expression expression = newFunc;
    Analitza::Variables *variables = v;
    int dimension = 2;
    RealInterval::List domain = RealInterval::List() << RealInterval(downlimit, uplimit);

    m_id = QUuid::createUuid();
    m_dateTime = KDateTime::currentLocalDateTime();

    if (expression.isCorrect())
    {
        Analitza::Analyzer a(variables);
        a.setExpression(expression);
        a.setExpression(a.dependenciesToLambda());

        if (dimension == 2)
        {
            QStringList bvars=a.expression().bvarList();


            if(!FunctionFactory::self()->containsFor2D(bvars))
            {
                m_errors << i18n("Function type not recognized");
            }
            else
            {
                bool correct=true;
                Analitza::ExpressionType expected=FunctionFactory::self()->typeFor2D(bvars);
                Analitza::ExpressionType actual=a.type();

                correct &= actual.canReduceTo(expected);

                if(correct)
                {
                    m_solver=static_cast<FunctionImpl2D*>(FunctionFactory::self()->item2D(bvars, a.expression(), variables));
                    m_solver->setDomain(domain);

                }
                else
                    m_errors << i18n("Function type not correct for Functions depending on %1", bvars.join(i18n(", ")));
            }
        }
        else if (dimension == 3)
        {
            QStringList bvars=a.expression().bvarList();

            if(!FunctionFactory::self()->containsFor3D(bvars))
            {
                m_errors << i18n("Function type not recognized");

                qDebug() << "dentro de funcion " << "pero q dbialosss" << bvars << m_errors;

            }
            else
            {
                bool correct=true;

                Analitza::ExpressionType expected=FunctionFactory::self()->typeFor3D(bvars);
                Analitza::ExpressionType actual=a.type();

                correct &= actual.canReduceTo(expected);

                if(correct)
                {
                    m_solver = static_cast<FunctionImpl3D*>(FunctionFactory::self()->item3D(bvars, a.expression(), variables));
                    m_solver->setDomain(domain);

                }
                else
                    m_errors << i18n("Function type not correct for Functions depending on %1", bvars.join(i18n(", ")));
            }
        }

    }
    else
    {
        m_errors << i18n("The expression is not correct");
    }
}

Function::Function(const Analitza::Expression &expression, int dimension, const RealInterval::List &domain,
                   Analitza::Variables *variables, const QString &name, const QColor &color)
    : m_solver(0)
    , m_show(true)
    , m_color(color)
    , m_name(name)
    , m_drawingType(Solid)
    , m_lineWidth(0.5)
{
    m_id = QUuid::createUuid();
    m_dateTime = KDateTime::currentLocalDateTime();

    if (expression.isCorrect())
    {
        Analitza::Analyzer a(variables);
        a.setExpression(expression);
        a.setExpression(a.dependenciesToLambda());

        if (dimension == 2)
        {
            QStringList bvars=a.expression().bvarList();


            if(!FunctionFactory::self()->containsFor2D(bvars))
            {
                m_errors << i18n("Function type not recognized");
            }
            else
            {
                bool correct=true;
                Analitza::ExpressionType expected=FunctionFactory::self()->typeFor2D(bvars);
                Analitza::ExpressionType actual=a.type();

                correct &= actual.canReduceTo(expected);

                if(correct)
                {
                    m_solver=static_cast<FunctionImpl2D*>(FunctionFactory::self()->item2D(bvars, a.expression(), variables));
                    m_solver->setDomain(domain);

                }
                else
                    m_errors << i18n("Function type not correct for Functions depending on %1", bvars.join(i18n(", ")));
            }
        }
        else if (dimension == 3)
        {
            QStringList bvars=a.expression().bvarList();

            if(!FunctionFactory::self()->containsFor3D(bvars))
            {
                m_errors << i18n("Function type not recognized");

                qDebug() << "dentro de funcion " << "pero q dbialosss" << bvars << m_errors;

            }
            else
            {
                bool correct=true;

                Analitza::ExpressionType expected=FunctionFactory::self()->typeFor3D(bvars);
                Analitza::ExpressionType actual=a.type();

                correct &= actual.canReduceTo(expected);

                if(correct)
                {
                    m_solver = static_cast<FunctionImpl3D*>(FunctionFactory::self()->item3D(bvars, a.expression(), variables));
                    m_solver->setDomain(domain);

                }
                else
                    m_errors << i18n("Function type not correct for Functions depending on %1", bvars.join(i18n(", ")));
            }
        }

    }
    else
    {
        m_errors << i18n("The expression is not correct");
    }
}

Function::Function(const Function &function)
    : m_show(function.m_show)
    , m_color(function.m_color)
    , m_name(function.m_name)
    , m_errors(function.m_errors)
    , m_lineWidth(function.m_lineWidth)
    , m_dateTime(function.dateTime())
    , m_drawingType(function.m_drawingType)
    , m_spaceId(function.m_spaceId)
    , m_id(function.m_id)
{
    if(function.m_solver)
        m_solver=function.m_solver->copy();
}

Function::~Function()
{
    if (m_solver)
        delete m_solver;
}

QPair< QPointF, QString > Function::calc(const QPointF & dp)
{
    //TODO gsoc
//     Q_ASSERT(m_function);
//     return m_function->calc(dp);

    return QPair<QPointF, QString>(QPointF(), QString());
}

void Function::update_points(const QRect& viewport)
{
    //TODO gsoc
}

const Analitza::Expression& Function::expression() const
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

void Function::setDomain(const RealInterval::List &domain)
{
    m_solver->setDomain(domain);
}

int Function::resolution() const
{
    return m_solver->resolution();
}

void Function::setResolution(int resolution)
{
    m_solver->setResolution(resolution);
}

bool Function::isShown() const
{
    return m_show && m_solver && m_solver->isCorrect();
}

QStringList Function::errors() const
{
    QStringList err(m_errors);
    if(m_solver)
    {
        err += m_solver->errors();

    }
    return err;
}

bool Function::isCorrect() const
{
    return m_solver && m_errors.isEmpty() && m_solver->isCorrect();
}

Function Function::operator = (const Function& f)
{
    if(&f!=this)
    {
        delete m_solver;

        if(f.m_solver)
        {
            m_solver=f.m_solver->copy();

            Q_ASSERT(m_solver);
        }
        else
            m_solver=0;
        m_show=f.m_show;
        m_color=f.m_color;
        m_name=f.m_name;
        m_errors=f.m_errors;

        m_lineWidth = f.m_lineWidth;
    }
    return *this;
}
