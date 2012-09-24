/*************************************************************************************
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

#include "plotter3d.h"

#include "surface.h"
#include "spacecurve.h"

#include "plotsmodel.h"
#include "private/utils/box3d.h"
#include "private/utils/triangle3d.h"

#include <QApplication>
#include <cmath>
#include <QDebug>
#include <KLocalizedString>
#include <KColorUtils>
#include <GL/gl.h>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

Q_DECLARE_METATYPE(PlotItem*);

using namespace std;

// #define DEBUG_GRAPH

Plotter3D::Plotter3D(QAbstractItemModel* model)
    : m_model(model)
{}

Plotter3D::~Plotter3D()
{}

void Plotter3D::initGL()
{
}

void Plotter3D::setViewport(const QRect& vp)
{

}

void Plotter3D::drawPlots()
{

}

void Plotter3D::updatePlots(const QModelIndex & parent, int start, int end)
{

}

void Plotter3D::setModel(QAbstractItemModel* f)
{
    m_model=f;
    modelChanged();
}

PlotItem* Plotter3D::itemAt(int row) const
{
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim3D)
        return 0;

    return plot;
}
