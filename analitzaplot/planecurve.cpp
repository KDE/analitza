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

