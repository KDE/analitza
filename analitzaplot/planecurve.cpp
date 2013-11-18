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

#include "private/abstractplanecurve.h"
#include "private/functiongraphfactory.h"

using namespace Analitza;

PlaneCurve::PlaneCurve(AbstractFunctionGraph* g )
    : FunctionGraph(g)
{}

PlaneCurve::~PlaneCurve()
{}

const QVector<QPointF> & PlaneCurve::points() const
{
    Q_ASSERT(backend());
    return static_cast<AbstractPlaneCurve*>(backend())->points;
}

QVector< int > PlaneCurve::jumps() const
{
    Q_ASSERT(backend());
    return static_cast<AbstractPlaneCurve*>(backend())->jumps;
}

void PlaneCurve::update(const QRectF& viewport)
{
    AbstractPlaneCurve* b = static_cast<AbstractPlaneCurve*>(backend());
    Q_ASSERT(b);
    
    //If we already have an interval and it's already calculated
    //we don't need to do it again
    if(b->hasIntervals() && !b->points.isEmpty())
        return;
    
    b->update(viewport);
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
