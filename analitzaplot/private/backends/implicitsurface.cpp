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

#include "private/abstractsurface.h"
// #include "private/surfacefactory.h"
#include "private/functiongraphfactory.h"

#include "analitza/value.h"
#include <analitza/vector.h>

#include "private/utils/marchingcubes.h"

class ImplicitSurf : public AbstractSurface , public MarchingCubes/*, static class? better macros FooClass*/
{
public:
    explicit ImplicitSurf(const Analitza::Expression& e);
    ImplicitSurf(const Analitza::Expression& e, Analitza::Variables* v);

    TYPE_NAME("implsurf")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Bool)) // is a equation
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("x,y,z")
    ICON_NAME("none")
    EXAMPLES("")

    //Own
    virtual ~ImplicitSurf() {
    }
    void update(const Box3D& viewport);
    
    double evalScalarField(double x, double y, double z);
};

double ImplicitSurf::evalScalarField(double x, double y, double z)
{
    arg("x")->setValue(x);
    arg("y")->setValue(y);
    arg("z")->setValue(z);
    
    return analyzer->calculateLambda().toReal().value();
}

ImplicitSurf::ImplicitSurf(const Analitza::Expression& e): AbstractSurface(e)
{
    buildGeometry();
}

ImplicitSurf::ImplicitSurf(const Analitza::Expression& e, Analitza::Variables* v): AbstractSurface(e)
{

}

void ImplicitSurf::update(const Box3D& viewport)
{
    faces.clear();
    faces << MarchingCubes::_faces_;
}

REGISTER_SURFACE(ImplicitSurf)
