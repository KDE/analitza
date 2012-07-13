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




#include "private/abstractsurface.h"
// #include "private/surfacefactory.h"
#include "private/functiongraphfactory.h"


#include "analitza/value.h"
#include <analitza/vector.h>

//TODO macros para las prop e abajo


class ANALITZAPLOT_EXPORT Fxy : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    CONSTRUCTORS(Fxy)
    TYPE_NAME("CartesianSurfacez=f(x,y)")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("x,y")
    ICON_NAME("none")
    EXAMPLES("x*x+y,x+y*sin(x),x*y")
    
    //Own

    void update(const Box& viewport);
    
private:

};

void Fxy::update(const Box& viewport)
{
//     buildParametricSurface();
}

REGISTER_SURFACE(Fxy)

class ANALITZAPLOT_EXPORT Fxz : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    CONSTRUCTORS(Fxz)
    TYPE_NAME("CartesianSurfaceY=f(x,z)")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("x,z")
    ICON_NAME("none")
    EXAMPLES("x+z")
    
    //Own

    void update(const Box& viewport);
    
private:

};

void Fxz::update(const Box& viewport)
{
//     buildParametricSurface();
}

REGISTER_SURFACE(Fxz)

