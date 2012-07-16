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

#include <QUuid>

#include <KDE/KLocalizedString>

#include "analitza/analyzer.h"
#include "analitza/variable.h"
#include <analitza/variables.h>

#include "private/abstractplanecurve.h"



///

///



PlaneCurve::PlaneCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor &col, Analitza::Variables *variables )
    : FunctionGraph(functionExpression,2, name, col, variables)
{
}

PlaneCurve::~PlaneCurve()
{
}

bool PlaneCurve::canDraw(const Analitza::Expression& functionExpression)
{
return FunctionGraph::canDraw(functionExpression, 2);
}


bool PlaneCurve::canDraw(const Analitza::Expression& functionExpression, QStringList& errors)
{
return FunctionGraph::canDraw(functionExpression, 2, errors);
}

bool PlaneCurve::reset(const Analitza::Expression& functionExpression)
{
    return FunctionGraph::reset(functionExpression, 2);
}


const QVector<QPointF> & PlaneCurve::points() const
{
    Q_ASSERT(backend());
    
    //NOTE GSOC POINTS=0
    //no siempre el backend va a generar puntos y si no lo hace no quiere decir que esta mal,
    //por ejemplo en el caso de parametric se hace un clip para ver si la curva esta dentro o no del viewport
//     Q_ASSERT(static_cast<AbstractPlaneCurve*>(backend())->points.size()>1);

    return static_cast<AbstractPlaneCurve*>(backend())->points;
}

QVector< int > PlaneCurve::jumps() const
{
    Q_ASSERT(backend());
    
//     Q_ASSERT(backend()->jumps().size()>1);
    return static_cast<AbstractPlaneCurve*>(backend())->jumps;
}

void PlaneCurve::update(const QRectF& viewport)
{
    
    Q_ASSERT(backend());
    
    
    
    static_cast<AbstractPlaneCurve*>(backend())->update(viewport);
    
    
    //NOTE GSOC POINTS=0
    //no siempre el backend va a generar puntos y si no lo hace no quiere decir que esta mal,
    //por ejemplo en el caso de parametric se hace un clip para ver si la curva esta dentro o no del viewport
    Q_ASSERT(backend()->isCorrect() /*|| static_cast<AbstractPlaneCurve*>(backend())->points.size()>=2*/);

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

