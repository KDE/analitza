/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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
#include <QPair>
#include <QVector3D>

double radiansToDegrees(double rad);

double radiansToGradians(double rad);

QPointF polarToCartesian(double radial, double polar);

void cartesianToPolar(double x, double y, double &radial, double &polar);

QVector3D cylindricalToCartesian(double radial, double polar, double height);

QVector3D sphericalToCartesian(double radial, double azimuth, double polar);

bool isSimilar(double a, double b, double diff = .0000001);

bool oppositeSign(double a, double b);

bool inInterval(double val, double from, double to);

bool traverse(double p1, double p2, double next);

QLineF slopeToLine(const double &der);

QLineF mirrorXY(const QLineF& l);

// greatest common divisor of two nonnegative integers (Binary GCD algorithm, iterative version, source: Wikipedia Dec. 2013)
unsigned int gcd(unsigned int u, unsigned int v);

QPair<unsigned int, unsigned int> simplifyFraction(unsigned int n, unsigned int d);

#endif
