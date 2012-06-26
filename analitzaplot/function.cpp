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

#include <QUuid>

#include <KDE/KLocalizedString>

#include "analitza/analyzer.h"
#include "analitza/variable.h"

#include "functiongraph.h"
#include "functiongraphfactory.h"

MappingGraph::MappingGraph(const QString &name, const QColor& col)
    : m_name (name), m_color(col)
{
    m_id = QUuid::createUuid().toString();
}

MappingGraph::~MappingGraph()
{
}

///

PlaneCurve::PlaneCurve(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
    : Curve(name, col), m_varsModule(v), m_planeCurve(0)
{
    reset(functionExpression);
}

PlaneCurve::~PlaneCurve()
{
    delete m_planeCurve;
}

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
    if(!PlaneCurveFactory::self()->contains(PlaneCurveFactory::self()->id(bvars)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=PlaneCurveFactory::self()->expressionType(PlaneCurveFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_planeCurve;
//             m_planeCurve=PlaneCurveFactory::self()->build(bvars, a.expression(), m_varsModule);
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
    if(!PlaneCurveFactory::self()->contains(PlaneCurveFactory::self()->id(bvars)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=PlaneCurveFactory::self()->expressionType(PlaneCurveFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_planeCurve;
//             m_planeCurve=PlaneCurveFactory::self()->build(bvars, a.expression(), m_varsModule);
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

    Analitza::Analyzer a(m_varsModule);
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!PlaneCurveFactory::self()->contains(PlaneCurveFactory::self()->id(bvars)))
        m_errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        m_errors << a.errors();
    else {
        Analitza::ExpressionType expected=PlaneCurveFactory::self()->expressionType(PlaneCurveFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {

            delete m_planeCurve;

            m_planeCurve=PlaneCurveFactory::self()->build(PlaneCurveFactory::self()->id(bvars), a.expression(), m_varsModule);
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));


    }

    return m_errors.empty();
}

const QString PlaneCurve::typeName() const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->typeName();
}

const Analitza::Expression & PlaneCurve::expression() const
{
    Q_ASSERT(m_planeCurve);
    
//     return m_planeCurve->
    return m_planeCurve->expression();
}

QString PlaneCurve::iconName() const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->iconName();
}

QStringList PlaneCurve::examples() const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->examples();
}

int PlaneCurve::spaceDimension() const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->spaceDimension();
}

CoordinateSystem PlaneCurve::coordinateSystem() const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->coordinateSystem();
}


// DrawingPrecision PlaneCurve::drawingPrecision() const
// {
//     Q_ASSERT(m_planeCurve);
//     
//     return m_planeCurve->drawingPrecision();
// }


// void PlaneCurve::setDrawingPrecision(DrawingPrecision precision)
// {
//     Q_ASSERT(m_planeCurve);
//     
//     m_planeCurve->setDrawingPrecision(precision);
// }

QStringList PlaneCurve::errors() const
{
    Q_ASSERT(m_planeCurve);
    
    QStringList err(m_errors);
    if(m_planeCurve) {
        err += m_planeCurve->errors();
    }
    return err;
}

bool PlaneCurve::isCorrect() const
{
    Q_ASSERT(m_planeCurve);

    return m_errors.isEmpty() && m_planeCurve->isCorrect();
}

QPair<Analitza::Expression, Analitza::Expression> PlaneCurve::interval(const QString &argname, bool evaluate) const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->interval(argname, evaluate);
}

void PlaneCurve::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(m_planeCurve);
    
    m_planeCurve->setInterval(argname, min, max);
}

QPair<double, double> PlaneCurve::interval(const QString &argname) const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->interval(argname);
}

void PlaneCurve::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(m_planeCurve);
    
    m_planeCurve->setInterval(argname, min, max);
}



QStringList PlaneCurve::parameters() const
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->parameters();
}

const QVector<QPointF> & PlaneCurve::points() const
{
    Q_ASSERT(m_planeCurve);

//     Q_ASSERT(m_planeCurve->points().size()>1);
    return m_planeCurve->points;
}

QVector< int > PlaneCurve::jumps() const
{
    Q_ASSERT(m_planeCurve);
    
//     Q_ASSERT(m_planeCurve->jumps().size()>1);
    return m_planeCurve->jumps;
}

void PlaneCurve::update(const QRect& viewport)
{
    Q_ASSERT(m_planeCurve);
    
    m_planeCurve->update(viewport);
}

QPair< QPointF, QString > PlaneCurve::image(const QPointF &mousepos)
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->image(mousepos);
}

QLineF PlaneCurve::tangent(const QPointF &mousepos)
{
    Q_ASSERT(m_planeCurve);
    
    return m_planeCurve->tangent(mousepos);
}

///















///

Surface::Surface(const Analitza::Expression &functionExpression, CoordinateSystem coordsys, Analitza::Variables *v, const QString &name, const QColor &col)
    : FunctionGraph(name, col), m_varsModule(v), m_surface(0)
{
    reset(functionExpression, coordsys);
}

Surface::~Surface()
{
    delete m_surface;
}

bool Surface::canDraw(const Analitza::Expression &functionExpression, CoordinateSystem coordsys) 
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
    if(!SurfaceFactory::self()->contains(SurfaceFactory::self()->id(bvars, coordsys)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=SurfaceFactory::self()->expressionType(SurfaceFactory::self()->id(bvars, coordsys));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_planeCurve;
//             m_planeCurve=SurfaceFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool Surface::canDraw(const Analitza::Expression &functionExpression, CoordinateSystem coordsys, QStringList &errors)
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
    if(!SurfaceFactory::self()->contains(SurfaceFactory::self()->id(bvars, coordsys)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=SurfaceFactory::self()->expressionType(SurfaceFactory::self()->id(bvars, coordsys));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_planeCurve;
//             m_planeCurve=SurfaceFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool Surface::reset(const Analitza::Expression& functionExpression, CoordinateSystem coordsys)
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
    if(!SurfaceFactory::self()->contains(SurfaceFactory::self()->id(bvars, coordsys)))
        m_errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        m_errors << a.errors();
    else {
        Analitza::ExpressionType expected=SurfaceFactory::self()->expressionType(SurfaceFactory::self()->id(bvars, coordsys));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {

            delete m_surface;

            m_surface=SurfaceFactory::self()->build(SurfaceFactory::self()->id(bvars, coordsys), a.expression(), m_varsModule);
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return m_errors.empty();
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

// DrawingPrecision Surface::drawingPrecision() const
// {
//     Q_ASSERT(m_surface);
//     
//     return m_surface->drawingPrecision();
// }
// 
// void Surface::setDrawingPrecision(DrawingPrecision precision)
// {
//     Q_ASSERT(m_surface);
//     
//     m_surface->setDrawingPrecision(precision);
// }

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

void Surface::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(m_surface);
    
    m_surface->setInterval(argname, min, max);
}

QPair<double, double> Surface::interval(const QString &argname) const
{
    Q_ASSERT(m_surface);
    
    return m_surface->interval(argname);
}

void Surface::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(m_surface);
    
    m_surface->setInterval(argname, min, max);
}



/*
RealInterval Surface::argumentInterval(const QString &argname) const
{
    Q_ASSERT(m_surface);
    
    return m_surface->argumentInterval(argname);
}
void Surface::setArgumentInverval(const QString &argname, const RealInterval &interval)
{
    Q_ASSERT(m_surface);
    
    m_surface->setArgumentInverval(argname, interval);
}*/
QStringList Surface::parameters() const
{
    QStringList ret;

    //TODO
//     foreach (const Analitza::Ci *var, m_expression.parameters())
//         ret.append(var->name());

    Q_ASSERT(m_surface);
    
    return m_surface->parameters();
}

const QVector< int >& Surface::indexes() const
{
    return QVector< int >();
}
    
const QVector<QVector3D> & Surface::points() const
{
    Q_ASSERT(m_surface);

//     Q_ASSERT(m_surface->points().size()>1);
    return m_surface->points();
}

void Surface::update()
{
    Q_ASSERT(m_surface);
    
//     m_surface->update(viewport);
}


