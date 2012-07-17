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


#ifndef ANALITZAPLOT_FUNCTIONUTILS__G
#define ANALITZAPLOT_FUNCTIONUTILS__G

#include <cmath>
#include <limits>

#include "box3d.h"
#include "triangle3d.h"

#include "analitza/expression.h"
#include "analitza/value.h"
#include "analitza/analyzer.h"

#include <QPair>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <QLineF>

using std::acos;
using std::atan;
using std::fabs;
using std::cos;
using std::sin;
using std::sqrt;


const double PI = std::acos(-1.0);

namespace Analitza
{
class Expression;
}

enum CoordinateSystem { Cartesian = 1, Polar, Cylindrical, Spherical };
enum CartesianAxis { XAxis = 1, YAxis, ZAxis };
enum PolarAxis { R = 1, p };

static bool isSimilar(double a, double b, double diff = .0000001)
{
    return fabs(a-b) < diff;
}

static bool traverse(double p1, double p2, double next)
{
    static const double delta=3;
    double diff=p2-p1, diff2=next-p2;
    
    bool ret = (diff>0 && diff2<-delta) || (diff<0 && diff2>delta);
    
    return ret;
}

static QLineF slopeToLine(const double &der)
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

static QLineF mirrorXY(const QLineF& l)
{
    return QLineF(l.y1(), l.x1(), l.y2(), l.x2());
}

//NOTE
//qvector3d/qpointf no soporta doble precision, es mejor trabajar los calculos en double y para mostrarlos usar recien qvector3d
//por esa razon no pongo un return qvector3d y pongo las nuevas coords by ref

static QPointF polarToCartesian(double radial, double polar)
{
    return QPointF(radial*cos(polar), radial*sin(polar)); 
}

static QVector3D cylindricalToCartesian(double radial, double polar, double height)
{
    return QVector3D(radial*cos(polar), radial*sin(polar), height);
}

static QVector3D sphericalToCartesian(double radial, double azimuth, double polar)
{
    return QVector3D(radial*cos(azimuth)*sin(polar), radial*sin(azimuth)*sin(polar), radial*cos(polar));
}

static Box3D createCube(const QVector3D &center, double halfEdge)
{
    QVector3D corner1(center.x()-halfEdge, center.y()-halfEdge, center.z()-halfEdge);
    QVector3D corner2(center.x()+halfEdge, center.y()+halfEdge, center.z()+halfEdge);
    
    return Box3D(corner1, corner2);
}


#endif // ANALITZAPLOT_FUNCTIONUTILS
