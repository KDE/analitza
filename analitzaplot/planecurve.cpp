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


#include "planecurve.h"

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


PlaneCurve::PlaneCurve()
	: d(new PlaneCurveData)
{
	
}

PlaneCurve::PlaneCurve(const PlaneCurve &other)
{
	
}

PlaneCurve::PlaneCurve(const Analitza::Expression &expresssion, Variables* vars)
{
	
}

PlaneCurve::PlaneCurve(const QString &expresssion, Variables* vars)
{
	
}



//BEGIN AbstractShape interface
void PlaneCurve::clearTags()
{
	d->m_tags.clear();;
}

void PlaneCurve::addTags(const QSet<QString> &tags)
{
	d->m_tags = tags;
}

QColor PlaneCurve::color() const
{
	return d->m_color;
}

CoordinateSystem PlaneCurve::coordinateSystem() const
{
	return d->m_coordinateSystem;
}

QStringList PlaneCurve::errors() const
{
	return QStringList();
}

Expression PlaneCurve::expression() const
{
	return d->m_expression;
}

QString PlaneCurve::id() const
{
	return d->m_id;
}

QString PlaneCurve::iconName() const
{
	return d->m_iconName;
}

bool PlaneCurve::isValid() const
{
	return false;
}

bool PlaneCurve::isVisible() const
{
	return d->m_visible;
}

QString PlaneCurve::name() const
{
	return d->m_name;
}

void PlaneCurve::setColor(const QColor &color)
{
	d->m_color = color;
}

void PlaneCurve::setExpression(const Expression &expression)
{
	d->m_expression = expression; //TODO
}

void PlaneCurve::setName(const QString &name)
{
	d->m_name = name;
}

void PlaneCurve::setVisible(bool visible)
{
	d->m_visible = visible;
}

Dimension PlaneCurve::dimension() const
{
	return d->m_spaceDimension;
}

QSet<QString> PlaneCurve::tags() const
{
	return d->m_tags;
}

QString PlaneCurve::typeName() const
{
	return d->m_typeName;
}

Variables * PlaneCurve::variables() const
{
	return d->m_analyzer->variables();
}


bool PlaneCurve::operator==(const PlaneCurve &other) const
{
	return false; // TODO
}

bool PlaneCurve::operator!=(const PlaneCurve &other) const
{
	return !((*this) == other);
}

PlaneCurve & PlaneCurve::operator=(const PlaneCurve &other)
{
	return *this; //TODO
}
//END AbstractShape interface

//BEGIN AbstractFunctionGraph interface
QStringList PlaneCurve::arguments() const
{
	return d->m_arguments.keys();
}

QPair<Expression, Expression> PlaneCurve::limits(const QString &arg) const
{
	return qMakePair(Expression(), Expression());
}

QStringList PlaneCurve::parameters() const
{
	return QStringList(); //TODO
}

void PlaneCurve::setLimits(const QString &arg, double min, double max)
{
	
}

void PlaneCurve::setLimits(const QString &arg, const Expression &min, const Expression &max)
{
	
}

//END AbstractFunctionGraph interface

QPair<QPointF, QString> PlaneCurve::image(const QPointF &mousepos)
{
	return qMakePair(QPointF(), QString());
}

QVector<int> PlaneCurve::jumps() const
{
	return d->m_jumps;
}

QVector<QPointF> PlaneCurve::points() const
{
	return d->m_points;
}

QLineF PlaneCurve::tangent(const QPointF &mousepos)
{
	return QLineF(); //TODO
}

void PlaneCurve::update(const QRectF& viewport)
{
	
}

//BEGIN static AbstractShape interface
QStringList PlaneCurve::builtinMethods()
{
	return QStringList();
}

bool PlaneCurve::canBuild(const Expression &expression, Analitza::Variables* vars)
{
	return true;
}
//END static AbstractShape interface
