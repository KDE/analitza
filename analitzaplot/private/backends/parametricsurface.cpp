/*************************************************************************************
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




#include "private/abstractsurface.h"
// #include "private/surfacefactory.h"
#include "private/functiongraphfactory.h"


#include "analitza/value.h"
#include <analitza/vector.h>

class ParamSurf : public AbstractSurface
{
public:
    CONSTRUCTORS(ParamSurf)
    TYPE_NAME("Parametric Surface")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                Analitza::ExpressionType(Analitza::ExpressionType::Vector, Analitza::ExpressionType(
                Analitza::ExpressionType::Value), 3)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList("u") << "v")
    ICON_NAME("draw-donut")
    EXAMPLES(QStringList())

    //Own

    QVector3D fromParametricArgs(double u, double v);
    void update(const Box3D& viewport);
};

QVector3D ParamSurf::fromParametricArgs(double u, double v)
{
    arg("u")->setValue(u);
    arg("v")->setValue(v);    
    
    Analitza::Expression res = analyzer->calculateLambda();
    Analitza::Cn x=res.elementAt(0).toReal();
    Analitza::Cn y=res.elementAt(1).toReal();
    Analitza::Cn z=res.elementAt(2).toReal();
    
    return QVector3D(x.value(), y.value(), z.value());
}

void ParamSurf::update(const Box3D& viewport)
{
    buildParametricSurface();
}


REGISTER_SURFACE(ParamSurf)
