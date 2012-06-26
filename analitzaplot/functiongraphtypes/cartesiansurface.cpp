/*************************************************************************************
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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


#include "functiongraph.h"
#include "functiongraphfactory.h"

#include "analitza/value.h"
#include <analitza/vector.h>

//TODO macros para las prop e abajo

class ANALITZAPLOT_EXPORT CartesianSurface : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    TYPE_NAME("CartesianSurface z=f(x,y)")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("x,y")
    ICON_NAME("none")
    EXAMPLES("x*x+y,x+y*sin(x),x*y")
    
    explicit CartesianSurface(const Analitza::Expression& expression, Analitza::Variables* variables);
    virtual ~CartesianSurface();
    
    //Own
    void update();
    
private:
};


CartesianSurface::CartesianSurface(const Analitza::Expression &expression, Analitza::Variables *variables)
    : AbstractSurface(expression, variables)
{
}

CartesianSurface::~CartesianSurface()
{
}

void CartesianSurface::update()
{

}


REGISTER_SURFACE(CartesianSurface)



