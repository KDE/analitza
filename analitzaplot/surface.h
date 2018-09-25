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

#ifndef SURFACE_H
#define SURFACE_H

#include "functiongraph.h"

#include <QVector3D>

namespace Analitza {

/**
 * \class Surface
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Represents a surface in 3D space.
 *
 * This class contains all necessary data that Plotter3D will use to draw the surface.
 */

class ANALITZAPLOT_EXPORT Surface : public FunctionGraph
{
public:
    explicit Surface(AbstractFunctionGraph* g);
    
    virtual ~Surface();

    //Own

    /** Update the surfaces's data @p oppositecorner1 and @p oppositecorner2 form an axis-aligned bounding box. */
    void update(const QVector3D& oppositecorner1, const QVector3D& oppositecorner2);

    // Surface data
    QVector<QVector3D> vertices() const;
    QVector<QVector3D> normals() const;
    QVector<unsigned int> indexes() const;
};

}

#endif // SURFACE_H
