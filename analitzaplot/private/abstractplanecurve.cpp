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

#include "abstractplanecurve.h"
#include "functiongraph.h"

#include <cmath>

#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variable.h"
#include "utils/mathutils.h"

using namespace Analitza;

AbstractPlaneCurve::AbstractPlaneCurve(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v)
    : AbstractFunctionGraph(e, v)
{}

AbstractPlaneCurve::~AbstractPlaneCurve()
{}

bool AbstractPlaneCurve::addPoint(const QPointF& p)
{
    int count=points.count();
    if(count<2) {
        points.append(p);
        return false;
    }
    
    double angle1=std::atan2(points[count-1].y()-points[count-2].y(), points[count-1].x()-points[count-2].x());
    double angle2=std::atan2(p.y()-points[count-1].y(), p.x()-points[count-1].x());
    
    bool append=!isSimilar(angle1, angle2);
    if(append)
        points.append(p);
    else
        points.last()=p;
        
    return append;
}
