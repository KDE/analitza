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
#include "analitza/variable.h"


#include "private/functiongraphfactory.h"
#include "private/abstractsurface.h"

Surface::Surface(const Analitza::Expression &functionExpression, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, 3,name, col)
{
}

Surface::Surface(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
    : FunctionGraph(functionExpression, v,3, name, col)
{
}

Surface::~Surface()
{
}

bool Surface::canDraw(const Analitza::Expression &functionExpression) 
{
    return FunctionGraph::canDraw(functionExpression, 3);
}

bool Surface::canDraw(const Analitza::Expression &functionExpression, QStringList &errors)
{
        return FunctionGraph::canDraw(functionExpression, 3, errors);
}

bool Surface::reset(const Analitza::Expression& functionExpression)
{
    return FunctionGraph::reset(functionExpression, 3);
}


const QVector< Face >& Surface::faces() const
{
    Q_ASSERT(backend());

    return static_cast<AbstractSurface*>(backend())->faces;
}

void Surface::update(const Box& viewport)
{
    Q_ASSERT(backend());
    
    static_cast<AbstractSurface*>(backend())->update(viewport);
    

}
