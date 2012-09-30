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

#include <analitza/value.h>
#include <analitza/vector.h>
#include <analitza/localize.h>
#include <QVector3D>

using namespace Analitza;

//TODO macros para las prop e abajo

//NOTE 
//FUTURE
//TODO GSOC Equipotential Curve -> Contour Plot : se puede lograr separando el tipo de superfcie
//por ejemplo una suprficies generada por un f(a,b) posee isolines ... 
//de esta manera es posible tener una funcion f(a,b,c,d) y dibujar sus contourplot3d
//esta es la propuesta:
// class SurfaceByFunctionOfTwoVars // cambiar el nombre obviamente
// {
//     ...
// public:
//     setISOCONTOURSValues(lista de valores constantes)
//     setISOCONTOURSVale(un solo valor constante) -- un solo valor al que todos los cntornos son equipotenciales
//     ...
// }
//

class  Fxy : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    CONSTRUCTORS(Fxy)
    TYPE_NAME(I18N_NOOP("Surface z=F(x,y)"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList("x") << "y")
    ICON_NAME("newfunction3d")
    EXAMPLES(QStringList("x*x+y") << "x+y*sin(x)" << "x*y")

    //Own

    QVector3D fromParametricArgs(double u, double v);
    void update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2);
};

QVector3D Fxy::fromParametricArgs(double u, double v)
{
    arg("x")->setValue(u);
    arg("y")->setValue(v);    
    
    return QVector3D(u,v,analyzer->calculateLambda().toReal().value());
}

void Fxy::update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2)
{
    buildParametricSurface();
//     vertices.clear();
//     normals.clear();
//     indices.clear();
// 
//     vertices << 0 << 0 << 0 << 0 << 0 << 1 << 1 << 0 << 0; // 3 por cada index
//     normals << 0 << 1 << 0; // 1 por primitiva = 1 por face (tri-face)-1por cada 3 indx
//     indices << 0 << 1 << 2; // enumeracion empieza desde cero
}

REGISTER_SURFACE(Fxy)

class Fxz : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    CONSTRUCTORS(Fxz)
    TYPE_NAME(I18N_NOOP("Surface y=F(x,z)"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList("x") << "z")
    ICON_NAME("newfunction3d")
    EXAMPLES(QStringList("x+z"))

    QVector3D fromParametricArgs(double u, double v);
    void update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2);
};

QVector3D Fxz::fromParametricArgs(double u, double v)
{
    arg("x")->setValue(u);
    arg("z")->setValue(v);    
    
    return QVector3D(u,analyzer->calculateLambda().toReal().value(),v);
}

void Fxz::update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2)
{
    buildParametricSurface();
}

REGISTER_SURFACE(Fxz)

