/*************************************************************************************
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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


#ifndef ANALITZAPLOT_QUADTREE_H
#define ANALITZAPLOT_QUADTREE_H
#include <qpoint.h>
#include <QList>
#include <QPair>
#include <analitza/value.h>
#include <analitza/analyzer.h>

namespace Analitza  {
namespace MathUtils {

class QuadTree
{
public:
    QuadTree(double x, double y, double size);
    ~QuadTree();
    
    double x; // center x
    double y; // center x
    double size; // edge size of box
    
    QuadTree *nodes[4]; // 0:NE 1:NW 2:SW 3:SE
};



}
}

#endif // ANALITZAPLOT_QUADTREE_H
