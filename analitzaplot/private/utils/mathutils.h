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

enum Dimension {
    All = 0,
    Dim1D = 1,
    Dim2D,
    Dim3D,
    DimAll
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

bool isSimilar(double a, double b, double diff = .0000001);

bool traverse(double p1, double p2, double next);

#endif
