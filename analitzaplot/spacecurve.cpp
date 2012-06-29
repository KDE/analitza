/*************************************************************************************
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


#include "spacecurve.h"

#include <QUuid>

#include <KDE/KLocalizedString>

#include "analitza/analyzer.h"
#include "analitza/variable.h"
#include <analitza/variables.h>

#include "private/abstractspacecurve.h"



///

///



SpaceCurve::SpaceCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, name, col)
{
    reset(functionExpression);
}

SpaceCurve::SpaceCurve(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, v,name, col)
{
    reset(functionExpression);
}

SpaceCurve::~SpaceCurve()
{
}

const QVector<QVector3D> & SpaceCurve::points() const
{
    Q_ASSERT(backend());

//     Q_ASSERT(backend()->points().size()>1);
    return static_cast<AbstractSpaceCurve*>(backend())->points;
}

QVector< int > SpaceCurve::jumps() const
{
    Q_ASSERT(backend());
    
//     Q_ASSERT(backend()->jumps().size()>1);
    return static_cast<AbstractSpaceCurve*>(backend())->jumps;
}

void SpaceCurve::update(const Box& viewport)
{
    
    Q_ASSERT(backend());
    
    static_cast<AbstractSpaceCurve*>(backend())->update(viewport);
}
