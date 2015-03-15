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

#ifndef PLOTTINGENUMS_H
#define PLOTTINGENUMS_H

#include <QFlags>

namespace Analitza {

enum Dimension
{
    Dim1D = 1,
    Dim2D = 2,
    Dim3D = 4,
    DimAll = Dim1D | Dim2D | Dim3D
};

Q_DECLARE_FLAGS(Dimensions, Dimension)

enum CoordinateSystem
{
    Cartesian = 1,
    Polar,
    Cylindrical,
    Spherical
};

enum CartesianAxis
{
    XAxis = 1,
    YAxis,
    ZAxis,
    InvalidAxis // used for query/select between some axis 
};

enum PlotStyle
{
    Dots = 1,
    Wired, 
    Solid
};

//TODO review the 3d bahaviour
enum PlottingFocusPolicy
{
    Current,
    All
};

//NOTE this is important in order to keep compatibility with KmPlot
enum GridStyle
{
    Squares = 1, // a solid quadrangular (rectangular, if keep aspect radio is off) mesh (in KmPlot is called Lines) 
    Circles, // classic polar grid: solid circles (ellipses, if keep aspect radio is off) around origin (in KmPlot is called Polar) 
    Crosses, // like KmPlot: show only a cross at every node of the quadrangular mesh
    HorizontalLines, // (or rows) show only horizontal lines of the quadrangular mesh, can be used as horizontal asymptotes
    VerticalLines, // (or columns) show only vertical lines of the quadrangular mesh, can be used as vertical asymptotes
};

enum AngleMode
{
    Radian,
    Degree,
    Gradian
};

enum ScaleMode // used for draw ticks marks in plotters
{
    Linear = 1, // -2 -1 0 1 ...
    Trigonometric, // -pi 0 +pi
//     TODO lin-lin, lin-log, log-lin, and log-log.
};

}

#endif
