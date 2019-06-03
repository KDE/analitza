/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef ANALITZAPLOT_PLANECURVE_H
#define ANALITZAPLOT_PLANECURVE_H

#include "functiongraph.h"

#include <QMap>
#include <QPoint>
#include <QRectF>
#include <QLineF>
#include <QColor>

namespace Analitza
{

/**
 * \class PlaneCurve
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Represents a curve in 2D space.
 *
 * This class contains all necessary data that Plotter2D will use to draw the curve.
 */

class ANALITZAPLOT_EXPORT PlaneCurve : public FunctionGraph 
{
public:
    explicit PlaneCurve(AbstractFunctionGraph* g);
    virtual ~PlaneCurve();

    QVector<int> jumps() const;
    const QVector<QPointF> & points() const;
    void update(const QRectF& viewport);
    QPair<QPointF, QString> image(const QPointF &mousepos); // calculate the image of the curve based on the mouse postion 
    QLineF tangent(const QPointF &mousepos);// calculate the tangent to the curve based on the mouse 
};

}

#endif // ANALITZAPLOT_PLANECURVE_H
