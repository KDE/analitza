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


#include "curve.h"

#include <KDE/KLocalizedString>

#include "private/abstractplanecurve.h"

#include "private/functiongraphfactory.h"

using namespace Analitza;

PlaneCurveOld::PlaneCurveOld(AbstractFunctionGraphOld* g )
    : FunctionGraph(g)
{}

PlaneCurveOld::~PlaneCurveOld()
{}

const QVector<QPointF> & PlaneCurveOld::points() const
{
    Q_ASSERT(backend());
    return static_cast<AbstractPlaneCurve*>(backend())->points;
}

QVector< int > PlaneCurveOld::jumps() const
{
    Q_ASSERT(backend());
    return static_cast<AbstractPlaneCurve*>(backend())->jumps;
}

void PlaneCurveOld::update(const QRectF& viewport)
{
    AbstractPlaneCurve* b = static_cast<AbstractPlaneCurve*>(backend());
    Q_ASSERT(b);
    
    //If we already have an interval and it's already calculated
    //we don't need to do it again
    if(b->hasIntervals() && !b->points.isEmpty())
        return;
    
    b->update(viewport);
}

QPair< QPointF, QString > PlaneCurveOld::image(const QPointF &mousepos)
{
    Q_ASSERT(backend());
    return static_cast<AbstractPlaneCurve*>(backend())->image(mousepos);
}

QLineF PlaneCurveOld::tangent(const QPointF &mousepos)
{
    Q_ASSERT(backend());
    return static_cast<AbstractPlaneCurve*>(backend())->tangent(mousepos);
}


///

PlaneCurveData::PlaneCurveData()
{

}

PlaneCurveData::PlaneCurveData(const PlaneCurveData& other)
{

}

PlaneCurveData::PlaneCurveData(const Expression& expresssion, Variables* vars)
{

}

// PlaneCurveData::~PlaneCurveData()
// {
// 
// }



///


Curve::Curve()
	: d(new PlaneCurveData)
{
	
}

Curve::Curve(const Curve &other)
{
	
}

Curve::Curve(const Analitza::Expression &expresssion, Variables* vars)
{
	
}

Curve::Curve(const QString &expresssion, Variables* vars)
{
	
}



//BEGIN AbstractShape interface
void Curve::clearTags()
{
	d->m_tags.clear();;
}

void Curve::addTags(const QSet<QString> &tags)
{
	d->m_tags = tags;
}

QColor Curve::color() const
{
	return d->m_color;
}

CoordinateSystem Curve::coordinateSystem() const
{
	return d->m_coordinateSystem;
}

QStringList Curve::errors() const
{
	return QStringList();
}

Expression Curve::expression() const
{
	return d->m_expression;
}

QString Curve::id() const
{
	return d->m_id;
}

QString Curve::iconName() const
{
	return d->m_iconName;
}

bool Curve::isValid() const
{
	return false;
}

bool Curve::isVisible() const
{
	return d->m_visible;
}

QString Curve::name() const
{
	return d->m_name;
}

void Curve::setColor(const QColor &color)
{
	d->m_color = color;
}

void Curve::setExpression(const Expression &expression)
{
	d->m_expression = expression; //TODO
}

void Curve::setName(const QString &name)
{
	d->m_name = name;
}

void Curve::setVisible(bool visible)
{
	d->m_visible = visible;
}

Dimension Curve::dimension() const
{
	return d->m_spaceDimension;
}

QSet<QString> Curve::tags() const
{
	return d->m_tags;
}

QString Curve::typeName() const
{
	return d->m_typeName;
}

Variables * Curve::variables() const
{
	return d->m_analyzer->variables();
}


bool Curve::operator==(const Curve &other) const
{
	return false; // TODO
}

bool Curve::operator!=(const Curve &other) const
{
	return !((*this) == other);
}

Curve & Curve::operator=(const Curve &other)
{
	return *this; //TODO
}
//END AbstractShape interface

//BEGIN AbstractFunctionGraph interface
QStringList Curve::arguments() const
{
	return d->m_arguments.keys();
}

QPair<Expression, Expression> Curve::limits(const QString &arg) const
{
	return qMakePair(Expression(), Expression());
}

QStringList Curve::parameters() const
{
	return QStringList(); //TODO
}

void Curve::setLimits(const QString &arg, double min, double max)
{
	
}

void Curve::setLimits(const QString &arg, const Expression &min, const Expression &max)
{
	
}

//END AbstractFunctionGraph interface

QPair<QPointF, QString> Curve::image(const QPointF &mousepos)
{
	return qMakePair(QPointF(), QString());
}

QVector<int> Curve::jumps() const
{
	return d->m_jumps;
}

QVector<QPointF> Curve::points() const
{
	return d->m_points;
}

QLineF Curve::tangent(const QPointF &mousepos)
{
	return QLineF(); //TODO
}

void Curve::update(const QRectF& viewport)
{
	
}

//BEGIN static AbstractShape interface
QStringList Curve::builtinMethods()
{
	return QStringList();
}

bool Curve::canBuild(const Expression &expression, Analitza::Variables* vars)
{
	return true;
}
//END static AbstractShape interface
