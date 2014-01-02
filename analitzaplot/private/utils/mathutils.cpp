/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

double radiansToDegrees(double rad)
{
    return rad*180.0/M_PI;
}

double radiansToGradians(double rad)
{
    return rad*200.0/M_PI;
}

QPointF polarToCartesian(double radial, double polar)
{
    return QPointF(radial*std::cos(polar), radial*std::sin(polar)); 
}

void cartesianToPolar(double x, double y, double &radial, double &polar)
{
    radial = std::sqrt(x*x+y*y);
    polar = std::atan2(y,x);
}

QVector3D cylindricalToCartesian(double radial, double polar, double height)
{
    return QVector3D(radial*std::cos(polar), radial*std::sin(polar), height);
}

QVector3D sphericalToCartesian(double radial, double azimuth, double polar)
{
    return QVector3D(radial*std::cos(azimuth)*std::sin(polar), radial*std::sin(azimuth)*std::sin(polar), radial*std::cos(polar));
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

bool oppositeSign(double a, double b)
{
    return ((a > 0 && b <= 0) || (a <= 0 && b > 0));
}

bool inInterval(double val, double from, double to)
{
    return (from <= val && val <= to);
}

QLineF slopeToLine(const double &der)
{
    double arcder = std::atan(der);
    const double len=6.*der;
    QPointF from, to;
    from.setX(len*std::cos(arcder));
    from.setY(len*std::sin(arcder));

    to.setX(-len*std::cos(arcder));
    to.setY(-len*std::sin(arcder));
    
    return QLineF(from, to);
}

QLineF mirrorXY(const QLineF& l)
{
    return QLineF(l.y1(), l.x1(), l.y2(), l.x2());
}

unsigned int gcd(unsigned int u, unsigned int v)
{
    int shift;

    /* GCD(0,v) == v; GCD(u,0) == u, GCD(0,0) == 0 */
    if (u == 0) return v;
    if (v == 0) return u;

    /* Let shift := lg K, where K is the greatest power of 2
          dividing both u and v. */
    for (shift = 0; ((u | v) & 1) == 0; ++shift) {
        u >>= 1;
        v >>= 1;
    }

    while ((u & 1) == 0)
        u >>= 1;

    /* From here on, u is always odd. */
    do {
        /* remove all factors of 2 in v -- they are not common */
        /*   note: v is not zero, so while will terminate */
        while ((v & 1) == 0)  /* Loop X */
            v >>= 1;

        /* Now u and v are both odd. Swap if necessary so u <= v,
           then set v = v - u (which is even). For bignums, the
           swapping is just pointer movement, and the subtraction
           can be done in-place. */
        if (u > v) {
            unsigned int t = v;
            v = u;
            u = t;
        }  // Swap u and v.
        v = v - u;                       // Here v >= u.
    } while (v != 0);

    /* restore common factors of 2 */
    return u << shift;
}

QPair< int unsigned , int unsigned > simplifyFraction(unsigned int n, int unsigned d)
{
    const unsigned int cf = gcd(n,d);
    
    return QPair< int unsigned , int unsigned >(n/cf, d/cf); // first=new n, second=new d
}
