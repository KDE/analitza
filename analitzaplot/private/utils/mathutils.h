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

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QLineF>
#include <QVector3D>

enum Dimension {
    Dim1D = 1,
    Dim2D = 2,
    Dim3D = 4,
    DimAll = Dim1D | Dim2D | Dim3D
};

enum CoordinateSystem {
    Cartesian = 1,
    Polar,
    Cylindrical,
    Spherical
};

enum CartesianAxis {
    XAxis = 1,
    YAxis,
    ZAxis
};

enum PolarAxis { R = 1, p };

QPointF polarToCartesian(double radial, double polar);

QVector3D cylindricalToCartesian(double radial, double polar, double height);

QVector3D sphericalToCartesian(double radial, double azimuth, double polar);

bool isSimilar(double a, double b, double diff = .0000001);

bool traverse(double p1, double p2, double next);

QLineF slopeToLine(const double &der);

#endif
