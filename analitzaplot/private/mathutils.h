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

#ifndef ANALITZAPLOT_MATHUTILS_H
#define ANALITZAPLOT_MATHUTILS_H

#include <QLineF>
#include <QPair>
#include <QVector3D>
#include "analitza/expressiontype.h"
#include <plottingenums.h>

namespace Analitza 
{

/**
 * \namespace MathUtils
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Numeric and symbolic math helpers.
 */

namespace MathUtils
{

ExpressionType createRealValuedFunctionType(Dimension fromDim = Dim1D);
ExpressionType createVectorValuedFunctionType(Dimension fromDim, Dimension toDim);
ExpressionType createListValuedFunctionType(Dimension fromDim = Dim1D);

void polarToCartesian(double radial, double polar, double &x, double &y);

void cartesianToPolar(double x, double y, double &radial, double &polar);

void cylindricalToCartesian(double radial, double polar, double height, double &x, double &y, double &z);

void sphericalToCartesian(double radial, double azimuth, double polar, double &x, double &y, double &z);

bool isSimilar(double a, double b, double diff = .0000001);

bool oppositeSign(double a, double b);

double linearInterpolation(double x, double x0, double x1, double fx0, double fx1);

//domain [x0,x1]x[y0,y1]
// southwest:f00, southeast:f10, northwest:f01, northeast:f11
double bilinearInterpolation(double x, double y, double x0, double x1, double y0, double y1, double fx0y0, double fx1y0, double fx0y1, double fx1y1);

bool traverse(double p1, double p2, double next);

QLineF slopeToLine(const double &der);

QLineF mirrorXY(const QLineF& l);
}

}

#endif // ANALITZAPLOT_MATHUTILS_H
