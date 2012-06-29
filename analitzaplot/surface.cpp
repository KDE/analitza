/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "surface.h"

#include "analitza/localize.h"
#include "analitza/variable.h"


#include "private/functiongraphfactory.h"
#include "private/abstractsurface.h"

Surface::Surface(const Analitza::Expression &functionExpression, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, name, col)
{
    reset(functionExpression);
}

Surface::Surface(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, v, name, col)
{
    reset(functionExpression);
}

Surface::~Surface()
{
    delete m_surface;
}

bool Surface::canDraw(const Analitza::Expression &functionExpression) 
{
    QStringList errors;
    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() && !functionExpression.isLambda()) {
        errors << i18n("The expression is not correct");
        return false;
    }

    Analitza::Analyzer a;
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!AbstractFunctionGraphFactory::self()->contains(AbstractFunctionGraphFactory::self()->id(bvars)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_planeCurve;
//             m_planeCurve=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool Surface::canDraw(const Analitza::Expression &functionExpression, QStringList &errors)
{
    Q_ASSERT(errors.isEmpty());
    
    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() && !functionExpression.isLambda()) {
        errors << i18n("The expression is not correct");
        return false;
    }

    Analitza::Analyzer a;
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!AbstractFunctionGraphFactory::self()->contains(AbstractFunctionGraphFactory::self()->id(bvars)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_planeCurve;
//             m_planeCurve=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool Surface::reset(const Analitza::Expression& functionExpression)
{
    m_errors.clear();

    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() || !functionExpression.isLambda())
    {
        m_errors << i18n("The expression is not correct");
        return false;
    }

    Analitza::Analyzer a(m_varsModule);
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!AbstractFunctionGraphFactory::self()->contains(AbstractFunctionGraphFactory::self()->id(bvars)))
        m_errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        m_errors << a.errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {

            delete m_surface;

            m_surface=static_cast<AbstractSurface*>(AbstractFunctionGraphFactory::self()->build(AbstractFunctionGraphFactory::self()->id(bvars), a.expression(), m_varsModule));
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return m_errors.empty();
}

void Surface::setVariables(Analitza::Variables* variables)
{
    Q_ASSERT(variables);
    
    delete m_varsModule;
    
    m_varsModule = variables;
    
    m_surface->setVariables(variables);
}

const QString Surface::typeName() const
{
    Q_ASSERT(m_surface);
    
    return m_surface->typeName();
}

const Analitza::Expression & Surface::expression() const
{
    Q_ASSERT(m_surface);
    
//     return m_surface->
    return m_surface->expression();
}

QString Surface::iconName() const
{
    Q_ASSERT(m_surface);
    
    return m_surface->iconName();
}

QStringList Surface::examples() const
{
    Q_ASSERT(m_surface);
    
    return m_surface->examples();
}

int Surface::spaceDimension() const
{
    Q_ASSERT(m_surface);
    
    return m_surface->spaceDimension();
}

CoordinateSystem Surface::coordinateSystem() const
{
    Q_ASSERT(m_surface);
    
    return m_surface->coordinateSystem();
}

QStringList Surface::errors() const
{
    Q_ASSERT(m_surface);
    
    QStringList err(m_errors);
    if(m_surface) {
        err += m_surface->errors();
    }
    return err;
}

bool Surface::isCorrect() const
{
    Q_ASSERT(m_surface);

    return m_errors.isEmpty() && m_surface->isCorrect();
}

QPair<Analitza::Expression, Analitza::Expression> Surface::interval(const QString &argname, bool evaluate)  const
{
    Q_ASSERT(m_surface);
    
    return m_surface->interval(argname, evaluate);
}

bool Surface::setInterval(const QString& argname, const Analitza::Expression& min, const Analitza::Expression& max)
{
    Q_ASSERT(m_surface);
    
    return m_surface->setInterval(argname, min, max);
}

QPair<double, double> Surface::interval(const QString &argname) const
{
    Q_ASSERT(m_surface);
    
    return m_surface->interval(argname);
}

bool Surface::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(m_surface);
    
    return m_surface->setInterval(argname, min, max);
}

QStringList Surface::parameters() const
{
    Q_ASSERT(m_surface);
    
    return m_surface->parameters();
}

const QVector< int >& Surface::indexes() const
{
    return m_surface->indexes;
}
    
const QVector<QVector3D> & Surface::points() const
{
    Q_ASSERT(m_surface);

    return m_surface->points;
}

void Surface::update(const Box& viewport)
{
    Q_ASSERT(m_surface);
    
//     m_surface->update(viewport);
}
