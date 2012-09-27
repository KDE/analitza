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

#include "surface.h"

#include "analitza/localize.h"

#include "private/functiongraphfactory.h"
#include "private/abstractsurface.h"

Surface::Surface(AbstractFunctionGraph* g): FunctionGraph(g)
{}

Surface::~Surface()
{}

const QVector<Triangle3D> & Surface::faces() const
{
    Q_ASSERT(backend());

    return static_cast<AbstractSurface*>(backend())->faces;
}

void Surface::update(const Box3D& viewport)
{
    Q_ASSERT(backend());
    
    static_cast<AbstractSurface*>(backend())->update(viewport);
}

const QVector< double > Surface::vertices() const
{
    Q_ASSERT(backend());

    return static_cast<AbstractSurface*>(backend())->vertices;
}

const QVector< double > Surface::normals() const
{
    Q_ASSERT(backend());

    return static_cast<AbstractSurface*>(backend())->normals;
}

const QVector< unsigned int > Surface::indices() const
{
    Q_ASSERT(backend());

    return static_cast<AbstractSurface*>(backend())->indices;
}
