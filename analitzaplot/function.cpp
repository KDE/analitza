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
#include <QUuid>

#include "analitza/analyzer.h"
#include <analitza/value.h>
#include <analitza/vector.h>
#include <analitza/variable.h>

#include "functiongraph.h"
#include "functiongraphfactory.h"

MappingGraph::MappingGraph(const QString &name, const QColor& col)
: m_name (name), m_color(col), m_plotStyle(Solid)
{
    m_id = QUuid::createUuid().toString();
}

MappingGraph::~MappingGraph()
{
}


/////

PlaneCurve::PlaneCurve(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
: Curve(name, col), m_varsModule(v), m_planeCurve(0)
{
    reset(functionExpression);
}

PlaneCurve::~PlaneCurve()
{
    delete m_planeCurve;
}

bool PlaneCurve::canReset(const Analitza::Expression &functionExpression) const
{
    QStringList tmperrs;
    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() && !functionExpression.isLambda()) {
        tmperrs << i18n("The expression is not correct");
        return false;
    }

    Analitza::Analyzer a(m_varsModule);
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());
    
    QStringList bvars;
    
    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());
    
    //TODO: turn into assertion
    if(!PlaneCurveFactory::self()->contains(bvars))                                        
        tmperrs << i18n("Function type not recognized");
    else if(!a.isCorrect())
        tmperrs << a.errors();
    else {
        Analitza::ExpressionType expected=PlaneCurveFactory::self()->expressionType(bvars);
        Analitza::ExpressionType actual=a.type();
        
        if(actual.canReduceTo(expected)) {
//             delete m_planeCurve;
//             m_planeCurve=PlaneCurveFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            tmperrs << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }
    
    return tmperrs.empty();
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

    Analitza::Analyzer a(m_varsModule);
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());
    
    QStringList bvars;
    
    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());
    
    //TODO: turn into assertion
    if(!PlaneCurveFactory::self()->contains(bvars))                                        
        m_errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        m_errors << a.errors();
    else {
        Analitza::ExpressionType expected=PlaneCurveFactory::self()->expressionType(bvars);
        Analitza::ExpressionType actual=a.type();
        
        if(actual.canReduceTo(expected)) {
            
            delete m_planeCurve;

            m_planeCurve=PlaneCurveFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
        
        
    }
    
    return m_errors.empty();
}



    const QString PlaneCurve::typeName() const
    {
        return m_planeCurve->typeName();
    }

const Analitza::Expression & PlaneCurve::expression() const
{
//     return m_planeCurve->
return m_planeCurve->expression();
}

QString PlaneCurve::iconName() const
{
return m_planeCurve->iconName();
}


QStringList PlaneCurve::examples() const
{
return m_planeCurve->examples();
}

int PlaneCurve::spaceDimension() const
{
    return m_planeCurve->spaceDimension();
}

CoordinateSystem PlaneCurve::coordinateSystem() const
{
return m_planeCurve->coordinateSystem();
}


DrawingPrecision PlaneCurve::drawingPrecision() const
{
    return m_planeCurve->drawingPrecision();
}


void PlaneCurve::setDrawingPrecision(DrawingPrecision precision)
{
    m_planeCurve->setDrawingPrecision(precision);
}

QStringList PlaneCurve::errors() const
{
    QStringList err(m_errors);
    if(m_planeCurve) {
        err += m_planeCurve->errors();
    }
    return err;
}


bool PlaneCurve::isCorrect() const
{
    if (m_planeCurve)
        return m_errors.isEmpty() && m_planeCurve->isCorrect();

    return m_errors.isEmpty();
}

    RealInterval PlaneCurve::argumentInterval(const QString &argname) const
    {
        return m_planeCurve->argumentInterval(argname);
    }
    void PlaneCurve::setArgumentInverval(const QString &argname, const RealInterval &interval)
    {
        m_planeCurve->setArgumentInverval(argname, interval);
    }
QStringList PlaneCurve::arguments() const
{
    QStringList ret;

    //TODO
//     foreach (const Analitza::Ci *var, m_expression.parameters())
//         ret.append(var->name());

    return m_planeCurve->arguments();
}

double PlaneCurve::arcLength() const
{
    return m_planeCurve->arcLength();
}

bool PlaneCurve::isClosed() const
{
    return m_planeCurve->isClosed();    
}

double PlaneCurve::area() const
{
    return m_planeCurve->area();
}

QPair<bool, double> PlaneCurve::isParallelTo(const Curve &othercurve)
{
    return m_planeCurve->isParallelTo(othercurve);
}

const QVector<QVector2D> & PlaneCurve::points() const
{
    Q_ASSERT(m_planeCurve);
    
//     Q_ASSERT(m_planeCurve->points().size()>1);
    return m_planeCurve->points();
}

QList< int > PlaneCurve::jumps() const
{
        Q_ASSERT(m_planeCurve);
//     Q_ASSERT(m_planeCurve->jumps().size()>1);
return m_planeCurve->jumps();
}

void PlaneCurve::update(const QRect& viewport)
{
    Q_ASSERT(m_planeCurve);
    m_planeCurve->update(viewport);
}


QPair< QVector2D, QString > PlaneCurve::calc(const QPointF &mousepos)
{
    Q_ASSERT(m_planeCurve);
    return m_planeCurve->calc(mousepos);
}

QLineF PlaneCurve::derivative(const QPointF &mousepos) const
{
    Q_ASSERT(m_planeCurve);
    return m_planeCurve->derivative(mousepos);
}

bool PlaneCurve::isImplicit() const
{
    return m_planeCurve->isImplicit();
}

bool PlaneCurve::isParametric() const
{
    return m_planeCurve->isParametric();
}

bool PlaneCurve::isAlgebraic() const // implicit plus only polynomails analitza work :)
{
    return m_planeCurve->isAlgebraic();
}

// PlaneCurve PlaneCurve::operator = (const PlaneCurve& f)
// {
//     if(&f!=this) {
//         delete m_planeCurve;
//         
//         if(f.m_planeCurve) {
//             m_planeCurve=dynamic_cast<AbstractCurve*>( f.m_planeCurve->copy() );
// //          m_planeCurve=copy(f.m_planeCurve);
//             Q_ASSERT(m_planeCurve);
//         } else
//             m_planeCurve=0;
//         m_expression=f.m_expression;
//         setGraphVisible(f.isGraphVisible());
//         setColor(f.color());
//         setName(f.name());
//         setId(f.id());
//         m_errors=f.m_errors;
//     }
//     return *this;
// }



