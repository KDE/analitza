/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "mathutils.h"

#include <cmath>

QPointF polarToCartesian(double radial, double polar)
{
    return QPointF(radial*cos(polar), radial*sin(polar)); 
}

void cartesianToPolar(double x, double y, double &radial, double &polar)
{
    radial = sqrt(x*x+y*y);
    polar = atan2(y,x);
}

QVector3D cylindricalToCartesian(double radial, double polar, double height)
{
    return QVector3D(radial*cos(polar), radial*sin(polar), height);
}

QVector3D sphericalToCartesian(double radial, double azimuth, double polar)
{
    return QVector3D(radial*cos(azimuth)*sin(polar), radial*sin(azimuth)*sin(polar), radial*cos(polar));
}

bool traverse(double p1, double p2, double next)
{
    static const double delta=3;
    double diff=p2-p1, diff2=next-p2;
    
    bool ret = (diff>0 && diff2<-delta) || (diff<0 && diff2>delta);
    
    return ret;
}

bool isSimilar(double a, double b, double diff)
{
    return std::fabs(a-b) < diff;
}

QLineF slopeToLine(const double &der)
{
    double arcder = atan(der);
    const double len=6.*der;
    QPointF from, to;
    from.setX(len*cos(arcder));
    from.setY(len*sin(arcder));

    to.setX(-len*cos(arcder));
    to.setY(-len*sin(arcder));
    return QLineF(from, to);
}

QLineF mirrorXY(const QLineF& l)
{
    return QLineF(l.y1(), l.x1(), l.y2(), l.x2());
}