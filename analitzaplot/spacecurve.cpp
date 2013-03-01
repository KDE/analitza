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

#include <KDE/KLocalizedString>

#include "private/abstractspacecurve.h"

using namespace Analitza;

SpaceCurveOld::SpaceCurveOld(AbstractFunctionGraphOld* g)
    : FunctionGraph(g)
{}

SpaceCurveOld::~SpaceCurveOld()
{}

const QVector<QVector3D> & SpaceCurveOld::points() const
{
    Q_ASSERT(backend());
    return static_cast<AbstractSpaceCurve*>(backend())->points;
}

QVector< int > SpaceCurveOld::jumps() const
{
    Q_ASSERT(backend());
    return static_cast<AbstractSpaceCurve*>(backend())->jumps;
}

void SpaceCurveOld::update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2)
{
    Q_ASSERT(backend());
    static_cast<AbstractSpaceCurve*>(backend())->update(oppositecorner1, oppositecorner2);
}
