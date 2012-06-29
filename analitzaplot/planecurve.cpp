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


#include "planecurve.h"

#include <QUuid>

#include <KDE/KLocalizedString>

#include "analitza/analyzer.h"
#include "analitza/variable.h"
#include <analitza/variables.h>

#include "private/abstractplanecurve.h"
#include "private/abstractfunctiongraphfactory.h"



///

///



PlaneCurve::PlaneCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, name, col)
{
    reset(functionExpression);
}

PlaneCurve::PlaneCurve(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, v,name, col)
{
    reset(functionExpression);
}

PlaneCurve::~PlaneCurve()
{
}
/*
bool PlaneCurve::canDraw(const Analitza::Expression &functionExpression)
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
    {
        errors << i18n("Function type not recognized");
    }
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete backend();
//             backend()=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool PlaneCurve::canDraw(const Analitza::Expression &functionExpression, QStringList &errors)
{
    Q_ASSERT(errors.isEmpty()); // el usuario deberia ingresar un lista vacia 
    
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
//             delete backend();
//             backend()=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool PlaneCurve::reset(const Analitza::Expression& functionExpression)
{
    m_errors.clear();

    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() || !functionExpression.isLambda())
    {
        m_errors << i18n("The expression is not correct");
        return false;
    }
    
    Analitza::Analyzer *a;
    if (m_varsModule)
        a = new Analitza::Analyzer(m_varsModule);
    else
        a = new Analitza::Analyzer;

    a->setExpression(functionExpression);
    a->setExpression(a->dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a->expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!AbstractFunctionGraphFactory::self()->contains(AbstractFunctionGraphFactory::self()->id(bvars)))
        m_errors << i18n("Function type not recognized");
    else if(!a->isCorrect())
        m_errors << a->errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a->type();

        if(actual.canReduceTo(expected)) {

            delete backend();

            if (m_varsModule)
                backend()= static_cast<AbstractPlaneCurve*>(AbstractFunctionGraphFactory::self()->build(AbstractFunctionGraphFactory::self()->id(bvars), a->expression(), m_varsModule));
            else
                backend()=static_cast<AbstractPlaneCurve*>(AbstractFunctionGraphFactory::self()->build(AbstractFunctionGraphFactory::self()->id(bvars), a->expression()));
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));

    }
    
    delete a;
    
    return m_errors.empty();
}

void PlaneCurve::setVariables(Analitza::Variables* variables)
{
    Q_ASSERT(variables);
    
    m_varsModule = variables;
    
    backend()->setVariables(variables);
}

const QString PlaneCurve::typeName() const
{
    Q_ASSERT(backend());
    
    return backend()->typeName();
}

const Analitza::Expression & PlaneCurve::expression() const
{
    Q_ASSERT(backend());
    
//     return backend()->
    return backend()->expression();
}

QString PlaneCurve::iconName() const
{
    Q_ASSERT(backend());
    
    return backend()->iconName();
}

QStringList PlaneCurve::examples() const
{
    Q_ASSERT(backend());
    
    return backend()->examples();
}

int PlaneCurve::spaceDimension() const
{
    Q_ASSERT(backend());
    
    return backend()->spaceDimension();
}

CoordinateSystem PlaneCurve::coordinateSystem() const
{
    Q_ASSERT(backend());
    
    return backend()->coordinateSystem();
}

QStringList PlaneCurve::errors() const
{
    Q_ASSERT(backend());
    
    QStringList err(m_errors);
    if(backend()) {
        err += backend()->errors();
    }
    return err;
}

bool PlaneCurve::isCorrect() const
{
    Q_ASSERT(backend());

    return m_errors.isEmpty() && backend()->isCorrect();
}

QPair<Analitza::Expression, Analitza::Expression> PlaneCurve::interval(const QString &argname, bool evaluate) const
{
    Q_ASSERT(backend());
    
    return backend()->interval(argname, evaluate);
}

bool PlaneCurve::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(backend());
    
    return backend()->setInterval(argname, min, max);
}

QPair<double, double> PlaneCurve::interval(const QString &argname) const
{
    Q_ASSERT(backend());
    
    return backend()->interval(argname);
}

bool PlaneCurve::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(backend());
    
    return backend()->setInterval(argname, min, max);
}

QStringList PlaneCurve::parameters() const
{
    Q_ASSERT(backend());
    
    return backend()->parameters();
}
*/
const QVector<QPointF> & PlaneCurve::points() const
{
    Q_ASSERT(backend());

//     Q_ASSERT(backend()->points().size()>1);
    return static_cast<AbstractPlaneCurve*>(backend())->points;
}

QVector< int > PlaneCurve::jumps() const
{
    Q_ASSERT(backend());
    
//     Q_ASSERT(backend()->jumps().size()>1);
    return static_cast<AbstractPlaneCurve*>(backend())->jumps;
}

void PlaneCurve::update(const QRect& viewport)
{
    
    Q_ASSERT(backend());
    
    static_cast<AbstractPlaneCurve*>(backend())->update(viewport);
}

QPair< QPointF, QString > PlaneCurve::image(const QPointF &mousepos)
{
    Q_ASSERT(backend());
    
    return static_cast<AbstractPlaneCurve*>(backend())->image(mousepos);
}

QLineF PlaneCurve::tangent(const QPointF &mousepos)
{
    Q_ASSERT(backend());
    
    return static_cast<AbstractPlaneCurve*>(backend())->tangent(mousepos);
}

